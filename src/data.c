#include <stdio.h>
#include "data.h"
#include "logging.h"


void print_data(struct Data data){
    printf("Data {\n");
    printf("\tid: %d\n", data.id);
    printf("\tuser: %s\n", data.user);
    printf("\tmessage: %s\n", data.message);
    printf("\ttime: %ld\n", data.time);
    printf("}\n");
}



char* data_to_string(struct Data data) {
    // Determine required size
    size_t len = snprintf(NULL, 0, "%d,%s,%s,%ld", data.id, data.user, data.message, data.time);

    // Allocate memory
    char *datastr = malloc(sizeof *datastr * (len + 1)); // +1 for null terminator
    if (!datastr) {
        fprintf(stderr, "%s() error: virtual memory allocation failed.\n", __func__);
        return NULL; // Return NULL on allocation failure
    }

    // Use snprintf with allocated buffer
    if (snprintf(datastr, len + 1, "%d,%s,%s,%ld", data.id, data.user, data.message, data.time) < 0) {
        fprintf(stderr, "%s() error: snprintf returned an error.\n", __func__);
        free(datastr); // Free the allocated memory
        return NULL;   // Return NULL on snprintf error
    }

    return datastr;
}
