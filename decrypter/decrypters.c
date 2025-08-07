// decrypter/decrypters.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>
#include "mta_rand.h"
#include "mta_crypt.h"
#include "utils.h" 

#define PIPE_DIR    "/mnt/mta"
#define MAIN_PIPE   PIPE_DIR "/server_pipe"
#define LOG_FILE_PATH  "/var/log/mtacrypt.log"
#define BUF_LEN     512

int main() {
    int id = get_next_client_id();
    
    init_logger(LOG_FILE_PATH);
    
    client_log_to_file("INFO", id, "Sent connect request to server");
           
    if (MTA_crypt_init() != MTA_CRYPT_RET_OK) {
        client_log_to_file("ERROR", id, "MTA_crypt_init failed");
        exit(EXIT_FAILURE);
    }

    char my_pipe[128];
    snprintf(my_pipe, sizeof(my_pipe), PIPE_DIR "/decrypter_pipe_%d", id);
    if (mkfifo(my_pipe, 0666) < 0 && errno != EEXIST) {
        client_log_to_file("ERROR", id, "open(%s): %s", MAIN_PIPE, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int fd_main = open(MAIN_PIPE, O_WRONLY | O_NONBLOCK);
    if (fd_main < 0) {
        client_log_to_file("ERROR", id, "open(%s): %s", my_pipe, strerror(errno));
        exit(EXIT_FAILURE);
    }
    write(fd_main, my_pipe, strlen(my_pipe));
    close(fd_main);

    int fd_my = open(my_pipe, O_RDWR | O_NONBLOCK);
    if (fd_my < 0) {
        client_log_to_file("ERROR", id, "open(%s): %s", my_pipe, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buf[BUF_LEN];
    int first = 1;

    while (1) {
        ssize_t n = read(fd_my, buf, sizeof(buf));
        int key_len=(int)n/8;
         if (n < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                usleep(100);
                continue;
            }

            client_log_to_file("ERROR", id, "read(%s): %s", my_pipe, strerror(errno));
            usleep(100);
            continue;
        }

        if (n == 0) {
            usleep(100);
            continue;
        }

        if (first) {
            client_log_to_file("INFO", id, "Received encrypted password");
            first = 0;
        } else {
            client_log_to_file("INFO", id, "Received new encrypted password %.*s", (int)n, buf);
        }

        unsigned int decrypted_len;
        char *decrypted = malloc(n + 1);
        char *guess_key = malloc(key_len+1);
        if (!decrypted || !guess_key) {
            client_log_to_file("ERROR", id, "malloc failed");
            free(decrypted);
            free(guess_key);
            continue;
        }

        int iterations = 0;
        while (1) {
            char tmp[BUF_LEN];
            ssize_t rn = read(fd_my, tmp, sizeof(tmp));
            if (rn > 0) {
                memcpy(buf, tmp, rn);
                n = rn;
                buf[n] = '\0';
                iterations = 0;
                client_log_to_file("INFO", id, "Received new encrypted password %.*s", (int)n, buf);
                continue;
            }
        
            iterations++;
            MTA_get_rand_data(guess_key, key_len);
            
            if (MTA_decrypt(guess_key, key_len, buf, (unsigned int)n, decrypted, &decrypted_len) == MTA_CRYPT_RET_OK
                && is_all_printable(decrypted, decrypted_len)) {

                decrypted[decrypted_len] = '\0';
                client_log_to_file("INFO", id, "Decrypted password: %.*s, Key: %.*s (in %d iterations)", decrypted_len, decrypted, key_len, guess_key, iterations);
                
                int fd_back = open(MAIN_PIPE, O_WRONLY | O_NONBLOCK);
                if (fd_back < 0) {
                    client_log_to_file("ERROR", id, "Could not send password back to server: %s", strerror(errno));
                    usleep(100);
                    fd_back = open(MAIN_PIPE, O_WRONLY | O_NONBLOCK);
                }
                if (fd_back >= 0) {
                    char msg[BUF_LEN];
                    int msg_len = snprintf(msg, BUF_LEN, "%.*s_%d", decrypted_len, decrypted, id);
                    write(fd_back, msg, msg_len);
                    close(fd_back);
                }

                break;

            }
        }

        free(decrypted);
        free(guess_key);
    }

    close(fd_my);
    close_logger();
    return 0;
}

