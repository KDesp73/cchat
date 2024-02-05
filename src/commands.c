#include "commands.h"
#include "data.h"
#include "errors.h"
#include "logging.h"
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


char* whisper(int clientfd, int sockfd, char* buffer, char** usernames, size_t num_usernames){
    DEBU("WHISPER: command: %s\n", buffer); 

    struct Data data = {};

    // Extract the command
    char* token = strtok(buffer, " ");
    if (token == NULL) {
        DEBU("Returned NULL at command\n");
        return NULL;
    }

    DEBU("WHISPER: command_name: %s\n", token);

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
        DEBU("Returned NULL at username\n");
        return ERROR_USERNAME_NOT_PROVIDED;
    }

    DEBU("WHISPER: username: %s\n", token);

    int username_indx = search_str(token, usernames, num_usernames);
    if (username_indx >= 0) {
        data.id = sockfd + username_indx; // we send the message to him
    } else {
        DEBU("Returned NULL at username search\n");
        return ERROR_USERNAME_DOES_NOT_EXIST;
    }

    // Extract the message
    token = strtok(NULL, "");
    if (token == NULL || is_empty(token)) {
        DEBU("Returned NULL at message\n");
        return ERROR_EMPTY_MESSAGE;
    }

    DEBU("WHISPER: message: %s\n", token);
    data.time = get_current_time();

    // Copy the message with bounds checking
    data.message = (char*) malloc(sizeof(token));
    strncpy(data.message, token, sizeof(data.message) - 1);
    data.message[sizeof(data.message) - 1] = '\0';  // Ensure null-termination

    // Check if data_to_string returns NULL
    char* result = data_to_string(data);

    DEBU("WHISPER: data_to_string: %s\n", data_to_string(data));


    return result;
}

