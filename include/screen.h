#ifndef SCREEN_H
#define SCREEN_H

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define moveup(x) printf("\033[%dA", x);
#define movedown(x) printf("\033[%dB", x);
#define moveright(x) printf("\033[%dC", x);
#define moveleft(x) printf("\033[%dD", x);

#endif // SCREEN_H
