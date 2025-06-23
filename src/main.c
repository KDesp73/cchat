#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>

#define CLIB_IMPLEMENTATION
#include "clib.h"

#include "utils.h"
#include "server.h"
#include "config.h"
#include "client.h"


int check_username(const char* username) {
    if (username == NULL) {
        WARN("Username not found");
    } else if(is_empty(username)) {
        WARN("Username is empty");
    } else if(strcmp(username, "server") == 0) {
        WARN("Your username cannot be 'server'");
    } else if(strlen(username) > MAX_USERNAME_LENGTH){
        WARN("Your username cannot be more than %d characters", MAX_USERNAME_LENGTH);
    } else {
        return true; 
    }
    return false;
}


void check_address_and_port(char *ip_address, int port) {
    if (port == UNSET_PORT && ip_address == NULL) {
        ERRO("No ip address specified");
        ERRO("No port specified");
        exit(1);
    }

    if (port == UNSET_PORT) {
        ERRO("No port specified");
        exit(1);
    }

    if (ip_address == NULL) {
        ERRO("No ip address specified");
        exit(1);
    }

    if (!is_valid_ip_address(ip_address)) {
        ERRO("Invalid ip address");
        exit(1);
    }

    // Port should range between 1024 and 49151
    if (port < 1024 || port > 49151) {
        ERRO("Port should range between 1024 and 49151");
        exit(1);
    }
}

const char* determine_username(const char* command, const char* arg, const char* file)
{
    if(STRCMP(command, "serve") && !check_username(arg)) return clib_format_text("server");

    if(!check_username(arg) && !check_username(file)){
        char* id = random_string(ID_LENGTH);
        char* username = clib_format_text("user#%s", id);
        free(id);
        return username;
    }

    if(!check_username(arg)) {
        return file;
    }

    return arg;
}

int main(int argc, char **argv) {
    srand(time(NULL));
    CliArguments args = clib_make_cli_arguments(5, 
        clib_create_argument('a', "address", "Sets the ip address that the chat is being hosted on", required_argument),
        clib_create_argument('p', "port", "Sets the port", required_argument),
        clib_create_argument('v', "version", "Prints the program's version", no_argument),
        clib_create_argument('h', "help", "Prints this message", no_argument),
        clib_create_argument('u', "username", "Sets the username for the current session (overrides the file one)", required_argument)
    );

    int port = UNSET_PORT;
    char* ip_address = NULL;
    char* arg_username = NULL;
    struct option* opts = clib_get_options(args);
    char* usage = clib_format_text("%s [serve | connect] -a <address> -p <port> [-h] [-v]", argv[0]);
    char* args_fmt = clib_generate_cli_format_string(args);

    int opt;
    while ((opt = getopt_long(argc, argv, args_fmt, opts, NULL)) != -1) {
        switch (opt) {
        case 'p':
            port = atoi(optarg);
            break;
        case 'a':
            ip_address = optarg;
            break;
        case 'v':
            printf("cchat v%s\n", VERSION);
            free(usage);
            free(args_fmt);
            exit(0);
        case 'u':
            arg_username = optarg;
            break;
        case 'h':
            clib_cli_help(args, usage, "Made by KDesp73");
            free(usage);
            free(args_fmt);
            exit(0);
        default:
            INFO("Usage: %s", usage);
            exit(1);
        }
    }
    free(args_fmt);

    check_address_and_port(ip_address, port);

    char *command = argv[argc - 1];

    char* username_path = USERNAME_PATH;
    char *file_username = read_file_line(username_path);
    free(username_path);

    const char* username = determine_username(command, arg_username, file_username);  

    if(username == NULL) {
        PANIC("Could not deternine the username");
        LOG(stderr, "TIP", "Try setting your username in the `~/.config/cchat/username` file");
    }

    if (strcmp("serve", command) == 0) {
        serve(ip_address, port, username);
    } else if (strcmp("connect", command) == 0) {
        connect_to(ip_address, port, username);
    } else {
        ERRO("Invalid command: '%s'", command);
    }

    free(file_username);

    return 0;
}

