#include "screen.h"

#include <stdlib.h>
#include <string.h>
#define STRINGIFY(x) #x 

char* color(int color_i) {
    if (color_i < 0 || color_i > 255) return "";
    
    char* color_str = (char*) malloc(11 * sizeof(char));
    strcpy(color_str, "\e[38;5;");
    strcat(color_str, STRINGIFY(color_i));
    strcat(color_str, "m");

    return color_str;
}
