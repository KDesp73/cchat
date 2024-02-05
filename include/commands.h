#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>

/* COMMAND IDEAS
 *  Global
 *  \whisper [username] [message]: Send a private message to a specific user.
 *
 *  Server only
 *  \kick [username]: Remove a user from the current chat room.
 *  \ban [username]: Ban a user from the chat.
 *  \mute [username]: Mute a user in the chat.
 *  \unmute [username]: Unmute a previously muted user.
 */

#define COMMAND_LIST "list"
#define COMMAND_HELP "help"
#define COMMAND_HELP_SHORT "?"
#define COMMAND_CLEAR "clear"

static char* commands[] = {
    COMMAND_LIST,
    COMMAND_HELP " or " COMMAND_HELP_SHORT,
    COMMAND_CLEAR,
};

char* list(char** usernames, size_t num_usernames);
char* help(char** commands, size_t size);
char* clear();

#endif // COMMANDS_H
