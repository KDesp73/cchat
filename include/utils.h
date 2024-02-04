#ifndef UTILS_H
#define UTILS_H
#include <time.h>

int is_empty(const char *s);
char* pop_arg(int* argc, char** argv);
void print_args(int argc, char** argv);
int is_valid_ip_address(char *ipAddress);
time_t get_current_time();
char* expand_time(time_t time);
char* read_file_line(const char* path);
char* random_string(size_t length);
int is_in(char* str, char** list, int size);
int search_int(int key, int* arr, size_t size);

#endif // UTILS_H
