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


int main(int argc, char** argv){
    srand(time(NULL));

    int port = -345678;
    char* ip_address = NULL;

    struct option long_options[] = {
        {"address", required_argument, NULL, 'a'},
        {"port", required_argument, NULL, 'p'},
        {"version", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "a:p:v", long_options, NULL)) != -1) {
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
            default:
                INFO("Usage: %s [serve|connect] -a [option] -p [option]\n", argv[0]);
                exit(1);
        }
    }

    if(port == -345678 && ip_address == NULL) {
        ERRO("No ip address specified\n");
        ERRO("No port specified\n");
        exit(1);
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

    char* username = read_file_line(username_path);


    if(strcmp("serve", command) == 0){
        serve(ip_address, port);
    } else if(strcmp("connect", command) == 0){
        if(username == NULL){
            username = (char*) calloc(strlen("user#") + 6, sizeof(char));
            WARN("Username not found\n");
            strcpy(username, "user#");
            strcat(username, random_string(6));
            INFO("Your username now is: %s\n", username);
        }

        if(username != NULL && strcmp(username, "server") == 0){
            WARN("Your username cannot be 'server'\n");
            memset(username, 0, strlen(username));
            username = (char*) calloc(strlen("user#") + 6, sizeof(char));
            strcpy(username, "user#");
            strcat(username, random_string(6));
            INFO("Your username now is: %s\n", username);
        }

        connect_to(ip_address, port, username); 
    } else {
        ERRO("Invalid command: '%s'\n", command);
    }

    return 0;
}
