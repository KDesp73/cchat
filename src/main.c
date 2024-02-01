#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>

#include "data.h"
#include "utils.h"
#include "server.h"
#include "logging.h"
#include "client.h"


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

    if(strcmp("serve", command) == 0){
        serve(ip_address, port);
    } else if(strcmp("connect", command) == 0){
        connect_to(ip_address, port);
    } else {
        ERRO("Invalid command: '%s'\n", command);
    }

    return 0;
}
