#include <stdio.h>
#include "data.h"

void print_data(struct Data data){
    printf("Data {\n");
    printf("\tid: %d\n", data.id);
    printf("\tuser: %s\n", data.user);
    printf("\tmessage: %s\n", data.message);
    printf("\ttime: %ld\n", data.time);
    printf("}\n");
}
