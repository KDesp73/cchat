#ifndef CONFIG_H
#define CONFIG_H

#define MAX_PENDING_CONNECTIONS 10
#define TIMEOUT_MS 50000
#define BUFFER_SIZE 1024
#define VERSION "1.0.8"
#define MAX_USERNAME_LENGTH 50
#define USERNAME_PATH clib_format_text("%s/.config/cchat/username", getenv("HOME"))
#define UNSET_PORT -65272 // Random (very slim chance this exact port will be entered)
#define ID_LENGTH 6

#endif // CONFIG_H
