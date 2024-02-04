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
#include "commands.h"
#include "screen.h"
#include "data.h"
#include "errors.h"
#include "logging.h"
#include "utils.h"
#include "config.h"

int clients[MAX_PENDING_CONNECTIONS];
char* usernames[MAX_PENDING_CONNECTIONS+2];
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

// =========== SERVER UTILS ===========
struct Data create_data(const char* message, int status);
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
                .user = color_username(((_username == NULL) ? "server" : _username), colors[0]),
                .message = buffer,
                .status = MESSAGE,
                .time = get_current_time()
            };

            for (int i = 0; i < num_clients; ++i) {
                send(clients[i], data_to_string(data), BUFFER_SIZE - 1, 0);
            }
        }
    }

    pthread_exit(NULL);
}

void *handle_client(void *arg) {
    int clientfd = *((int *)arg);
    free(arg);

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
        send(clientfd, data_to_string(create_data(ERROR_MAX_CLIENTS_REACHED, ERROR)), BUFFER_SIZE, 0);

        close(clientfd);
        pthread_exit(NULL);
    } else if(is_in(check_data->user, usernames, num_usernames)) {
        WARN("%s\n", ERROR_USERNAME_EXISTS);
        send(clientfd, data_to_string(create_data(ERROR_USERNAME_EXISTS, WARNING)), BUFFER_SIZE, 0);

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
    send(clientfd, data_to_string(create_data(str, INFORMATION)), BUFFER_SIZE, 0);

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
                char* command = "list"; 
                DEBU("Command: %s\n", command);
                run_command(command, clientfd);
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
                usernames[j] = usernames[j + 1];
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

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);

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
        send(clients[i], data_to_string(create_data("Server closed", INFORMATION)), BUFFER_SIZE, 0);
        close(clients[i]);
    }

    free(_username);
    
    close(sockfd);
}

struct Data create_data(const char* message, int status){
    struct Data data;

    data.id = -1;
    data.user = ((_username == NULL) ? "server" : _username);
    data.status = status;
    data.time = get_current_time();
    data.message = (char*) calloc(strlen(message), sizeof(char));
    strcpy(data.message, message);

    return data;
}

void run_command(char* command, int fd){
    if(command == NULL) return;

    char* buffer = (char*) calloc(num_usernames * BUFFER_SIZE, sizeof(char));
    strcpy(buffer, "");

    if(strcmp(command, COMMAND_LIST) == 0){
        for(size_t i = 0; i < num_usernames; ++i){
            if(usernames[i] == NULL) continue;
            strcat(buffer, usernames[i]);
            strcat(buffer, "\n");
        }
        if(fd == _sockfd){
            printf("%s\n", buffer);
        } else {
            send(fd, data_to_string(create_data(buffer, COMMAND)), BUFFER_SIZE, 0);
        }
    } else {
        if(fd == _sockfd) {
            WARN("%s\n", ERROR_COMMAND_NOT_FOUND);
        } else {
            send(fd, data_to_string(create_data(ERROR_COMMAND_NOT_FOUND, WARNING)), BUFFER_SIZE, 0);
        }
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

