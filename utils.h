#ifndef UTILS_H
#define UTILS_H

int is_all_printable(const char *s, int len);
void generate_printable_password(char *buffer, int len);
long long global_timestamp();
int get_next_client_id(void);
void init_logger(const char *path);
void close_logger(void);
void client_log_to_file(const char *level, int id, const char *fmt, ...);
void server_log_to_file(const char *level, const char *fmt, ...);
void log_to_file_simple(const char *fmt, ...);

#endif

