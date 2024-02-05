#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include "server.h"
#include "screen.h"
#include "data.h"
#include "errors.h"
#include "logging.h"
#include "utils.h"
#include "config.h"
#include "commands.h"

int clients[MAX_PENDING_CONNECTIONS];
char* usernames[MAX_PENDING_CONNECTIONS+1];
int num_clients = 0;
int num_usernames = 0;

char* colors[MAX_PENDING_CONNECTIONS + 1] = {
    red, 
    green, 
    yellow, 
    blue, 
    purple, 
    cyan, 
};

char* _username = NULL;
int _sockfd = -1;

#define SERVER_NAME ((_username == NULL) ? "server" : _username)

// =========== SERVER UTILS ===========
void close_server(int sockfd);
void run_command(char* command, int clientfd);
char* color_username(const char* username, const char* color);
void *handle_stdin(void *arg);
void *handle_client(void *arg);
// =========== SERVER UTILS ===========


void *handle_stdin(void *arg) {
    while (1) {
        char buffer[BUFFER_SIZE] = {0};
        read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

        if (!is_empty(buffer)) {
            buffer[strcspn(buffer, "\n")] = 0; // remove newline

            if(buffer[0] == '\\'){
                char* command = buffer + 1;
                run_command(command, _sockfd);
                continue;
            }

            struct Data data = {
                .id = -1,  // Use a special ID for messages from the server
                .user = color_username(SERVER_NAME, colors[0]),
                .message = buffer,
                .status = MESSAGE,
                .time = get_current_time()
            };

            for (int i = 0; i < num_clients; ++i) {
                send(clients[i], data_to_string(data), BUFFER_SIZE - 1, 0);
            }
        }
        fflush(stdin);
    }

    pthread_exit(NULL);
}

void *handle_client(void *arg) {
    int clientfd = *((int *)arg);
    free(arg);
    DEBU("clientfd: %d\n", clientfd);

    // Get init data from client
    char check_buf[BUFFER_SIZE];
    if(recv(clientfd, check_buf, BUFFER_SIZE, 0) <= 0){
        WARN("No init data received");
        close(clientfd);
        pthread_exit(NULL);
    }
    struct Data* check_data = string_to_data(check_buf);

    // Add the new client to the list
    if (num_clients >= MAX_PENDING_CONNECTIONS) {
        ERRO("%s\n", ERROR_MAX_CLIENTS_REACHED);
        send(clientfd, data_to_string(create_data(ERROR_MAX_CLIENTS_REACHED, ERROR, SERVER_NAME)), BUFFER_SIZE, 0);

        close(clientfd);
        pthread_exit(NULL);
    } else if(is_in(check_data->user, usernames, num_usernames)) {
        WARN("%s\n", ERROR_USERNAME_EXISTS);
        send(clientfd, data_to_string(create_data(ERROR_USERNAME_EXISTS, WARNING, SERVER_NAME)), BUFFER_SIZE, 0);

        close(clientfd);
        pthread_exit(NULL);
    } else {
        clients[num_clients] = clientfd;
        usernames[num_usernames] = check_data->user;
        num_clients++;
        num_usernames++;
    }

    INFO("Client '%s' connected\n", check_data->user);

    char str[] = "Connected as: ";
    strcat(str, check_data->user);
    send(clientfd, data_to_string(create_data(str, INFORMATION, SERVER_NAME)), BUFFER_SIZE, 0);

    while (1) {
        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_received = recv(clientfd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            INFO("Client disconnected\n");
            break;
        }

        buffer[bytes_received] = '\0'; // Ensure null-termination

        struct Data *data = string_to_data(buffer);

        if (data) {
            // Check if message is command
            if(data->message[0] == '\\') {
                char* command = data->message + 1; 
                DEBU("Command: %s\n", command);
                run_command(command, clientfd);
                continue;
            } else {
                
                strcpy(data->user, color_username(data->user, colors[search_int(clientfd, clients, num_clients) + 1]));
                print_message(data);

                // Broadcast the message to all clients
                for (int i = 0; i < num_clients; ++i) {
                    if (clients[i] != clientfd) {
                        send(clients[i], data_to_string(*data), bytes_received, 0);
                    }
                }
            }

            free(data->user);
            free(data->message);
            free(data);
        }
    }

    // Remove the disconnected client from the list
    for (int i = 0; i < num_clients; ++i) {
        if (clients[i] == clientfd) {
            for (int j = i; j < num_clients - 1; ++j) {
                clients[j] = clients[j + 1];
                usernames[j + 1] = usernames[j + 2]; // + 1 Because of server's username
            }
            num_clients--;
            num_usernames--;
            break;
        }
    }

    close(clientfd);
    pthread_exit(NULL);
}

void siginthandler(int params){
    close_server(_sockfd); 
    INFO("Server closed\n");
    
    exit(0);
}

void serve(const char *ip_address, int port, char* username) {
	signal(SIGINT, siginthandler);

    if(username != NULL && !is_empty(username)){
        _username = (char*) calloc(strlen(username), sizeof(char));
        strcpy(_username, username);
    }

    // Add server's username in the list of usernames
    if(username != NULL) usernames[num_usernames++] = username;
    else usernames[num_usernames++] = "server";
    DEBU("server username: %s\n", _username);

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DEBU("sockfd: %d\n", _sockfd);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {inet_addr(ip_address)}
    };

    if (bind(_sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        handle_error("Bind Failed");
    }

    INFO("Press Ctrl+C to close server\n");
    INFO("Waiting for clients to connect\n");
    if (listen(_sockfd, MAX_PENDING_CONNECTIONS) < 0) {
        handle_error("Listen failed");
    }

    pthread_t stdin_thread;
    if (pthread_create(&stdin_thread, NULL, handle_stdin, NULL) != 0) {
        ERRO("Error creating thread for stdin\n");
    } else {
        pthread_detach(stdin_thread);
    }

    while (1) {
        int clientfd = accept(_sockfd, 0, 0);

        if (clientfd < 0) {
            ERRO("Error accepting connection\n");
            continue;
        }


        pthread_t thread;
        int *client_arg = malloc(sizeof(int));
        *client_arg = clientfd;

        if (pthread_create(&thread, NULL, handle_client, client_arg) != 0) {
            ERRO("Error creating thread\n");
            close(clientfd);
            free(client_arg); // Free the allocated memory
        } else {
            pthread_detach(thread);
        }
    }
    
    close_server(_sockfd);
}

void close_server(int sockfd){
    for (int i = 0; i < num_clients; ++i) {
        send(clients[i], data_to_string(create_data("Server closed", INFORMATION, SERVER_NAME)), BUFFER_SIZE, 0);
        close(clients[i]);
    }

    free(_username);
    
    close(sockfd);
}


#define CHECK_BUFFER(buff) \
    do {if(buff == NULL) {free(buffer); return;}} while(0);

void run_command(char* command, int fd){
    if(command == NULL) return;

    char* buffer = NULL;

    if(strcmp(command, COMMAND_LIST) == 0){
        buffer = list(usernames, num_usernames);
    } else if(strcmp(command, COMMAND_HELP) == 0 || strcmp(command, COMMAND_HELP_SHORT) == 0){
        buffer = help(command_help, ARR_LEN(command_help));
    } else if(strcmp(command, COMMAND_CLEAR) == 0) {
        buffer = clear();
    } else if(starts_with(command, COMMAND_WHISPER)) {
        whisper(fd, _sockfd, command, usernames, num_usernames);
        return;
    } else {
        if(fd == _sockfd) {
            WARN("%s\n", ERROR_COMMAND_NOT_FOUND);
        } else {
            send(fd, data_to_string(create_data(ERROR_COMMAND_NOT_FOUND, WARNING, SERVER_NAME)), BUFFER_SIZE, 0);
        }
        free(buffer);
        return;
    } 

    CHECK_BUFFER(buffer);
    if(fd == _sockfd){
        printf("%s\n", buffer);
    } else {
        send(fd, data_to_string(create_data(buffer, COMMAND, SERVER_NAME)), BUFFER_SIZE, 0);
    }

    free(buffer);
}


char* color_username(const char* username, const char* color) {
    size_t colored_username_size = strlen(color) + strlen(username) + strlen(reset) + 1;

    char* colored_username = (char*)calloc(colored_username_size, sizeof(char));

    if (colored_username != NULL) {
        strcpy(colored_username, color);
        strcat(colored_username, username);
        strcat(colored_username, reset);
    } else {
        return NULL;
    }

    return colored_username;
}

