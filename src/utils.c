#include "utils.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>

int is_in(char* str, char** list, int size){
    for(size_t i = 0; i < size; ++i){
        if(strcmp(str, list[i]) == 0) return 1;
    }
    return 0;
}

int is_empty(const char *s) {
    while (*s != '\0') {
        if (!isspace((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

char* pop_arg(int* argc, char** argv){
    char* arg = argv[0];

    for(int i = 0; i < *argc; i++){
        if(i == *argc - 1) argv[i] = NULL;
        else argv[i] = argv[i+1];
    }
    *argc = *argc-1;

    return arg;
}

void print_args(int argc, char** argv){
    for(int i = 0; i < argc; i++){
        printf("%s ", argv[i]);
    } 
    printf("\n");
}

int is_valid_ip_address(char *ipAddress){
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

time_t get_current_time(){
    return time(0);
}

char* expand_time(time_t time){
    char* str = ctime(&time);
    str[strlen(str)-1] = '\0';

    return str;
}

char* read_file_line(const char* path){
    FILE* fptr = fopen(path, "r");
    char* buffer = (char*) calloc(100, sizeof(char));

    if(fptr){
        fgets(buffer, 100, fptr);
        fclose(fptr);
    } else return NULL;

    if(is_empty(buffer)) return NULL;

    buffer[strcspn(buffer, "\n")] = 0; // remove newline

    return buffer;
}

char *random_string(size_t length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";        
    char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * (length +1));

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}
