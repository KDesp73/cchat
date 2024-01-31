#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#define PORT 9876
#define MAX_PENDING_CONNECTIONS 10
#define TIMEOUT_MS 50000

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        AF_INET,
        htons(PORT),
        0
    };

    int bind_status = bind(sockfd, &address, sizeof(address));

    if(bind_status == -1) {
        fprintf(stderr, "Socket is busy. Try again in a moment\n");
        return 1;
    }

    listen(sockfd, MAX_PENDING_CONNECTIONS);

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

    while(1) {
        char buffer[256] = { 0 };

        poll(fds, 2, TIMEOUT_MS);

        if (fds[0].revents & POLLIN) {
            printf("> ");
            read(0, buffer, 255);
            send(clientfd, buffer, 255, 0);
        } else if (fds[1].revents & POLLIN) {
            if (recv(clientfd, buffer, 255, 0) == 0) {
                return 0;
            }

            printf("%s\n", buffer);
        }
    }

    return 0;
}
