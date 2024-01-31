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

void connect_to(char* ip_address, int port){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = inet_addr(ip_address)
    };

    int connect_status = connect(sockfd, (struct sockaddr*)&address, sizeof(address));

    if(connect_status == -1) {
        perror("Connect failed");
        exit(1);
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
            send(sockfd, buffer, BUFFER_SIZE-1, 0);
        } else if (fds[1].revents & POLLIN) {
            if (recv(sockfd, buffer, BUFFER_SIZE-1, 0) == 0) {
                exit(0);
            }

            printf("%s\n", buffer);
        }
    }
}

void serve(char* ip_address, int port){ 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = inet_addr(ip_address)
    };

    if(bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("Bind Failed");
        exit(1);
    }

    if(listen(sockfd, MAX_PENDING_CONNECTIONS) < 0){
        perror("Listen failed");
        exit(1);
    }

    int clientfd = accept(sockfd, 0, 0);

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

    printf("Press Ctrl+C to terminate\n");
    while(1) {
        char buffer[BUFFER_SIZE] = { 0 };

        poll(fds, 2, TIMEOUT_MS);

        if (fds[0].revents & POLLIN) {
            read(0, buffer, BUFFER_SIZE-1);
            send(clientfd, buffer, BUFFER_SIZE-1, 0);
        } else if (fds[1].revents & POLLIN) {
            if (recv(clientfd, buffer, BUFFER_SIZE-1, 0) == 0) {
                exit(0);
            }

            printf("%s\n", buffer);
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

int main(int argc, char** argv){
    int port;
    char* ip_address;

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
                exit(1);
        }
    }

    char* command = argv[argc-1];

    if(strcmp("serve", command) == 0){
        serve(ip_address, port);
    } else if(strcmp("connect", command) == 0){
        connect_to(ip_address, port);
    } else {
        printf("[ERRO] Invalid command: '%s'\n", command);
    }

    return 0;
}
