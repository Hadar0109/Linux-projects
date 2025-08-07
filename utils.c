#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdarg.h> 
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include "mta_rand.h"
#include "utils.h"

static FILE *log_fp = NULL;

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

int get_next_client_id(void) {
    const char *idpath = "/mnt/mta/id_file";

    int fd = open(idpath, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open id_file");
        exit(EXIT_FAILURE);
    }

    if (flock(fd, LOCK_EX) < 0) {
        perror("flock");
        close(fd);
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        flock(fd, LOCK_UN);
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (st.st_size == 0) {
        if (dprintf(fd, "0\n") < 0) {
            perror("init id_file");
            flock(fd, LOCK_UN);
            close(fd);
            exit(EXIT_FAILURE);
        }
        fsync(fd);
    }

    lseek(fd, 0, SEEK_SET);
    char buf[32] = {0};
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n < 0) {
        perror("read id_file");
        flock(fd, LOCK_UN);
        close(fd);
        exit(EXIT_FAILURE);
    }
    int id = atoi(buf);

    int next = id + 1;
    lseek(fd, 0, SEEK_SET);
    if (dprintf(fd, "%d\n", next) < 0) {
        perror("update id_file");
        flock(fd, LOCK_UN);
        close(fd);
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, strlen(buf)); 

    flock(fd, LOCK_UN);
    close(fd);

    return next;
}

void init_logger(const char *path) {
    log_fp = fopen(path, "a");
    if (!log_fp) {
        perror("fopen log file");
        exit(EXIT_FAILURE);
    }
}

void close_logger(void) {
    if (log_fp) {
        fclose(log_fp);
        log_fp = NULL;
    }
}

void client_log_to_file(const char *level, int id, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    long long ts = global_timestamp();
    fprintf(log_fp, "\n%lld  [CLIENT #%d] [%s] ", ts, id, level);
    vfprintf(log_fp, fmt, ap);
    //fprintf(log_fp, "\n");
    fflush(log_fp);

    va_end(ap);
}

void server_log_to_file(const char *level, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    long long ts = global_timestamp();
    fprintf(log_fp, "%lld  [SERVER] [%s] ", ts, level);
    vfprintf(log_fp, fmt, ap);
    fprintf(log_fp, "\n");
    fflush(log_fp);

    va_end(ap);
}

void log_to_file_simple(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    vfprintf(log_fp, fmt, ap);
    fprintf(log_fp, "\n");
    fflush(log_fp);

    va_end(ap);
}
