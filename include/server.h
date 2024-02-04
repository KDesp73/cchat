#ifndef SERVER_H
#define SERVER_H

void serve(const char* ip_address, int port, char* username);
struct Data create_data(const char* message, int status);
void close_server(int sockfd);
void run_command(char* command, int clientfd);


#endif // SERVER_H
