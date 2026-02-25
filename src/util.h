#ifndef MC_UTIL_H
#define MC_UTIL_H

#include <stddef.h>
#include <sys/stat.h>
#include <time.h>

/* Format byte count as human-readable string (e.g. "1G", "512K") into buf. */
void util_human_size(unsigned long long bytes, char *buf, size_t size);

void mode_str(mode_t m, char *buf, size_t size);
void format_date(char *buf, size_t size, time_t t);
const char *basename_of(const char *path);
void copy_basename(char *dst, size_t size, const char *path);

#endif /* MC_UTIL_H */
