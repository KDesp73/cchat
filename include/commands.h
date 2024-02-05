#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>

/* COMMAND IDEAS TO IMPLEMENT
 *  Global
 *  \whoami: prints user's username
 *
 *  Server only
 *  \admin [username]: Make user admin (can use server-only commands)
 *  \kick [username]: Remove a user from the current chat room.
 *  \ban [username]: Ban a user from the chat.
 *  \mute [username]: Mute a user in the chat.
 *  \unmute [username]: Unmute a previously muted user.
 */

#define COMMAND_LIST "list"
#define COMMAND_HELP "help"
#define COMMAND_HELP_SHORT "?"
#define COMMAND_CLEAR "clear"
#define COMMAND_WHISPER "whisper"

static char* command_help[] = {
    COMMAND_LIST ": lists all users",
    COMMAND_HELP " or " COMMAND_HELP_SHORT ": prints this list",
    COMMAND_CLEAR ": clears the screen ",
    COMMAND_WHISPER ": \\whisper [username] [message] - send private messages",
};

char* list(char** usernames, size_t num_usernames);
char* help(char** commands, size_t size);
char* clear();
void whisper(int clientfd, int sockfd, char* buffer, char** usernames, size_t num_usernames);

#endif // COMMANDS_H
