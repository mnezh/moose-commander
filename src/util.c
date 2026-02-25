#include "util.h"
#include <stdio.h>
#include <string.h>

#ifndef S_ISLNK
#define S_ISLNK(m)  0
#endif
#ifndef S_ISCHR
#define S_ISCHR(m)  0
#endif
#ifndef S_ISBLK
#define S_ISBLK(m)  0
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(m) 0
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(m) 0
#endif

void mode_str(mode_t m, char *buf, size_t size) {
    char *p = buf;
    if (size < 11) return;
    *p++ = S_ISREG(m) ? '-' : S_ISDIR(m) ? 'd' : S_ISLNK(m) ? 'l' : S_ISCHR(m) ? 'c' : S_ISBLK(m) ? 'b' : S_ISFIFO(m) ? 'p' : S_ISSOCK(m) ? 's' : '?';
    *p++ = (m & S_IRUSR) ? 'r' : '-';
    *p++ = (m & S_IWUSR) ? 'w' : '-';
    *p++ = (m & S_IXUSR) ? 'x' : '-';
    *p++ = (m & S_IRGRP) ? 'r' : '-';
    *p++ = (m & S_IWGRP) ? 'w' : '-';
    *p++ = (m & S_IXGRP) ? 'x' : '-';
    *p++ = (m & S_IROTH) ? 'r' : '-';
    *p++ = (m & S_IWOTH) ? 'w' : '-';
    *p++ = (m & S_IXOTH) ? 'x' : '-';
    *p = '\0';
}

/* Output matches "%b %e %H:%M"; works on minimal libcs (e.g. ELKS) without strftime. */
void format_date(char *buf, size_t size, time_t t) {
    struct tm *tm = localtime(&t);
    if (!tm) {
        snprintf(buf, size, "?");
        return;
    }
    static const char *const mon[] = { "Jan","Feb","Mar","Apr","May","Jun",
                                       "Jul","Aug","Sep","Oct","Nov","Dec" };
    int m = (unsigned)tm->tm_mon < 12u ? (int)tm->tm_mon : 0;
    snprintf(buf, size, "%s %2d %02d:%02d",
             mon[m], tm->tm_mday, tm->tm_hour, tm->tm_min);
}

const char *basename_of(const char *path) {
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

void copy_basename(char *dst, size_t size, const char *path) {
    const char *base = basename_of(path);
    snprintf(dst, size, "%s", base);
}

void util_human_size(unsigned long long bytes, char *buf, size_t size) {
    if (bytes >= 1024ULL * 1024 * 1024 * 1024)
        snprintf(buf, size, "%lluT", bytes / (1024ULL * 1024 * 1024 * 1024));
    else if (bytes >= 1024ULL * 1024 * 1024)
        snprintf(buf, size, "%lluG", bytes / (1024ULL * 1024 * 1024));
    else if (bytes >= 1024ULL * 1024)
        snprintf(buf, size, "%lluM", bytes / (1024ULL * 1024));
    else if (bytes >= 1024)
        snprintf(buf, size, "%lluK", bytes / 1024);
    else
        snprintf(buf, size, "%llu", bytes);
}
