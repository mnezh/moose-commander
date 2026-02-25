#include "data.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

#include <pwd.h>
#include <grp.h>
#if !defined(__ELKS__)
#include <sys/statvfs.h>
#endif
#if defined(__APPLE__) || defined(__MACH__)
#include <sys/mount.h>
#endif
#if defined(__linux__) && !defined(__ELKS__)
#include <sys/sysmacros.h>
#endif
#if !defined(major)
#define major(dev)  (((unsigned)(dev) >> 8) & 0xFF)
#define minor(dev)  ((unsigned)(dev) & 0xFF)
#endif
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

static void mode_str(mode_t m, char *buf, size_t size) {
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

/* Use our own date formatter everywhere instead of strftime, so we build on
 * minimal libcs (e.g. ELKS) that don't provide strftime. Output matches "%b %e %H:%M". */
static void format_date(char *buf, size_t size, time_t t) {
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

#if !defined(__ELKS__)
static void human_size(unsigned long long bytes, char *buf, size_t size) {
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
#endif

static const char *basename_of(const char *path) {
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

static void copy_basename(char *dst, size_t size, const char *path) {
    const char *base = basename_of(path);
    snprintf(dst, size, "%s", base);
}

void data_fill_file_info(const char *path, struct file_info *out) {
    memset(out, 0, sizeof(*out));

    struct stat st;
    if (stat(path, &st) != 0) {
        out->not_found = 1;
        copy_basename(out->file_name, sizeof(out->file_name), path);
        snprintf(out->error_msg, sizeof(out->error_msg), "%s", strerror(errno));
        return;
    }

    copy_basename(out->file_name, sizeof(out->file_name), path);
    mode_str(st.st_mode, out->mode, sizeof(out->mode));
    snprintf(out->mode_oct, sizeof(out->mode_oct), "%04o", (unsigned)(st.st_mode & 07777));
    snprintf(out->links, sizeof(out->links), "%lu", (unsigned long)st.st_nlink);

    {
        const char *user = "?";
        const char *grp = "?";
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        if (pw) user = pw->pw_name;
        else { static char u[16]; snprintf(u, sizeof u, "%lu", (unsigned long)st.st_uid); user = u; }
        if (gr) grp = gr->gr_name;
        else { static char g[16]; snprintf(g, sizeof g, "%lu", (unsigned long)st.st_gid); grp = g; }
        snprintf(out->owner, sizeof(out->owner), "%s", user);
        snprintf(out->group, sizeof(out->group), "%s", grp);
    }

#if defined(__ELKS__)
    snprintf(out->size, sizeof(out->size), "%lu", (unsigned long)st.st_size);
#else
    {
        unsigned long blocks = (unsigned long)st.st_blocks;
        if (blocks == 0 && st.st_size > 0)
            blocks = (st.st_size + 511) / 512;
        snprintf(out->size, sizeof(out->size), "%lu (%lu blocks)", (unsigned long)st.st_size, blocks);
    }
#endif

    format_date(out->changed, sizeof(out->changed), st.st_ctime);
    format_date(out->modified, sizeof(out->modified), st.st_mtime);
    format_date(out->accessed, sizeof(out->accessed), st.st_atime);

#if !defined(__ELKS__)
    {
        struct statvfs vfs;
        if (statvfs(path, &vfs) == 0) {
            out->has_fs = 1;
            snprintf(out->filesystem, sizeof(out->filesystem), "/");
            snprintf(out->device, sizeof(out->device), "%lu:%lu", (unsigned long)major(st.st_dev), (unsigned long)minor(st.st_dev));
#if defined(__APPLE__) && defined(__MACH__)
            struct statfs sfs;
            if (statfs(path, &sfs) == 0 && sfs.f_fstypename[0])
                snprintf(out->fstype, sizeof(out->fstype), "%s", sfs.f_fstypename);
            else
                snprintf(out->fstype, sizeof(out->fstype), "-");
#else
            snprintf(out->fstype, sizeof(out->fstype), "-");
#endif
            {
                unsigned long long total = (unsigned long long)vfs.f_blocks * vfs.f_frsize;
                unsigned long long free_bytes = (unsigned long long)vfs.f_bavail * vfs.f_frsize;
                unsigned long pct = total ? (unsigned long)(100ULL * free_bytes / total) : 0;
                char total_s[16], free_s[16];
                human_size(total, total_s, sizeof total_s);
                human_size(free_bytes, free_s, sizeof free_s);
                snprintf(out->total_space, sizeof(out->total_space), "%s", total_s);
                snprintf(out->free_space, sizeof(out->free_space), "%s (%lu%%)", free_s, pct);
            }
            {
                unsigned long long total_nodes = (unsigned long long)vfs.f_files;
                unsigned long long free_nodes = (unsigned long long)vfs.f_ffree;
                unsigned long pct = total_nodes ? (unsigned long)(100ULL * free_nodes / total_nodes) : 0;
                snprintf(out->total_nodes, sizeof(out->total_nodes), "%llu", total_nodes);
                snprintf(out->free_nodes, sizeof(out->free_nodes), "%llu (%lu%%)", free_nodes, pct);
            }
        }
    }
#endif
}
