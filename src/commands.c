#include "commands.h"
#include "data.h"
#include "errors.h"
#include "logging.h"
#include "config.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


char* return_arr(char** arr, size_t size){
    char* buffer = (char*) malloc(size * 100 * sizeof(char));
    for(size_t i = 0; i < size; ++i){
        if(arr[i] == NULL) continue;
        strcat(buffer, arr[i]);
        if(i != size - 1) strcat(buffer, "\n");
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


void whisper(int clientfd, int sockfd, char* buffer, char** usernames, size_t num_usernames){
    struct Data data = {};

    // Extract the command
    char* token = strtok(buffer, " ");
    if (token == NULL) {
        DEBU("`whisper` returned NULL\n");
        if(clientfd != sockfd)
            send(clientfd, data_to_string(create_data(ERROR_INCORRECT_COMMAND, WARNING, usernames[0])), BUFFER_SIZE, 0);
        else 
            WARN("%s\n", ERROR_INCORRECT_COMMAND);
        return;
    }

    // Set the message status
    data.status = MESSAGE;


    // Set the user based on clientfd
    //
    //data.user == the one who sends the message
    data.user = (char*) malloc(sizeof(usernames[clientfd - sockfd]));
    strncpy(data.user, usernames[clientfd - sockfd], sizeof(usernames[clientfd - sockfd]) - 1);
    data.user[sizeof(data.user) - 1] = '\0';  // Ensure null-termination

    // Extract the target username
    token = strtok(NULL, " ");
    if (token == NULL) {
        if(clientfd != sockfd)
            send(clientfd, data_to_string(create_data(ERROR_USERNAME_NOT_PROVIDED, WARNING, usernames[0])), BUFFER_SIZE, 0);
        else 
            WARN("%s\n", ERROR_USERNAME_NOT_PROVIDED);
        return;
    }

    int username_indx = search_str(token, usernames, num_usernames);
    if (username_indx >= 0) {
        data.id = sockfd + username_indx; // we send the message to him
    } else {
        if(clientfd != sockfd)
            send(clientfd, data_to_string(create_data(ERROR_USERNAME_DOES_NOT_EXIST, WARNING, usernames[0])), BUFFER_SIZE, 0);
        else 
            WARN("%s\n", ERROR_USERNAME_DOES_NOT_EXIST);
        return;
    }

    token = strtok(NULL, "");
    if (token == NULL || is_empty(token)) {
        if(clientfd != sockfd)
            send(clientfd, data_to_string(create_data(ERROR_EMPTY_MESSAGE, WARNING, usernames[0])), BUFFER_SIZE, 0);
        else 
            WARN("%s\n", ERROR_EMPTY_MESSAGE);
        return;
    }

    data.time = get_current_time();

    data.message = (char*) malloc(sizeof(token));
    strncpy(data.message, token, sizeof(data.message) - 1);
    data.message[sizeof(data.message) - 1] = '\0';  // Ensure null-termination

    DEBU("WHISPER: data: %s\n", data_to_string(data));

    DEBU("data->id: %d, fd: %d\n", data->id, fd);
    if(data.id == clientfd) {
        DEBU("fd: %d\n", fd);
        if(clientfd != sockfd) {
            send(clientfd, data_to_string(create_data(ERROR_MESSAGING_SELF, WARNING, usernames[0])), BUFFER_SIZE, 0);
        } else {
            WARN("%s\n", ERROR_MESSAGING_SELF);
        } 

        return;
    }


    if(data.id == sockfd) print_message(&data);
    else send(data.id, data_to_string(create_data(data.message, MESSAGE, data.user)), BUFFER_SIZE, 0);

    return;
}

char* whoami(int fd, int sockfd, char**usernames, size_t num_usernames){
    if(fd < sockfd) return NULL;

    char* buff = (char*) malloc(sizeof(usernames[fd - sockfd]));

    strcpy(buff, usernames[fd - sockfd]);
    return buff;
}

