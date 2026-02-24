#include <stdio.h>
#include <unistd.h>
#if !defined(__ELKS__)
#include <sys/ioctl.h>
#endif
#include "ansi.h"

#define COLS   16
#define ROWS   15
#define Y_FIRST 0x10

static const int fg[] = { 30, 31, 32, 33, 34, 35, 36, 37, 90, 91, 92, 93, 94, 95, 96, 97 };
static const int bg[] = { 40, 41, 42, 43, 44, 45, 46, 47, 100, 101, 102, 103, 104, 105, 106, 107 };

#define ASCII_TABLE_WIDTH 42

static void pad_to(int n) {
    while (n-- > 0) putchar(' ');
}

#if defined(__ELKS__)
static int get_dims_ansi(int *rows, int *cols) {
    unsigned char buf[32];
    int i = 0;
    int r = 0, c = 0;

    printf("\033[999;999H\033[6n");
    fflush(stdout);

    while (i < (int)sizeof(buf)) {
        if (read(STDIN_FILENO, buf + i, 1) != 1) return -1;
        if (buf[i] == 'R') break;
        i++;
    }
    if (i >= (int)sizeof(buf) || buf[i] != 'R') return -1;
    buf[i] = '\0';

    i = 0;
    while (i < (int)sizeof(buf) && buf[i] != '[') i++;
    if (i >= (int)sizeof(buf) || buf[i] != '[') return -1;
    i++;
    for (r = 0; buf[i] >= '0' && buf[i] <= '9'; i++) r = r * 10 + (buf[i] - '0');
    if (buf[i] != ';') return -1;
    i++;
    for (c = 0; buf[i] >= '0' && buf[i] <= '9'; i++) c = c * 10 + (buf[i] - '0');
    if (r <= 0 || c <= 0) return -1;

    *rows = r;
    *cols = c;
    return 0;
}
#endif

static int get_terminal_size(int *rows, int *cols) {
#if defined(__ELKS__)
    return get_dims_ansi(rows, cols);
#else
    struct winsize ws;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != 0) return -1;
    *rows = (int)ws.ws_row;
    *cols = (int)ws.ws_col;
    return 0;
#endif
}

static void put_ascii_cell(int code) {
    if (code >= 0x20 && code != 0x7F)
        printf("%c ", (unsigned char)code);
    else
        printf(". ");
}

static void print_color_cell(int sgr, int i) {
    printf("\033[%dm %2d \033[0m", sgr, i);
}

int main(void) {
    int line = 0;
    int n;
    int rows = 0, cols = 0;

    if (get_terminal_size(&rows, &cols) == 0)
        printf("Terminal: %d x %d\n\n", cols, rows);

    n = printf("    ");
    for (int c = 0; c < COLS; c++) n += printf("%X ", c);
    pad_to(ASCII_TABLE_WIDTH - n);
    printf("  16 foreground colors\n");
    line++;

    for (int r = 0; r < ROWS; r++) {
        int y_hi = r + 1;
        n = 0;
        n += printf("%X0 ", y_hi);
        for (int c = 0; c < COLS; c++) {
            put_ascii_cell(y_hi * 0x10 + c);
            n += 2;
        }
        pad_to(ASCII_TABLE_WIDTH - n);
        printf("  ");
        if (line == 1) {
            for (int i = 0; i < 8; i++) print_color_cell(fg[i], i);
            putchar('\n');
        } else if (line == 2) {
            for (int i = 8; i < 16; i++) print_color_cell(fg[i], i);
            putchar('\n');
        } else if (line == 3) {
            printf("16 background colors\n");
        } else if (line == 4) {
            for (int i = 0; i < 8; i++) print_color_cell(bg[i], i);
            putchar('\n');
        } else if (line == 5) {
            for (int i = 8; i < 16; i++) print_color_cell(bg[i], i);
            putchar('\n');
        } else {
            putchar('\n');
        }
        line++;
    }
    printf(ANSI_RESET);
    return 0;
}
