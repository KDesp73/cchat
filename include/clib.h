/*
 * MIT License
 * 
 * Copyright (c) 2024 Konstantinos Despoinidis
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/


/*
 * Usage: 
 * #define CLIB_IMPLEMENTATION
 * #define CLIB_MENUS // if you want to use the menu methods
 * #inlcude "clib.h"
 *
 * -[TOC]-
 * 1. SYSTEM
 * 2. MEMORY
 * 3. MENUS // needs its own define!
 * 4. UTILS
 * 5. ANSI
 * 6. FILES
 * 7. LOGGING
 * */

#ifndef CLIB_H
#define CLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>


// START [TYPES] START //
typedef const char * Cstr;

typedef struct {
    Cstr* items;
    size_t count;
} CstrArray;
// END [TYPES] END//

// START [DECLARATIONS] START //
static CstrArray clib_cstr_array_make(Cstr first, ...);
static Cstr clib_cstr_array_join(Cstr sep, CstrArray cstrs);

#ifndef _WIN32
    #define PATH_SEP "/"
#else 
    #define PATH_SEP "\\"
#endif // _WIN32

#define true 1
#define false 0
#define BOOL(x) (x ? "true" : "false")

// ANSI
#define RESET "\e[0;39m"
#define BOLD "\e[1m"
#define UNDERLINE "\033[4m"
#define ITALIC "\e[3m"
#define CLEAR "\e[2J"
#define ERASE_LINE "\e[2K"
#define HIDE_CURSOR printf("\e[?25l")
#define SHOW_CURSOR printf("\e[?25h")
#define MOVE_CURSOR_UP(lines) printf("\033[%zuA", lines)
#define CLEAR_BELOW_CURSOR printf("\033[J")

static Cstr clib_color(int color, int bg);
static void clib_clear_screen();
static void clib_print_color_table();

#define COLOR_BG(c) clib_color(c, 1)
#define COLOR_FG(c) clib_color(c, 0)

// SYSTEM
static char* clib_execute_command(const char* command);
static char* clib_get_env(const char* varname);
static int clib_set_env(const char* varname, const char* value, int overwrite);
static int clib_unset_env(const char* varname);

// MEMORY
static void* clib_safe_malloc(size_t size);
static void* clib_safe_calloc(size_t nmemb, size_t size);
static void* clib_safe_realloc(void *ptr, size_t size);
static void clib_safe_free(void **ptr);

// FILES
static void clib_create_file(const char *filename);
static void clib_write_file(const char *filename, const char *data);
static char* clib_read_file(const char *filename);
static void clib_delete_file(const char *filename);
static void clib_append_file(const char *filename, const char *data);
static void clib_copy_file(const char *source, const char *destination);
static void clib_move_file(const char *source, const char *destination);
static long clib_file_size(const char *filename);
static int clib_file_exists(const char *filename);

// UTILS
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define SWAP(x, y) do { \
    typeof(x) temp = x; \
    x = y; \
    y = temp; \
} while (0)

#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY(x) __builtin_expect(!!(x), 1)
#else
    #define LIKELY(x) (x)
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define UNLIKELY(x) (x)
#endif
static int clib_eu_mod(int a, int b);
static char* clib_shift_args(int *argc, char ***argv);
#define ITOA(s, i) sprintf(s, "%d", i);
#define JOIN(sep, ...) clib_cstr_array_join(sep, clib_cstr_array_make(__VA_ARGS__, NULL))
#define CONCAT(...) JOIN("", __VA_ARGS__)
#define PATH(...) JOIN(PATH_SEP, __VA_ARGS__)

// LOGGING
#define handle_error(msg) \
    do { perror(CONCAT("[ERRO] ", msg, "\n")); exit(1); } while (0)

#define LOG(stream, type, format, ...) \
    fprintf(stream, CONCAT("[%s] ", format, "\n"), type, ##__VA_ARGS__)

#define INFO(format, ...) \
    LOG(stdout, "INFO", format, ##__VA_ARGS__)

#define ERRO(format, ...) \
    LOG(stderr, "ERRO", format, ##__VA_ARGS__)

#define WARN(format, ...) \
    LOG(stderr, "WARN", format, ##__VA_ARGS__)

#ifdef DEBUG
    #define DEBU(format, ...) \
        LOG(stderr, "DEBU", format, ##__VA_ARGS__)
#else
    #define DEBU(format, ...) 
#endif // DEBUG

#define PANIC(format, ...)                            \
    do {                                              \
        LOG(stderr, "PANIC", format, ##__VA_ARGS__);  \
        exit(1);                                      \
    } while(0)

#define DEMO(expr)                    \
    do {                              \
        LOG(stdout, "DEMO", #expr);   \
        expr;                         \
    } while(0)

// MENUS
#ifdef CLIB_MENUS
#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>

    int _getch();
#endif

// TODO: Add more keys
typedef enum {
    CLIB_KEY_UNKNOWN = -1,
    CLIB_KEY_ENTER = 10,
    CLIB_KEY_BACKSPACE = 127,
    CLIB_KEY_TAB = 9,
    CLIB_KEY_ESC = 27,
    CLIB_KEY_SPACE = 32,
    CLIB_KEY_0 = '0',
    CLIB_KEY_1 = '1',
    CLIB_KEY_2 = '2',
    CLIB_KEY_3 = '3',
    CLIB_KEY_4 = '4',
    CLIB_KEY_5 = '5',
    CLIB_KEY_6 = '6',
    CLIB_KEY_7 = '7',
    CLIB_KEY_8 = '8',
    CLIB_KEY_9 = '9',
    CLIB_KEY_A = 'A',
    CLIB_KEY_B = 'B',
    CLIB_KEY_C = 'C',
    CLIB_KEY_D = 'D',
    CLIB_KEY_E = 'E',
    CLIB_KEY_F = 'F',
    CLIB_KEY_G = 'G',
    CLIB_KEY_H = 'H',
    CLIB_KEY_I = 'I',
    CLIB_KEY_J = 'J',
    CLIB_KEY_K = 'K',
    CLIB_KEY_L = 'L',
    CLIB_KEY_M = 'M',
    CLIB_KEY_N = 'N',
    CLIB_KEY_O = 'O',
    CLIB_KEY_P = 'P',
    CLIB_KEY_Q = 'Q',
    CLIB_KEY_R = 'R',
    CLIB_KEY_S = 'S',
    CLIB_KEY_T = 'T',
    CLIB_KEY_U = 'U',
    CLIB_KEY_V = 'V',
    CLIB_KEY_W = 'W',
    CLIB_KEY_X = 'X',
    CLIB_KEY_Y = 'Y',
    CLIB_KEY_Z = 'Z',
    CLIB_KEY_a = 'a',
    CLIB_KEY_b = 'b',
    CLIB_KEY_c = 'c',
    CLIB_KEY_d = 'd',
    CLIB_KEY_e = 'e',
    CLIB_KEY_f = 'f',
    CLIB_KEY_g = 'g',
    CLIB_KEY_h = 'h',
    CLIB_KEY_i = 'i',
    CLIB_KEY_j = 'j',
    CLIB_KEY_k = 'k',
    CLIB_KEY_l = 'l',
    CLIB_KEY_m = 'm',
    CLIB_KEY_n = 'n',
    CLIB_KEY_o = 'o',
    CLIB_KEY_p = 'p',
    CLIB_KEY_q = 'q',
    CLIB_KEY_r = 'r',
    CLIB_KEY_s = 's',
    CLIB_KEY_t = 't',
    CLIB_KEY_u = 'u',
    CLIB_KEY_v = 'v',
    CLIB_KEY_w = 'w',
    CLIB_KEY_x = 'x',
    CLIB_KEY_y = 'y',
    CLIB_KEY_z = 'z',
    // Defined later
    CLIB_KEY_ARROW_UP,
    CLIB_KEY_ARROW_DOWN,
    CLIB_KEY_ARROW_LEFT,
    CLIB_KEY_ARROW_RIGHT
} ClibKey;

typedef void (*ClibPrintOptionFunc)(Cstr option, int is_selected, int color);
static void clib_default_print_option(Cstr option, int is_selected, int color);
static void clib_arrow_print_option(Cstr option, int is_selected, int color);
static void clib_brackets_print_option(Cstr option, int is_selected, int color);
static void clib_enable_input_buffering();
static void clib_disable_input_buffering();
static int clib_getch();
static int clib_menu(Cstr title, int color, ClibPrintOptionFunc print_option, Cstr first_option, ...);
#endif // CLIB_MENUS

// END [DECLARATIONS] END//

// START [IMPLEMENTATIONS] START //
#ifdef CLIB_IMPLEMENTATION
#ifdef CLIB_MENUS
#ifndef _WIN32
    int _getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

static void clib_enable_input_buffering(){
    #ifdef _WIN32
        // Enable console input buffering
        HANDLE hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

        DWORD consoleMode;
        GetConsoleMode(hConsoleInput, &consoleMode);

        consoleMode |= ENABLE_LINE_INPUT;

        SetConsoleMode(hConsoleInput, consoleMode);
    #else
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= (ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &term);

    #endif
    SHOW_CURSOR;
}

static void clib_disable_input_buffering(){
    #ifdef _WIN32
        // Disable console input buffering
        DWORD mode;
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, mode & ~ENABLE_ECHO_INPUT & ~ENABLE_LINE_INPUT);
    #else
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &term);

    #endif
    HIDE_CURSOR;
}

static int clib_getch() {
    int ch;
    #ifdef _WIN32
        ch = _getch();
        if (ch == 0 || ch == 224) {
            // Handle extended keys (arrows, function keys)
            ch = _getch();
            switch (ch) {
                case 72: return CLIB_KEY_ARROW_UP;
                case 80: return CLIB_KEY_ARROW_DOWN;
                case 75: return CLIB_KEY_ARROW_LEFT;
                case 77: return CLIB_KEY_ARROW_RIGHT;
                default: return CLIB_KEY_UNKNOWN;
            }
        }
    #else
        ch = _getch();
        if (ch == 27) {
            if (getchar() == '[') {
                switch (getchar()) {
                    case 'A': return CLIB_KEY_ARROW_UP;
                    case 'B': return CLIB_KEY_ARROW_DOWN;
                    case 'C': return CLIB_KEY_ARROW_RIGHT;
                    case 'D': return CLIB_KEY_ARROW_LEFT;
                    default: return CLIB_KEY_UNKNOWN;
                }
            }
            return CLIB_KEY_ESC;
        }
    #endif

    switch (ch) {
        case 10: return CLIB_KEY_ENTER;
        case 127: return CLIB_KEY_BACKSPACE;
        case 9: return CLIB_KEY_TAB;
        case 27: return CLIB_KEY_ESC;
        case 32: return CLIB_KEY_SPACE;
        case '0': return CLIB_KEY_0;
        case '1': return CLIB_KEY_1;
        case '2': return CLIB_KEY_2;
        case '3': return CLIB_KEY_3;
        case '4': return CLIB_KEY_4;
        case '5': return CLIB_KEY_5;
        case '6': return CLIB_KEY_6;
        case '7': return CLIB_KEY_7;
        case '8': return CLIB_KEY_8;
        case '9': return CLIB_KEY_9;
        case 'A': return CLIB_KEY_A;
        case 'B': return CLIB_KEY_B;
        case 'C': return CLIB_KEY_C;
        case 'D': return CLIB_KEY_D;
        case 'E': return CLIB_KEY_E;
        case 'F': return CLIB_KEY_F;
        case 'G': return CLIB_KEY_G;
        case 'H': return CLIB_KEY_H;
        case 'I': return CLIB_KEY_I;
        case 'J': return CLIB_KEY_J;
        case 'K': return CLIB_KEY_K;
        case 'L': return CLIB_KEY_L;
        case 'M': return CLIB_KEY_M;
        case 'N': return CLIB_KEY_N;
        case 'O': return CLIB_KEY_O;
        case 'P': return CLIB_KEY_P;
        case 'Q': return CLIB_KEY_Q;
        case 'R': return CLIB_KEY_R;
        case 'S': return CLIB_KEY_S;
        case 'T': return CLIB_KEY_T;
        case 'U': return CLIB_KEY_U;
        case 'V': return CLIB_KEY_V;
        case 'W': return CLIB_KEY_W;
        case 'X': return CLIB_KEY_X;
        case 'Y': return CLIB_KEY_Y;
        case 'Z': return CLIB_KEY_Z;
        case 'a': return CLIB_KEY_a;
        case 'b': return CLIB_KEY_b;
        case 'c': return CLIB_KEY_c;
        case 'd': return CLIB_KEY_d;
        case 'e': return CLIB_KEY_e;
        case 'f': return CLIB_KEY_f;
        case 'g': return CLIB_KEY_g;
        case 'h': return CLIB_KEY_h;
        case 'i': return CLIB_KEY_i;
        case 'j': return CLIB_KEY_j;
        case 'k': return CLIB_KEY_k;
        case 'l': return CLIB_KEY_l;
        case 'm': return CLIB_KEY_m;
        case 'n': return CLIB_KEY_n;
        case 'o': return CLIB_KEY_o;
        case 'p': return CLIB_KEY_p;
        case 'q': return CLIB_KEY_q;
        case 'r': return CLIB_KEY_r;
        case 's': return CLIB_KEY_s;
        case 't': return CLIB_KEY_t;
        case 'u': return CLIB_KEY_u;
        case 'v': return CLIB_KEY_v;
        case 'w': return CLIB_KEY_w;
        case 'x': return CLIB_KEY_x;
        case 'y': return CLIB_KEY_y;
        case 'z': return CLIB_KEY_z;
        // Add other mappings as needed
        default: return CLIB_KEY_UNKNOWN;
    }
}


static void clib_default_print_option(Cstr option, int is_selected, int color){
    is_selected ? printf("%s%s%s", COLOR_BG(color), option, RESET) : printf("%s", option);
}

static void clib_arrow_print_option(Cstr option, int is_selected, int color){
    is_selected ? printf("%s>%s %s", COLOR_FG(color), RESET, option) : printf("  %s", option);
}

static void clib_brackets_print_option(Cstr option, int is_selected, int color){
    is_selected ? printf("%s[%s%s%s]%s", COLOR_FG(color), RESET, option, COLOR_FG(color), RESET) : printf(" %s ", option);
}

static int clib_menu(Cstr title, int color, ClibPrintOptionFunc print_option, Cstr first_option, ...){
    clib_disable_input_buffering();

    int selected = 0;
    size_t size = 0;
    CstrArray options = {0};

    if (first_option == NULL) {
        return -1;
    }

    va_list args;
    va_start(args, first_option);
    options.count += 1;

    for (Cstr next = va_arg(args, Cstr); next != NULL; next = va_arg(args, Cstr)) {
        options.count += 1;
    }
    va_end(args);

    options.items = (Cstr*) malloc(sizeof(options.items[0]) * options.count);
    if (options.items == NULL) {
        PANIC("could not allocate memory: %s", strerror(errno));
    }
    options.count = 0;

    options.items[options.count] = first_option;
    options.count += 1;

    va_start(args, first_option);
    for (Cstr next = va_arg(args, Cstr); next != NULL; next = va_arg(args, Cstr)) {
        options.items[options.count++] = next;
    }

    va_end(args);

    while(true){
        if(title != NULL){
            printf("%s%s%s\n", COLOR_FG(color), title, RESET);
        }
        for(size_t i = 0; i < options.count; ++i){
            print_option(options.items[i], selected == i, color);
            printf("\n");
        }
        
        int pressed = clib_getch();
        switch (pressed) {
            case CLIB_KEY_ARROW_UP:
                selected = clib_eu_mod((selected-1), options.count);
                break;
            case CLIB_KEY_ARROW_DOWN:
                selected = clib_eu_mod((selected+1), options.count);
                break;
            case CLIB_KEY_ENTER:
                clib_enable_input_buffering();
                free(options.items);
                return selected; 
            default:
                break;
        }

        MOVE_CURSOR_UP(options.count + (title != NULL));
        CLEAR_BELOW_CURSOR;
    }
}
#endif // CLIB_MENUS

static int clib_eu_mod(int a, int b){
    if (b == 0) {
        // Handle division by zero case
        fprintf(stderr, "Error: Division by zero is undefined.\n");
        return 0;
    }
    
    int r = a % b;
    if (r < 0) {
        r += (b > 0) ? b : -b;
    }
    return r;
}

static CstrArray clib_cstr_array_make(Cstr first, ...) {
    CstrArray result = {0};

    if (first == NULL) {
        return result;
    }

    result.count += 1;

    va_list args;
    va_start(args, first);
    for (Cstr next = va_arg(args, Cstr); next != NULL; next = va_arg(args, Cstr)) {
        result.count += 1;
    }
    va_end(args);

    result.items = (Cstr*) malloc(sizeof(result.items[0]) * result.count);
    if (result.items == NULL) {
        PANIC("could not allocate memory: %s", strerror(errno));
    }
    result.count = 0;

    result.items[result.count++] = first;

    va_start(args, first);
    for (Cstr next = va_arg(args, Cstr); next != NULL; next = va_arg(args, Cstr)) {
        result.items[result.count++] = next;
    }
    va_end(args);

    return result;
}

static Cstr clib_cstr_array_join(Cstr sep, CstrArray cstrs) {
    if (cstrs.count == 0) {
        return "";
    }

    const size_t sep_len = strlen(sep);
    size_t len = 0;
    for (size_t i = 0; i < cstrs.count; ++i) {
        len += strlen(cstrs.items[i]);
    }

    const size_t result_len = (cstrs.count - 1) * sep_len + len + 1;
    char *result = malloc(sizeof(char) * result_len);
    if (result == NULL) {
        PANIC("could not allocate memory: %s", strerror(errno));
    }

    len = 0;
    for (size_t i = 0; i < cstrs.count; ++i) {
        if (i > 0) {
            memcpy(result + len, sep, sep_len);
            len += sep_len;
        }

        size_t elem_len = strlen(cstrs.items[i]);
        memcpy(result + len, cstrs.items[i], elem_len);
        len += elem_len;
    }
    result[len] = '\0';

    return result;
}

static char* clib_shift_args(int *argc, char ***argv) {
    assert(*argc > 0);
    char *result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}

static Cstr clib_color(int color, int bg) {
    if (color < 0 || color > 255) return "";

    char where_code[12], color_string[12];
    ITOA(where_code, bg + 3);
    ITOA(color_string, color);

    return CONCAT("\e[", where_code, "8;5;", color_string, "m");
}

static void clib_clear_screen() {
#ifdef _WIN32
    system("cls"); // Clear screen for Windows
#else
    system("clear"); // Clear screen for Linux and macOS
#endif
}


static void clib_print_color_table(){
    for(int i = 0; i < 256; i++){
        if(i % 21 == 0) printf("\n");
        
        printf("%s%3d ", clib_color(i, 0), i);
    }
    printf("%s\n", RESET);
}


static void clib_copy_file(const char *source, const char *destination) {
    FILE *srcFile = fopen(source, "r");
    if (srcFile == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    FILE *destFile = fopen(destination, "w");
    if (destFile == NULL) {
        perror("Error opening destination file");
        fclose(srcFile);
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0) {
        if (fwrite(buffer, 1, bytesRead, destFile) != bytesRead) {
            perror("Error writing to destination file");
            fclose(srcFile);
            fclose(destFile);
            exit(EXIT_FAILURE);
        }
    }

    fclose(srcFile);
    fclose(destFile);
}

static void clib_move_file(const char *source, const char *destination) {
    if (rename(source, destination) != 0) {
        perror("Error moving/renaming file");
        exit(EXIT_FAILURE);
    }
}

static long clib_file_size(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

static int clib_file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file);
        return 1;
    }
    return 0;
}

static void clib_append_file(const char *filename, const char *data) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file for appending");
        exit(EXIT_FAILURE);
    }
    if (fputs(data, file) == EOF) {
        perror("Error appending to file");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

static void clib_create_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

static void clib_write_file(const char *filename, const char *data) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    if (fputs(data, file) == EOF) {
        perror("Error writing to file");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

static char* clib_read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, file_size, file);
    if (bytesRead != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}

static void clib_delete_file(const char *filename) {
    if (remove(filename) != 0) {
        perror("Error deleting file");
        exit(EXIT_FAILURE);
    }
}

static void* clib_safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static void* clib_safe_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static void* clib_safe_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        fprintf(stderr, "Memory reallocation error\n");
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

static void clib_safe_free(void **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

static char* clib_execute_command(const char* command) {
    char buffer[128];
    char *result = NULL;
    size_t result_size = 0;
    FILE *pipe = popen(command, "r");
    if (!pipe) {
        return NULL;
    }

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t buffer_len = strlen(buffer);
        result = realloc(result, result_size + buffer_len + 1);
        if (!result) {
            pclose(pipe);
            return NULL;
        }
        strcpy(result + result_size, buffer);
        result_size += buffer_len;
    }

    pclose(pipe);
    return result;
}

static char* clib_get_env(const char* varname) {
    return getenv(varname);
}

static int set_envclib_(const char* varname, const char* value, int overwrite) {
    return setenv(varname, value, overwrite);
}

static int clib_unset_env(const char* varname) {
    return unsetenv(varname);
}

#endif // CLIB_IMPLEMENTATION
// END [IMPLEMENTATIONS] END//



#endif // CLIB_H

