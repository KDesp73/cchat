#ifndef SERVER_H
#define SERVER_H

void serve(const char* ip_address, int port, char* username);
struct Data create_error_data(const char* message);

#endif // SERVER_H
