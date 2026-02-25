#ifndef MC_ELKS_H
#define MC_ELKS_H

#if defined(__ELKS__)

struct file_info;

/* Fill filesystem-related fields of *out for path (stat + ustatfs, no /dev scan). */
void elks_fill_file_info_fs(const char *path, struct file_info *out);

/* FST_* type to string (for reporting). */
const char *elks_get_fs_type(long type);

#endif /* __ELKS__ */

#endif /* MC_ELKS_H */
