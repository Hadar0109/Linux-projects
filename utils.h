#ifndef UTILS_H
#define UTILS_H

int is_all_printable(const char *s, int len);
void generate_printable_password(char *buffer, int len);
long long global_timestamp();
void check_allocation(void* ptr);

#endif

