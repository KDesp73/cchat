#ifndef SERVER_H
#define SERVER_H

void serve(const char* ip_address, int port, char* username);
struct Data create_error_data(const char* message);
struct Data create_info_data(const char* message);
struct Data create_warning_data(const char* message);
void close_server(int sockfd);


#endif // SERVER_H
