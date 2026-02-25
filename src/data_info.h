#ifndef MC_DATA_INFO_H
#define MC_DATA_INFO_H

#include <sys/types.h>
#include <time.h>

#define FILE_INFO_NAME_LEN   256
#define FILE_INFO_LINE_LEN   64

/* ----- Raw data (retrieval only; no human-readable formatting) ----- */

typedef struct raw_file_data {
    int  not_found;
    char error_msg[FILE_INFO_LINE_LEN];
    char file_name[FILE_INFO_NAME_LEN];

    mode_t st_mode;
    unsigned long st_nlink;
    char owner[FILE_INFO_LINE_LEN];
    char group[FILE_INFO_LINE_LEN];
    unsigned long st_size;
    time_t ctime;
    time_t mtime;
    time_t atime;
    dev_t st_dev;
} raw_file_data_t;

typedef struct raw_fs_data {
    int has_fs;
    char filesystem[FILE_INFO_LINE_LEN];
    char device[FILE_INFO_LINE_LEN];
    char fstype[32];
    unsigned long long total_bytes;
    unsigned long long free_bytes;
    unsigned long long total_nodes;
    unsigned long long free_nodes;
} raw_fs_data_t;

/* Fill *out_file and *out_fs with raw system data for path. */
void fetch_file_info(const char *path, raw_file_data_t *out_file, raw_fs_data_t *out_fs);

/* ----- Display data (file-specific and filesystem-specific blocks) ----- */

struct file_info_file {
    char file_name[FILE_INFO_NAME_LEN];
    char mode[16];
    char mode_oct[8];
    char links[FILE_INFO_LINE_LEN];
    char owner[FILE_INFO_LINE_LEN];
    char group[FILE_INFO_LINE_LEN];
    char size[FILE_INFO_LINE_LEN];
    char changed[32];
    char modified[32];
    char accessed[32];
};

struct file_info_fs {
    int  has_fs;
    char filesystem[FILE_INFO_LINE_LEN];
    char device[FILE_INFO_LINE_LEN];
    char fstype[32];
    char total_space[FILE_INFO_LINE_LEN];
    char free_space[FILE_INFO_LINE_LEN];
    char total_nodes[FILE_INFO_LINE_LEN];
    char free_nodes[FILE_INFO_LINE_LEN];
};

struct file_info {
    int  not_found;
    char error_msg[FILE_INFO_LINE_LEN];
    struct file_info_file file;
    struct file_info_fs fs;
};

/* Fetch raw data for path and fill *out with human-readable file info. */
void fetch_translated_file_info(const char *path, struct file_info *out);

#endif /* MC_DATA_INFO_H */
