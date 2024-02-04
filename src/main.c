#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"
#include "server.h"
#include "config.h"
#include "logging.h"
#include "client.h"

void check_username(char** username) {
    if (*username == NULL) {
        *username = (char *)calloc(strlen("user#") + 6, sizeof(char));
        WARN("Username not found\n");
        strcpy(*username, "user#");
        strcat(*username, random_string(6));
        INFO("Your username now is: %s\n", *username);
    }

    if (*username != NULL && strcmp(*username, "server") == 0) {
        WARN("Your username cannot be 'server'\n");
        free(*username);
        *username = (char *)calloc(strlen("user#") + 6, sizeof(char));
        strcpy(*username, "user#");
        strcat(*username, random_string(6));
        INFO("Your username now is: %s\n", *username);
    }
}
void check_address_and_port(char *ip_address, int port) {
    if (port == -345678 && ip_address == NULL) {
        ERRO("No ip address specified\n");
        ERRO("No port specified\n");
        exit(1);
    }

    if (port == -345678) {
        ERRO("No port specified\n");
        exit(1);
    }

    if (ip_address == NULL) {
        ERRO("No ip address specified\n");
        exit(1);
    }

    if (!is_valid_ip_address(ip_address)) {
        ERRO("Invalid ip address\n");
        exit(1);
    }

    // Port should range between 1024 and 49151
    if (port < 1024 || port > 49151) {
        ERRO("Port should range between 1024 and 49151\n");
        exit(1);
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    const char* home = getenv("HOME");
    const char* rest = "/.config/cchat/username";

    if(home == NULL){
        handle_error("HOME environment variable not set\n");
    }

    char username_path[strlen(home) + strlen(rest) + 1];
    strcpy(username_path, home);
    strcat(username_path, rest);

    int port = -345678;
    char *ip_address = NULL;
    char *arg_username = NULL;
    struct option long_options[] = {
        {"address", required_argument, NULL, 'a'},
        {"port", required_argument, NULL, 'p'},
        {"version", no_argument, NULL, 'v'},
        {"username", required_argument, NULL, 'u'},
        {"debug", no_argument, NULL, 'd'},
        {NULL, 0, NULL, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "a:p:vu:d", long_options, NULL)) != -1) {
        switch (opt) {
        case 'p':
            port = atoi(optarg);
            break;
        case 'a':
            ip_address = optarg;
            break;
        case 'v':
            printf("cchat v%s\n", VERSION);
            exit(0);
        case 'u':
            arg_username = optarg;
            break;
        case 'd':
            debug = 1;
            break;
        default:
            INFO("Usage: %s [serve|connect] -a [option] -p [option]\n", argv[0]);
            exit(1);
        }
    }

    DEBU("username_path: %s\n", username_path);
    DEBU("username: %s\n", read_file_line(username_path));

    check_address_and_port(ip_address, port);

    char *command = argv[argc - 1];

    char *file_username = read_file_line(username_path);

    if (strcmp("serve", command) == 0) {
        serve(ip_address, port, arg_username);
    } else if (strcmp("connect", command) == 0) {
        if(arg_username == NULL) check_username(&file_username);
        connect_to(ip_address, port, ((arg_username != NULL) ? arg_username : file_username));
    } else {
        ERRO("Invalid command: '%s'\n", command);
    }

    return 0;
}

