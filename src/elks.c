#if defined(__ELKS__)

#include "elks.h"
#include <sys/types.h>
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

#endif /* __ELKS__ */
