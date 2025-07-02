#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "mta_rand.h"
#include "mta_crypt.h"
#include "utils.h"
#include "threads.h"

extern char *encrypted_data;
extern char *correct_password;
extern char *cracked_result;

extern int password_len, key_len;
extern int password_ready, password_cracked, timeout_seconds;

extern pthread_mutex_t g_lock;
extern pthread_cond_t g_new_password;
extern pthread_cond_t g_password_cracked;

static int client_counter = 0;

void* producer() {
    char *original_password = malloc(password_len);
    check_allocation(original_password);
    char *key = malloc(key_len);
    check_allocation(key);
    unsigned int encrypted_len;

    for(;;) {
        generate_printable_password(original_password, password_len);
        MTA_get_rand_data(key, key_len);
        MTA_encrypt(key, key_len, original_password, password_len, encrypted_data, &encrypted_len);

        printf("%lld  [SERVER]     [INFO]  New password generated: ", global_timestamp());
        fwrite(original_password, 1, password_len, stdout);
        printf(", key: ");
        fwrite(key, 1, key_len, stdout);
        printf(", After encryption: ");
        fwrite(encrypted_data, 1, encrypted_len, stdout);
        printf("\n");

        pthread_mutex_lock(&g_lock);
        memcpy(correct_password, original_password, password_len);
        password_ready = 1;
        password_cracked = 0;
        pthread_cond_broadcast(&g_new_password);
        pthread_mutex_unlock(&g_lock);

        pthread_mutex_lock(&g_lock);
        if (timeout_seconds > 0) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout_seconds;

            int wait_result = 0;
            while (!password_cracked && wait_result != ETIMEDOUT) {
                wait_result = pthread_cond_timedwait(&g_password_cracked, &g_lock, &ts);
            } 

            if (!password_cracked) {
                printf("%lld  [SERVER]     [ERROR] No password received during the configured timeout period (%d seconds), regenerating password\n", global_timestamp(), timeout_seconds);
            }
        } else {
            while (!password_cracked)
                pthread_cond_wait(&g_password_cracked, &g_lock);
        }
        pthread_mutex_unlock(&g_lock);
    }

    free(original_password);
    free(key);
    return NULL;
}

void* consumer() {
    int client_id;

    pthread_mutex_lock(&g_lock);
    client_id = client_counter++;
    pthread_mutex_unlock(&g_lock);

    char *local_encrypted = malloc(password_len);
    check_allocation(local_encrypted);
    char *guess_key = malloc(key_len);
    check_allocation(guess_key);
    char *decrypted = malloc(password_len);
    check_allocation(decrypted);
    unsigned int decrypted_len;
    int iterations;

    for(;;) {
        pthread_mutex_lock(&g_lock);
        while (!password_ready)
            pthread_cond_wait(&g_new_password, &g_lock);
        memcpy(local_encrypted, encrypted_data, password_len);
        pthread_mutex_unlock(&g_lock);

        iterations = 0;

        for(;;) {
            pthread_mutex_lock(&g_lock);
            if (password_cracked) {
                pthread_mutex_unlock(&g_lock);
                break;
            }
            pthread_mutex_unlock(&g_lock);

            iterations++;
            MTA_get_rand_data(guess_key, key_len);
            if (MTA_decrypt(guess_key, key_len, local_encrypted, password_len, decrypted, &decrypted_len) == MTA_CRYPT_RET_OK) {
                if (is_all_printable(decrypted, password_len)) {
                    printf("%lld  [CLIENT #%d]  [INFO]  After decryption(", global_timestamp(), client_id);
                    fwrite(decrypted, 1, password_len, stdout);
                    printf("), key guessed(");
                    fwrite(guess_key, 1, key_len, stdout);
                    printf("), sending to server after %d iterations\n", iterations);

                    pthread_mutex_lock(&g_lock);
                    if (!password_cracked) {
                        if (memcmp(decrypted, correct_password, password_len) == 0) {
                            memcpy(cracked_result, decrypted, password_len);
                            password_cracked = 1;
                            printf("%lld  [SERVER]     [OK]    Password decrypted successfully by client #%d, received(", global_timestamp(), client_id);
                            fwrite(decrypted, 1, password_len, stdout);
                            printf("), is (");
                            fwrite(correct_password, 1, password_len, stdout);
                            printf(")\n");
                            pthread_cond_signal(&g_password_cracked);
                        } else {
                            printf("%lld  [SERVER]     [ERROR] Wrong password received from client %d, should be (", global_timestamp(), client_id);
                            fwrite(correct_password, 1, password_len, stdout);
                            printf(")\n");
                        }
                    }
                    pthread_mutex_unlock(&g_lock);
                }
            }
        }
    }

    free(local_encrypted);
    free(guess_key);
    free(decrypted);
    return NULL;
}

