#include "data_info.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#if defined(__ELKS__)
#include "elks.h"
#include <sys/mount.h>
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

void fetch_file_info(const char *path, raw_file_data_t *out_file, raw_fs_data_t *out_fs) {
    memset(out_file, 0, sizeof(*out_file));
    memset(out_fs, 0, sizeof(*out_fs));

    struct stat st;
    if (stat(path, &st) != 0) {
        out_file->not_found = 1;
        copy_basename(out_file->file_name, sizeof(out_file->file_name), path);
        snprintf(out_file->error_msg, sizeof(out_file->error_msg), "%s", strerror(errno));
        return;
    }

    copy_basename(out_file->file_name, sizeof(out_file->file_name), path);
    out_file->st_mode = st.st_mode;
    out_file->st_nlink = (unsigned long)st.st_nlink;
    {
        const char *user = "?";
        const char *grp = "?";
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        if (pw) user = pw->pw_name;
        else { static char u[16]; snprintf(u, sizeof u, "%lu", (unsigned long)st.st_uid); user = u; }
        if (gr) grp = gr->gr_name;
        else { static char g[16]; snprintf(g, sizeof g, "%lu", (unsigned long)st.st_gid); grp = g; }
        snprintf(out_file->owner, sizeof(out_file->owner), "%s", user);
        snprintf(out_file->group, sizeof(out_file->group), "%s", grp);
    }
    out_file->st_size = (unsigned long)st.st_size;
    out_file->ctime = st.st_ctime;
    out_file->mtime = st.st_mtime;
    out_file->atime = st.st_atime;
    out_file->st_dev = st.st_dev;

#if defined(__ELKS__)
    {
        struct statfs sf;
        if (ustatfs(st.st_dev, &sf, 0) == 0) {
            out_fs->has_fs = 1;
            snprintf(out_fs->filesystem, sizeof(out_fs->filesystem), "/");
            snprintf(out_fs->device, sizeof(out_fs->device), "%u:%u",
                     (unsigned)(st.st_dev >> 8) & 0xFF, (unsigned)(st.st_dev & 0xFF));
            snprintf(out_fs->fstype, sizeof(out_fs->fstype), "%s", elks_get_fs_type(sf.f_type));
            out_fs->total_bytes = (unsigned long long)(sf.f_blocks * sf.f_bsize);
            out_fs->free_bytes  = (unsigned long long)(sf.f_bfree * sf.f_bsize);
            out_fs->total_nodes = (unsigned long long)sf.f_files;
            out_fs->free_nodes  = (unsigned long long)sf.f_ffree;
        }
    }
#else
    {
        struct statvfs vfs;
        if (statvfs(path, &vfs) == 0) {
            out_fs->has_fs = 1;
            snprintf(out_fs->filesystem, sizeof(out_fs->filesystem), "/");
            snprintf(out_fs->device, sizeof(out_fs->device), "%lu:%lu",
                     (unsigned long)major(st.st_dev), (unsigned long)minor(st.st_dev));
#if defined(__APPLE__) && defined(__MACH__)
            struct statfs sfs;
            if (statfs(path, &sfs) == 0 && sfs.f_fstypename[0])
                snprintf(out_fs->fstype, sizeof(out_fs->fstype), "%s", sfs.f_fstypename);
            else
                snprintf(out_fs->fstype, sizeof(out_fs->fstype), "-");
#else
            snprintf(out_fs->fstype, sizeof(out_fs->fstype), "-");
#endif
            out_fs->total_bytes = (unsigned long long)vfs.f_blocks * vfs.f_frsize;
            out_fs->free_bytes  = (unsigned long long)vfs.f_bavail * vfs.f_frsize;
            out_fs->total_nodes = (unsigned long long)vfs.f_files;
            out_fs->free_nodes  = (unsigned long long)vfs.f_ffree;
        }
    }
#endif
}

static void translate_file_info(const raw_file_data_t *raw_file, const raw_fs_data_t *raw_fs,
                                struct file_info *out) {
    memset(out, 0, sizeof(*out));
    out->not_found = raw_file->not_found;
    snprintf(out->error_msg, sizeof(out->error_msg), "%s", raw_file->error_msg);
    snprintf(out->file.file_name, sizeof(out->file.file_name), "%s", raw_file->file_name);

    if (raw_file->not_found)
        return;

    mode_str(raw_file->st_mode, out->file.mode, sizeof(out->file.mode));
    snprintf(out->file.mode_oct, sizeof(out->file.mode_oct), "%04o",
             (unsigned)(raw_file->st_mode & 07777));
    snprintf(out->file.links, sizeof(out->file.links), "%lu", raw_file->st_nlink);
    snprintf(out->file.owner, sizeof(out->file.owner), "%s", raw_file->owner);
    snprintf(out->file.group, sizeof(out->file.group), "%s", raw_file->group);

    snprintf(out->file.size, sizeof(out->file.size), "%lu", raw_file->st_size);

    format_date(out->file.changed, sizeof(out->file.changed), raw_file->ctime);
    format_date(out->file.modified, sizeof(out->file.modified), raw_file->mtime);
    format_date(out->file.accessed, sizeof(out->file.accessed), raw_file->atime);

    out->fs.has_fs = raw_fs->has_fs;
    if (!raw_fs->has_fs)
        return;

    snprintf(out->fs.filesystem, sizeof(out->fs.filesystem), "%s", raw_fs->filesystem);
    snprintf(out->fs.device, sizeof(out->fs.device), "%s", raw_fs->device);
    snprintf(out->fs.fstype, sizeof(out->fs.fstype), "%s", raw_fs->fstype);

    {
        char total_s[16], free_s[16];
        util_human_size(raw_fs->total_bytes, total_s, sizeof total_s);
        util_human_size(raw_fs->free_bytes, free_s, sizeof free_s);
        unsigned long pct = raw_fs->total_bytes
            ? (unsigned long)(100ULL * raw_fs->free_bytes / raw_fs->total_bytes) : 0;
        snprintf(out->fs.total_space, sizeof(out->fs.total_space), "%s", total_s);
        snprintf(out->fs.free_space, sizeof(out->fs.free_space), "%s (%lu%%)", free_s, pct);
    }
    {
        unsigned long pct = raw_fs->total_nodes
            ? (unsigned long)(100ULL * raw_fs->free_nodes / raw_fs->total_nodes) : 0;
        snprintf(out->fs.total_nodes, sizeof(out->fs.total_nodes), "%llu", raw_fs->total_nodes);
        snprintf(out->fs.free_nodes, sizeof(out->fs.free_nodes), "%llu (%lu%%)",
                 raw_fs->free_nodes, pct);
    }
}

void fetch_translated_file_info(const char *path, struct file_info *out) {
    raw_file_data_t raw_file;
    raw_fs_data_t raw_fs;
    fetch_file_info(path, &raw_file, &raw_fs);
    translate_file_info(&raw_file, &raw_fs, out);
}
