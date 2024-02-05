#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* return_arr(char** arr, size_t size){
    char* buffer = (char*) malloc(size * 100 * sizeof(char));
    for(size_t i = 0; i < size; ++i){
        if(arr[i] == NULL) continue;
        strcat(buffer, arr[i]);
        strcat(buffer, "\n");
    }

    return buffer;
}

char* list(char** usernames, size_t num_usernames){
    return return_arr(usernames, num_usernames);
}

char* help(char** commands, size_t size){
    return return_arr(commands, size);
}

char* clear(){
    char* buffer = (char*)malloc(sizeof("\033[H")); 
    strcpy(buffer, "\033[H\033[J");

    return buffer;
}
