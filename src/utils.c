#include "utils.h"
#include "config.h"
#include "logging.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>

void print_string_esc_chars(const char *str){
    for(int i = 0; i < strlen(str) + 1; i++){
        switch (str[i]) {
            case '\0':
                printf("\\0 ");
                break;
            case '\n':
                printf("\\n ");
                break;
            default:
                printf("%c ", str[i]);
                break;
        }
    }
    printf("\n");
}

void null_terminate(char* str, int length) {
    if (str == NULL) {
        return;
    }

    if (length <= 0) {
        return;
    }

    if (str[length] == '\0') {
        return;
    }

    str[length] = '\0';
}

int starts_with_escape_sequence(const char *str) {
    if (str == NULL || strlen(str) < 1 || str[0] != '\033') // '\033' represents octal escape sequence for ASCII escape character
        return 0;

    return 1;
}

int is_ansi(const char* str){
    if(starts_with_escape_sequence(str)) return 1;
    if (str == NULL || strlen(str) < 2 || str[0] != '\x1B' || str[1] != '[')
        return 0;

    int len = strlen(str);
    if (str[len - 1] < 'A' || (str[len - 1] > 'Z' && str[len - 1] < 'a') || str[len - 1] > 'z')
        return 0;


    return 1;
}

int starts_with(const char *a, const char *b) {
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

int search_int(int key, int *arr, size_t size){
    for(size_t i = 0; i < size; i++){
        if(key == arr[i]) return i;
    }
    return -1;
}

int search_str(const char* key, char** arr, size_t size){
    for(size_t i = 0; i < size; i++){
        if(arr[i] == NULL) continue;
        if(strcmp(key, arr[i]) == 0) return i;
    }
    return -1;
}

int is_in(const char* str,  const char** list, int size){
    if(str == NULL) return -1;
    for(size_t i = 0; i < size; ++i){
        if(list[i] == NULL) continue;
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
    char* buffer = (char*) calloc(256, sizeof(char));

    if(fptr){
        fgets(buffer, 256, fptr);
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
