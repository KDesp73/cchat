#include "server.h"
#include "data.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include "config.h"
#include "screen.h"
#include "logging.h"
#include "utils.h"

void serve(char* ip_address, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {inet_addr(ip_address)}
    };

    if(bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        handle_error("Bind Failed");
    }

    INFO("Waiting for a client to connect\n");
    if(listen(sockfd, MAX_PENDING_CONNECTIONS) < 0) {
        handle_error("Listen failed");
    }

    while (1) {
        int clientfd = accept(sockfd, 0, 0);

        if (clientfd < 0) {
            ERRO("Error accepting connection\n");
            continue;
        }

        INFO("Client connected\n");

        struct pollfd fds[2] = {
            {
                0, // stdin
                POLLIN,
                0
            },
            {
                clientfd,
                POLLIN,
                0
            }
        };

        while (1) {
            char buffer[BUFFER_SIZE] = { 0 };

            poll(fds, 2, TIMEOUT_MS);

            if (fds[0].revents & POLLIN) {
                read(0, buffer, BUFFER_SIZE - 1);

                if(is_empty(buffer)) continue;

                struct Data data = {
                    .id = sockfd,
                    .user = "server",
                    .message = buffer,
                    .time = get_current_time()
                };

                send(clientfd, data_to_string(data), BUFFER_SIZE - 1, 0);
            } else if (fds[1].revents & POLLIN) {
                int bytes_received = recv(clientfd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_received <= 0) {
                    INFO("Client disconnected\n");
                    close(clientfd);
                    break;
                }

                struct Data* data = string_to_data(buffer);

                if(data){
                    printf("%s%s:%s %s\n", red, data->user, reset, data->message);
                    
                    free(data->user);
                    free(data->message);
                    free(data);
                }
            }
        }
    }

    close(sockfd);
}
