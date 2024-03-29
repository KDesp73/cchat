#include <signal.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include "client.h"
#include "data.h"
#include "logging.h"
#include "screen.h"
#include "config.h"
#include "utils.h"

void client_siginthandler(int params){
    INFO("Exited\n");
    exit(0); 
}

void connect_to(const char* ip_address, int port, char* username){
	signal(SIGINT, client_siginthandler);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    DEBU("sockfd: %d\n", sockfd);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {inet_addr(ip_address)}
    };

    int connect_status = connect(sockfd, (struct sockaddr*)&address, sizeof(address));

    if(connect_status == -1) {
        handle_error("Connect failed");
    }

    struct pollfd fds[2] = {
        {
            STDIN_FILENO,
            POLLIN,
            0
        },
        {
            sockfd,
            POLLIN,
            0
        }
    };

    // Send initial message to the server
    struct Data data = {
        .user = username,
        .time = get_current_time(),
        .status = INFORMATION,
        .message = "OK?"
    };

    send(sockfd, data_to_string(data), BUFFER_SIZE, 0);

    while(1) {
        char buffer[BUFFER_SIZE] = { 0 };

        poll(fds, 2, TIMEOUT_MS);

        if (fds[0].revents & POLLIN) {
            read(0, buffer, BUFFER_SIZE-1);

            if(is_empty(buffer) || is_ansi(buffer)) continue;

            buffer[strcspn(buffer, "\n")] = 0; // remove newline

            struct Data data = {
                .id = sockfd,
                .user = username,
                .message = buffer,
                .status = MESSAGE,
                .time = get_current_time()
            };

            send(sockfd, data_to_string(data), BUFFER_SIZE-1, 0);
        } else if (fds[1].revents & POLLIN) {
            if (recv(sockfd, buffer, BUFFER_SIZE-1, 0) == 0) {
                exit(0);
            }

            struct Data* data = string_to_data(buffer);

            if(data){
                switch (data->status) {
                    case MESSAGE:
                        print_message(data);
                        break;
                    case ERROR:
                        ERRO("%s\n", data->message);
                        break;
                    case INFORMATION: 
                        INFO("%s\n", data->message);
                        break;
                    case WARNING:
                        WARN("%s\n", data->message);
                        break;
                    case COMMAND:
                        printf("%s\n", data->message);
                        break;
                    default:
                        fprintf(stderr, "Invalid status: %zu\n", data->status);
                        break;
                }


                free(data->user);
                free(data->message);
                free(data);
            }

            fflush(stdin);
        }
    }
}
