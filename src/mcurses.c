#include "mcurses.h"
#include "ansi.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

static struct termios orig_termios;

static void raw_off(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf(ANSI_SHOW_CURSOR);
}

static void raw_on(void) {
    struct termios raw;

    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(raw_off);

    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    printf(ANSI_HIDE_CURSOR);
}

static void move(int row, int col) {
    /* ANSI CUP is 1-based; we use 0-based row/col internally */
    printf(ANSI_CUP_FMT, row + 1, col + 1);
}

void initscr(void) {
    raw_on();
    printf(ANSI_ALT_BUF_ON);
}

void endwin(void) {
    printf(ANSI_RESET);
    printf(ANSI_ALT_BUF_OFF);
    raw_off();
}

void clear(void) {
    printf(ANSI_CLS);
}

/* Use vsprintf (not vprintf): vsprintf is the standard "v" function usually
 * present in ELKS/bcc libc. */
int mvprintw(int row, int col, const char *fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    move(row, col);
    int n = vsprintf(buf, fmt, ap);
    va_end(ap);
    if (n >= 0)
        printf("%s", buf);
    return n >= 0 ? n : 0;
}

void refresh(void) {
    fflush(stdout);
}

int getch(void) {
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) == 1)
        return (unsigned char)c;
    return ERR;
}
