#ifndef SCREEN_H
#define SCREEN_H

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define moveup(x) printf("\033[%dA", x);
#define movedown(x) printf("\033[%dB", x);
#define moveright(x) printf("\033[%dC", x);
#define moveleft(x) printf("\033[%dD", x);

#define black "\e[0;30m"
#define red "\e[0;31m"
#define green "\e[0;32m"
#define yellow "\e[0;33m"
#define blue "\e[0;34m"
#define purple "\e[0;35m"
#define cyan "\e[0;36m"
#define lgrey "\e[0;37m"
#define dgrey "\e[0;38m"
#define reset "\e[0;39m"


#endif // SCREEN_H
