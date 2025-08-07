// encrypter/encrypter.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>
#include "mta_rand.h"
#include "mta_crypt.h"
#include "utils.h"

#define PIPE_DIR        "/mnt/mta"
#define CONFIG_FILE     PIPE_DIR "/mtacrypt.conf"
#define ENCRYPTER_PIPE  PIPE_DIR "/server_pipe"
#define LOG_FILE_PATH   "/var/log/mtacrypt.log"
#define MAX_CLIENTS     16
#define MAX_PATH_LEN    256
#define BUF_LEN         512

int main() {
    init_logger(LOG_FILE_PATH);
    
    if (MTA_crypt_init() != MTA_CRYPT_RET_OK) {
        server_log_to_file("ERROR", "MTA_crypt_init failed");
        exit(EXIT_FAILURE);
    }

    FILE *cfg = fopen(CONFIG_FILE, "r");
    if (!cfg) {
        server_log_to_file("ERROR", "fopen %s failed: %s", CONFIG_FILE, strerror(errno));
        exit(EXIT_FAILURE);
    }
    log_to_file_simple("Reading %s...", CONFIG_FILE);

    int password_len = 0;
    char line[BUF_LEN];
    while (fgets(line, sizeof(line), cfg)) {
        if (sscanf(line, "PASSWORD_LENGTH=%d", &password_len) == 1)
            break;
    }
    fclose(cfg);
    
    if (password_len <= 0) {
        server_log_to_file("ERROR", "Invalid PASSWORD_LENGTH in %s", CONFIG_FILE);
        exit(EXIT_FAILURE);
    }
    log_to_file_simple("Password length set to %d", password_len);

    char *original  = malloc(password_len + 1);
    char *encrypted = malloc(password_len * 4 + 1);
    if (!original || !encrypted) {
        server_log_to_file("ERROR", "malloc failed");
        exit(EXIT_FAILURE);
    }

    char client_pipes[MAX_CLIENTS][MAX_PATH_LEN];
    int client_count = 0;
    int got_decrypted = 0;
    int key_len = password_len / 8;

    unsigned char *key = malloc(key_len + 1);
    if (!key) {
        server_log_to_file("ERROR", "malloc key failed");
        exit(EXIT_FAILURE);
    }

    generate_printable_password(original, password_len);
    MTA_get_rand_data(key, key_len);

    server_log_to_file("INFO", "New password generated: %.*s, key: %.*s ,", password_len, original, key_len, (char*)key);

    unsigned int encrypted_len;
    MTA_encrypt(key, key_len, original, password_len, encrypted, &encrypted_len);

    log_to_file_simple("After encryption: %.*s", encrypted_len, encrypted);
    free(key);
    
    if (mkfifo(ENCRYPTER_PIPE, 0666) < 0 && errno != EEXIST) {
        server_log_to_file("ERROR", "mkfifo(%s): %s", ENCRYPTER_PIPE, strerror(errno));
        exit(EXIT_FAILURE);
    }
    log_to_file_simple("Listening on %s", ENCRYPTER_PIPE);

    int ep_fd = open(ENCRYPTER_PIPE, O_RDWR);
    if (ep_fd < 0) {
        server_log_to_file("ERROR", "open(%s): %s", ENCRYPTER_PIPE, strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (;;) {
        if (got_decrypted) {
            unsigned char *key = malloc(key_len+1);
            if (!key) {
                server_log_to_file("ERROR", "malloc key failed");
                break;
            }

            generate_printable_password(original, password_len);
            MTA_get_rand_data(key, key_len);
            
            unsigned int encrypted_len;
            MTA_encrypt(key, key_len, original, password_len, encrypted, &encrypted_len);

            server_log_to_file("INFO", "New password: %.*s, key: %.*s, Encrypted: %.*s", password_len, original, key_len, (char*)key, encrypted_len, encrypted);

            free(key);
            got_decrypted = 0;
        }

        for (int i = 0; i < client_count; i++) {
            int cfd = open(client_pipes[i], O_WRONLY);
            if (cfd < 0) {
                server_log_to_file("ERROR", "open(%s) for broadcast: %s", client_pipes[i], strerror(errno));
                continue;
            }
            ssize_t w = write(cfd, encrypted, encrypted_len);
            if (w <= 0) {
                server_log_to_file("ERROR", "write(%s) failed: %s", client_pipes[i], strerror(errno));
            } 
            close(cfd);
        }

        while (1) {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(ep_fd, &rfds);
            if (select(ep_fd + 1, &rfds, NULL, NULL, NULL) < 0) {
                if (errno == EINTR) 
                    continue;
                server_log_to_file("ERROR", "select: %s", strerror(errno));
                continue;
            }
            if (!FD_ISSET(ep_fd, &rfds)) 
                continue;

            char buf[BUF_LEN];
            ssize_t n = read(ep_fd, buf, sizeof(buf) - 1);
            if (n < 0) {
                server_log_to_file("ERROR", "Error reading from encrypter pipe (fd=%d): %s", ep_fd, strerror(errno));
                break;
            }
            buf[n] = '\0';

            if (strncmp(buf, PIPE_DIR, strlen(PIPE_DIR)) == 0) {
                buf[strcspn(buf, "\r\n")] = '\0';
                int id = atoi(strrchr(buf, '_') + 1);
                server_log_to_file("INFO", "Received connection request from decrypter id %d, fifo name %s", id, buf);
                
                if (client_count < MAX_CLIENTS) {
                    strncpy(client_pipes[client_count++], buf, MAX_PATH_LEN);
                    int cfd = open(buf, O_WRONLY);
                    
                    if (cfd >= 0) {
                        ssize_t w = write(cfd, encrypted, encrypted_len);
                        if (w < 0){
                            server_log_to_file("ERROR", "Failed to write to new decrypter %d: %s", id, strerror(errno));
                        }
                        close(cfd);
                    } else {
                        server_log_to_file("ERROR", "Could not open fifo %s for new decrypter: %s", buf, strerror(errno));
                    }
                }
                continue;
            }

            if (memcmp(buf, original, password_len) == 0) {
                int id = atoi(strrchr(buf, '_') + 1);
                server_log_to_file("OK", "Password decrypted successfully by decrypter #%d",id);
                got_decrypted = 1;
                break;
            }
        }
    }

    close(ep_fd);
    free(original);
    free(encrypted);
    close_logger();
    return 0;
}

