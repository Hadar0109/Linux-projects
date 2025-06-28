#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "mta_crypt.h"
#include "utils.h"
#include "threads.h"

char *encrypted_data = NULL;
char *correct_password = NULL;
char *cracked_result = NULL;

int password_len = -1;
int key_len = -1;
int password_ready = 0;
int password_cracked = 0;
int timeout_seconds = -1;

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_new_password = PTHREAD_COND_INITIALIZER;
pthread_cond_t g_password_cracked = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]) {
    int num_consumers = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            num_consumers = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
            password_len = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            timeout_seconds = atoi(argv[++i]);
        }
        else {
            fprintf(stderr, "Unknown or incomplete flag: %s\n", argv[i]);
            printf("Usage: %s -n <num_consumers> -l <password_length> [-t <timeout_seconds>]\n", argv[0]);
            return 1;
        }
    }


    if (num_consumers <= 0 || password_len <= 0 || password_len % 8 != 0) {
        fprintf(stderr, "Error: invalid arguments- ");
        if (num_consumers <= 0 ) {
            fprintf(stderr, "missing num of decrypters. ");
        }
        if (password_len <= 0 ) {
            fprintf(stderr, "missing password length. ");
        }
        else if (password_len % 8 != 0) {
            fprintf(stderr, "password length must be a positive multiple of 8. ");
        }
        printf("\nUsage: %s -n <num_consumers> -l <password_length> [-t <timeout_seconds>]\n", argv[0]);
        return 1;
    }

    key_len = password_len / 8;

    if (MTA_crypt_init() != MTA_CRYPT_RET_OK) {
        fprintf(stderr, "Failed to initialize crypto library\n");
        return 1;
    }

    encrypted_data = malloc(password_len);
    check_allocation(encrypted_data);
    correct_password = malloc(password_len);
    check_allocation(correct_password);
    cracked_result = malloc(password_len);
    check_allocation(cracked_result);

    if (!encrypted_data || !correct_password || !cracked_result) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    pthread_t ptid;
    pthread_t ctid[num_consumers];

    pthread_create(&ptid, NULL, producer, NULL);
    for (int i = 0; i < num_consumers; i++)
        pthread_create(&ctid[i], NULL, consumer, NULL);

    pthread_join(ptid, NULL);
    for (int i = 0; i < num_consumers; i++)
        pthread_join(ctid[i], NULL);

    free(encrypted_data);
    free(correct_password);
    free(cracked_result);

    return 0;
}

