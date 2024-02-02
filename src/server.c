#include <pthread.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include "server.h"
#include "data.h"
#include "logging.h"
#include "utils.h"

#define MAX_PENDING_CONNECTIONS 10
#define TIMEOUT_MS 50000
#define BUFFER_SIZE 256

int clients[MAX_PENDING_CONNECTIONS];
int num_clients = 0;

void *handle_stdin(void *arg) {
    while (1) {
        char buffer[BUFFER_SIZE] = {0};
        read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

        if (!is_empty(buffer)) {
            buffer[strcspn(buffer, "\n")] = 0; // remove newline

            struct Data data = {
                .id = -1,  // Use a special ID for messages from the server
                .user = "server",
                .message = buffer,
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

    // Add the new client to the list
    if (num_clients < MAX_PENDING_CONNECTIONS) {
        clients[num_clients++] = clientfd;
    } else {
        ERRO("Max number of clients reached\n");
        close(clientfd);
        pthread_exit(NULL);
    }

    struct pollfd fds[2] = {
        {
            .fd = STDIN_FILENO,
            .events = POLLIN,
            .revents = 0
        },
        {
            .fd = clientfd,
            .events = POLLIN,
            .revents = 0
        }
    };

    while (1) {
        int poll_result = poll(fds, 2, TIMEOUT_MS);

        if (poll_result < 0) {
            ERRO("Error in poll\n");
            break;
        }

        if (poll_result > 0) {
            if (fds[0].revents & POLLIN) {
                char buffer[BUFFER_SIZE] = {0};
                ssize_t bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

                if (bytes_read <= 0) {
                    // Handle stdin closure or error
                    break;
                }

                buffer[bytes_read] = '\0'; // Ensure null-termination

                if(is_empty(buffer)) continue;
                buffer[strcspn(buffer, "\n")] = 0; // remove newline

                struct Data data = {
                    .id = -1,
                    .user = "server",
                    .message = buffer,
                    .time = get_current_time()
                };

                for (int i = 0; i < num_clients; ++i) {
                    send(clients[i], data_to_string(data), BUFFER_SIZE - 1, 0);
                }
            }

            if (fds[1].revents & POLLIN) {
                char buffer[BUFFER_SIZE] = {0};
                ssize_t bytes_received = recv(clientfd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_received <= 0) {
                    INFO("Client disconnected\n");
                    break;
                }

                struct Data *data = string_to_data(buffer);

                if (data) {
                    print_message(data);

                    free(data->user);
                    free(data->message);
                    free(data);
                }
            }
        }
    }

    // Remove the disconnected client from the list
    for (int i = 0; i < num_clients; ++i) {
        if (clients[i] == clientfd) {
            for (int j = i; j < num_clients - 1; ++j) {
                clients[j] = clients[j + 1];
            }
            num_clients--;
            break;
        }
    }

    close(clientfd);
    pthread_exit(NULL);
}

void serve(const char *ip_address, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {inet_addr(ip_address)}
    };

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        handle_error("Bind Failed");
    }

    INFO("Waiting for clients to connect\n");
    if (listen(sockfd, MAX_PENDING_CONNECTIONS) < 0) {
        handle_error("Listen failed");
    }

    pthread_t stdin_thread;
    if (pthread_create(&stdin_thread, NULL, handle_stdin, NULL) != 0) {
        ERRO("Error creating thread for stdin\n");
    } else {
        pthread_detach(stdin_thread);
    }

    while (1) {
        int clientfd = accept(sockfd, 0, 0);

        if (clientfd < 0) {
            ERRO("Error accepting connection\n");
            continue;
        }

        INFO("Client connected\n");

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

    close(sockfd);
}
