#if defined(__ELKS__)

#include "elks.h"
#include "data.h"
#include "util.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

/* FST_MINIX, FST_MSDOS, FST_ROMFS from <sys/mount.h> */
const char *elks_get_fs_type(long type) {
    switch (type) {
        case FST_MINIX:  return "minix";
        case FST_MSDOS:  return "msdos";
        case FST_ROMFS:  return "romfs";
        default:         return "Unknown";
    }
}

void elks_fill_file_info_fs(const char *path, struct file_info *out) {
    struct stat st;
    struct statfs sf;

    if (stat(path, &st) != 0)
        return;
    if (ustatfs(st.st_dev, &sf, 0) != 0)
        return;

    out->has_fs = 1;
    snprintf(out->filesystem, sizeof(out->filesystem), "/");
    snprintf(out->device, sizeof(out->device), "%u:%u",
             (unsigned)(st.st_dev >> 8) & 0xFF, (unsigned)(st.st_dev & 0xFF));
    snprintf(out->fstype, sizeof(out->fstype), "%s", elks_get_fs_type(sf.f_type));

    {
        unsigned long total_kb = (unsigned long)(sf.f_blocks * sf.f_bsize) / 1024;
        unsigned long free_kb  = (unsigned long)(sf.f_bfree * sf.f_bsize) / 1024;
        unsigned long long total_bytes = (unsigned long long)total_kb * 1024;
        unsigned long long free_bytes  = (unsigned long long)free_kb * 1024;
        unsigned long pct = total_kb ? (free_kb * 100 / total_kb) : 0;
        char total_s[16], free_s[16];
        util_human_size(total_bytes, total_s, sizeof total_s);
        util_human_size(free_bytes, free_s, sizeof free_s);
        snprintf(out->total_space, sizeof(out->total_space), "%s", total_s);
        snprintf(out->free_space, sizeof(out->free_space), "%s (%lu%%)", free_s, pct);
    }
    {
        long total_nodes = (long)sf.f_files;
        long free_nodes  = (long)sf.f_ffree;
        unsigned long pct = total_nodes ? (unsigned long)(free_nodes * 100 / total_nodes) : 0;
        snprintf(out->total_nodes, sizeof(out->total_nodes), "%ld", total_nodes);
        snprintf(out->free_nodes, sizeof(out->free_nodes), "%ld (%lu%%)", free_nodes, pct);
    }
}

#endif /* __ELKS__ */
