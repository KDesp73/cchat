#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#define MAX_PENDING_CONNECTIONS 10
#define TIMEOUT_MS 50000
#define BUFFER_SIZE 256

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define moveup(x) printf("\033[%dA", x);
#define movedown(x) printf("\033[%dB", x);
#define moveright(x) printf("\033[%dC", x);
#define moveleft(x) printf("\033[%dD", x);


#define CONCAT(a, b) a""b

#define handle_error(msg) \
    do { perror(CONCAT("[ERRO] ", msg)); exit(EXIT_FAILURE); } while (0)

#define LOG(type, format, ...) \
    printf("[%s] " format, type, ##__VA_ARGS__); // '##' == if they exist

#define INFO(format, ...) \
    LOG("INFO", format, ##__VA_ARGS__);

#define ERRO(format, ...) \
    LOG("ERRO", format, ##__VA_ARGS__);

#define WARN(format, ...) \
    LOG("WARN", format, ##__VA_ARGS__);


int is_empty(const char *s) {
    while (*s != '\0') {
        if (!isspace((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

void connect_to(char* ip_address, int port){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = inet_addr(ip_address)
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

    while(1) {
        char buffer[BUFFER_SIZE] = { 0 };

        poll(fds, 2, TIMEOUT_MS);

        if (fds[0].revents & POLLIN) {
            read(0, buffer, BUFFER_SIZE-1);

            if(is_empty(buffer)) continue;

            send(sockfd, buffer, BUFFER_SIZE-1, 0);
        } else if (fds[1].revents & POLLIN) {
            if (recv(sockfd, buffer, BUFFER_SIZE-1, 0) == 0) {
                exit(0);
            }

            printf("%s\n", buffer);
        }
    }
}

void serve(char* ip_address, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = inet_addr(ip_address)
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

                send(clientfd, buffer, BUFFER_SIZE - 1, 0);
            } else if (fds[1].revents & POLLIN) {
                int bytes_received = recv(clientfd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_received <= 0) {
                    INFO("Client disconnected\n");
                    close(clientfd);
                    break;
                }

                printf("%s\n", buffer);
            }
        }
    }

    close(sockfd);
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


int main(int argc, char** argv){
    int port = -345678;
    char* ip_address = NULL;

    struct option long_options[] = {
        {"address", required_argument, NULL, 'a'},
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "a:p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'a':
                ip_address = optarg;
                break;
            default:
                INFO("Usage: %s [serve|connect] -a [option] -p [option]\n", argv[0]);
                exit(1);
        }
    }

    if(port ==  -345678){
        ERRO("No port specified\n");
        exit(1);
    } 

    if(ip_address == NULL){
        ERRO("No ip address specified\n");
        exit(1);
    }

    if(!isValidIpAddress(ip_address)){
        ERRO("Invalid ip address\n");
        exit(1);
    }

    // Port should range between 1024 and 49151
    if(port < 1024 || port > 49151) {
        ERRO("Port should range between 1024 and 49151\n");
        exit(1);
    }



    char* command = argv[argc-1];

    if(strcmp("serve", command) == 0){
        serve(ip_address, port);
    } else if(strcmp("connect", command) == 0){
        connect_to(ip_address, port);
    } else {
        ERRO("Invalid command: '%s'\n", command);
    }

    return 0;
}
