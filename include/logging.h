#ifndef LOGGING_H
#define LOGGING_H
#include <stdlib.h>


#define CONCAT(a, b) a "" b
#define handle_error(msg) \
    do { perror(CONCAT("[ERRO] ", msg)); exit(1); } while (0)

#define LOG(type, format, ...) \
    printf("[%s] " format, type, ##__VA_ARGS__); // '##' == if they exist

#define INFO(format, ...) \
    LOG("INFO", format, ##__VA_ARGS__);

#define ERRO(format, ...) \
    LOG("ERRO", format, ##__VA_ARGS__);

#define WARN(format, ...) \
    LOG("WARN", format, ##__VA_ARGS__);

#ifdef DEBUG
    #define DEBU(format, ...) \
        do{ LOG("DEBU", format, ##__VA_ARGS__); } while(0);
#else
    #define DEBU(format, ...) 
#endif // DEBUG

#endif // LOGGING_H
