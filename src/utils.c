#include "utils.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>

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

int isValidIpAddress(char *ipAddress){
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}
