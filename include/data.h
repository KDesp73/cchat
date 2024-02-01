#ifndef DATA_H
#define DATA_H

#include <time.h>

struct Data {
    int id;
    char* user;
    char* message;
    time_t time;
};

void print_data(struct Data data);
char* data_to_string(struct Data data);
struct Data* string_to_data(char* str);

#endif // DATA_H
