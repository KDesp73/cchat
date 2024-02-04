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
#include "config.h"
#include "utils.h"

void connect_to(const char* ip_address, int port, char* username){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

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
            0, // stdin
            POLLIN,
            0
        },
        {
            sockfd,
            POLLIN,
            0
        }
    };

    INFO("Connected as: %s\n", username);
    while(1) {
        char buffer[BUFFER_SIZE] = { 0 };

        poll(fds, 2, TIMEOUT_MS);

        if (fds[0].revents & POLLIN) {
            read(0, buffer, BUFFER_SIZE-1);

            if(is_empty(buffer)) continue;

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
                    default:
                        fprintf(stderr, "Invalid status\n");
                        break;
                }


                free(data->user);
                free(data->message);
                free(data);
            }
        }
    }
}
