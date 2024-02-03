#ifndef DATA_H
#define DATA_H

#include <time.h>

#define MESSAGE 0
#define ERROR 1
#define INFORMATION 2
#define WARNING 3

struct Data {
    int id;
    char* user;
    char* message;
    size_t status;
    time_t time;
};

void print_data(struct Data data);
void print_message(struct Data* data);
char* data_to_string(struct Data data);
struct Data* string_to_data(char* str);

#endif // DATA_H
