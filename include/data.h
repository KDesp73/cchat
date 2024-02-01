#ifndef DATA_H
#define DATA_H

struct Data {
    int id;
    char* user;
    char* message;
    long time;
};

void print_data(struct Data data);

#endif // DATA_H
