#ifndef UTILS_H
#define UTILS_H
#include "clib.h"
#include <time.h>
#include <stddef.h>

// Should be added to the clib library
#define UNREACHABLE \
    do { \
        PANIC("This part of the code should be unreachable!"); \
    } while(0);

#define STRCMP(s1, s2) (strcmp(s1, s2) == 0)
CstrArray make_cstr_array(size_t capacity, Cstr first, ...);
void cstr_array_append(CstrArray* array, Cstr item);
void cstr_array_remove(CstrArray* array, Cstr item);
void cstr_array_remove_index(CstrArray* array, size_t index);


int starts_with(const char *a, const char *b);
int is_empty(const char *s);
char* pop_arg(int* argc, char** argv);
void print_args(int argc, char** argv);
int is_valid_ip_address(char *ipAddress);
time_t get_current_time();
char* expand_time(time_t time);
char* read_file_line(const char* path);
char* random_string(size_t length);
int is_in(const char* str, char** list, int size);
int search_int(int key, int* arr, size_t size);
int search_str(const char* key, char** arr, size_t size);
int is_ansi(const char* str);
void null_terminate(char* str, int length);
void print_string_esc_chars(const char* str);


#define NULL_TERMINATE(str) null_terminate(str, strlen(str))



#endif // UTILS_H
