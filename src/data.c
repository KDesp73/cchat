#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "screen.h"
#include "utils.h"

void print_data(struct Data data){
    printf("Data {\n");
    printf("\tid: %d\n", data.id);
    printf("\tuser: %s\n", data.user);
    printf("\tmessage: %s\n", data.message);
    printf("\tis_error: %zu\n", data.status);
    printf("\ttime: %ld\n", data.time);
    printf("}\n");
}

void print_message(struct Data* data){
    char time_str[32];
    strftime(time_str, 32, "%d.%m.%Y %H:%M:%S", localtime(&data->time));  
    printf("%s:%s %s %s(%s)%s\n", data->user, reset, data->message, black, time_str, reset);
}

char* data_to_string(struct Data data) {
    char* formatting = "%d,%s,%s,%zu,%ld";

    // Determine required size
    size_t len = snprintf(NULL, 0, formatting, data.id, data.user, data.message, data.status, data.time);

    // Allocate memory
    char *datastr = malloc(sizeof *datastr * (len + 1)); // +1 for null terminator
    if (!datastr) {
        fprintf(stderr, "%s() error: virtual memory allocation failed.\n", __func__);
        return NULL; // Return NULL on allocation failure
    }

    // Use snprintf with allocated buffer
    if (snprintf(datastr, len + 1, formatting, data.id, data.user, data.message, data.status, data.time) < 0) {
        fprintf(stderr, "%s() error: snprintf returned an error.\n", __func__);
        free(datastr); // Free the allocated memory
        return NULL;   // Return NULL on snprintf error
    }

    return datastr;
}


struct Data *string_to_data(char *str) {
    if(is_empty(str)) {
        return NULL;
    }

    struct Data *data = (struct Data *)malloc(sizeof(struct Data));

    if (!data) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    // Tokenize the input string
    // Parse and set the id
    char *token = strtok(str, ",");
    if (!token) {
        fprintf(stderr, "Invalid input string (id).\n");
        free(data);
        return NULL;
    }
    data->id = atoi(token);

    // Parse and set the user
    token = strtok(NULL, ",");
    if (!token) {
        fprintf(stderr, "Invalid input string (user).\n");
        free(data);
        return NULL;
    }
    data->user = strdup(token);

    // Parse and set the message
    token = strtok(NULL, ",");
    if (!token) {
        fprintf(stderr, "Invalid input string (message).\n");
        free(data->user);
        free(data);
        return NULL;
    }
    data->message = strdup(token);

    // Parse and set the error flag
    token = strtok(NULL, ",");
    if (!token) {
        fprintf(stderr, "Invalid input string (is_error).\n");
        free(data->user);
        free(data->message);
        free(data);
        return NULL;
    }
    data->status = atoi(token);

    // Parse and set the time
    token = strtok(NULL, ",");
    if (!token) {
        fprintf(stderr, "Invalid input string (time).\n");
        free(data->user);
        free(data->message);
        free(data);
        return NULL;
    }
    data->time = atoi(token);

    return data;
}

