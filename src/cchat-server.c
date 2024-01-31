#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#define PORT 9876
#define MAX_PENDING_CONNECTIONS 10
#define TIMEOUT_MS 50000


int terminated = 0;

void siginthandler(int args){
    terminated = 1;
}

int main() {
	signal(SIGINT, siginthandler);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        AF_INET,
        htons(PORT),
        0
    };

    if(bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("Bind Failed");
        return 1;
    }

    if(listen(sockfd, MAX_PENDING_CONNECTIONS) < 0){
        perror("Listen failed");
        return 1;
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
    while(!terminated) {
        char buffer[256] = { 0 };

        poll(fds, 2, TIMEOUT_MS);

        if (fds[0].revents & POLLIN) {
            read(0, buffer, 255);
            send(clientfd, buffer, 255, 0);
        } else if (fds[1].revents & POLLIN) {
            if (recv(clientfd, buffer, 255, 0) == 0) {
                return 0;
            }

            printf("%s\n", buffer);
        }
    }

    close(sockfd);

    return 0;
}
