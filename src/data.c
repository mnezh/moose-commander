#include "data.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

#include <pwd.h>
#include <grp.h>
#if defined(__ELKS__)
#include "elks.h"
#endif
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

#if defined(__ELKS__)
    elks_fill_file_info_fs(path, out);
#else
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
                util_human_size(total, total_s, sizeof total_s);
                util_human_size(free_bytes, free_s, sizeof free_s);
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
