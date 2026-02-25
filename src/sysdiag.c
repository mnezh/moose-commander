#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#if !defined(__ELKS__)
#include <sys/ioctl.h>
#endif
#if defined(__ELKS__)
#include "elks.h"
#include <sys/stat.h>
#include <sys/mount.h>
#endif
#include <pwd.h>
#include <grp.h>
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
static int request_cursor_report(unsigned char *buf, int size, const char *prefix) {
    int i = 0;
    if (prefix) printf("%s", prefix);
    printf("\033[6n");
    fflush(stdout);
    while (i < size) {
        if (read(STDIN_FILENO, buf + i, 1) != 1) return -1;
        if (buf[i] == 'R') break;
        i++;
    }
    if (i >= size || buf[i] != 'R') return -1;
    buf[i] = '\0';
    return 0;
}

static int parse_cursor_report(const unsigned char *buf, int *row, int *col) {
    int i = 0;
    int r = 0, c = 0;
    while (buf[i] && buf[i] != '[') i++;
    if (!buf[i] || buf[i] != '[') return -1;
    i++;
    for (r = 0; buf[i] >= '0' && buf[i] <= '9'; i++) r = r * 10 + (buf[i] - '0');
    if (buf[i] != ';') return -1;
    i++;
    for (c = 0; buf[i] >= '0' && buf[i] <= '9'; i++) c = c * 10 + (buf[i] - '0');
    *row = r;
    *col = c;
    return 0;
}

static int get_dims_ansi(int *rows, int *cols) {
    unsigned char buf[32];
    int saved_row, saved_col;
    int r, c;

    if (request_cursor_report(buf, (int)sizeof(buf), NULL) != 0) return -1;
    if (parse_cursor_report(buf, &saved_row, &saved_col) != 0) return -1;

    if (request_cursor_report(buf, (int)sizeof(buf), "\033[999;999H") != 0) return -1;
    if (parse_cursor_report(buf, &r, &c) != 0) return -1;
    if (r <= 0 || c <= 0) return -1;

    printf("\033[%d;%dH", saved_row, saved_col);

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

static void print_user_group(void) {
    uid_t uid = 0;
    gid_t gid = 0;
    struct passwd *pw = getpwuid(uid);
    struct group *gr = getgrgid(gid);

    printf("getpwuid(0): %s\n", pw ? pw->pw_name : "(NULL, not supported)");
    printf("getgrgid(0): %s\n", gr ? gr->gr_name : "(NULL, not supported)");
}

#if defined(__ELKS__)
static void report_space(const char *path) {
    struct stat st;
    struct statfs sf;
    unsigned long total_kb, free_kb, used_kb;
    int percentage = 0;

    if (stat(path, &st) < 0) {
        perror("Error accessing path");
        return;
    }
    if (ustatfs(st.st_dev, &sf, 0) < 0) {
        perror("Error getting filesystem stats");
        return;
    }

    total_kb = (unsigned long)(sf.f_blocks * sf.f_bsize) / 1024;
    free_kb  = (unsigned long)(sf.f_bfree * sf.f_bsize) / 1024;
    used_kb  = total_kb - free_kb;
    if (total_kb > 0)
        percentage = (int)((used_kb * 100) / total_kb);

    printf("\nFile Path:   %s\n", path);
    printf("FS Type:     %s (raw 0x%lx)\n", elks_get_fs_type(sf.f_type), (unsigned long)sf.f_type);
    printf("Device:      Major %d, Minor %d\n",
           (int)(st.st_dev >> 8) & 0xFF, (int)(st.st_dev & 0xFF));
    printf("------------------------------\n");
    printf("Total Space: %lu KB\n", total_kb);
    printf("Used Space:  %lu KB (%d%%)\n", used_kb, percentage);
    printf("Free Space:  %lu KB\n", free_kb);
    printf("Inodes:      %ld total / %ld free\n", (long)sf.f_files, (long)sf.f_ffree);
}
#endif

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
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

    printf("\nChecking libc for user/group functions:\n");
    print_user_group();

#if defined(__ELKS__)
    report_space(argc > 1 ? argv[1] : "/");
#endif

    return 0;
}
