#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "mta_rand.h"
#include "utils.h"

int is_all_printable(const char *s, int len) {
    for (int i = 0; i < len; i++) {
        if (!isprint((unsigned char)s[i])) return 0;
    }
    return 1;
}

void generate_printable_password(char *buffer, int len) {
    for (int i = 0; i < len; ++i) {
        char c;
        do {
            c = MTA_get_rand_char();
        } while (!isprint(c));
        buffer[i] = c;
    }
}

long long global_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void check_allocation(void* ptr) {
    if(!ptr) {
        printf("memory allocation failed");
        exit(1);
    }
}
  
