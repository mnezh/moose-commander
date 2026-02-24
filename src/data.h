#ifndef MC_DATA_H
#define MC_DATA_H

#define FILE_INFO_NAME_LEN   256
#define FILE_INFO_LINE_LEN   64

struct file_info {
    int  not_found;
    char error_msg[FILE_INFO_LINE_LEN];

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

    int  has_fs;
    char filesystem[FILE_INFO_LINE_LEN];
    char device[FILE_INFO_LINE_LEN];
    char fstype[32];
    char total_space[FILE_INFO_LINE_LEN];
    char free_space[FILE_INFO_LINE_LEN];
    char total_nodes[FILE_INFO_LINE_LEN];
    char free_nodes[FILE_INFO_LINE_LEN];
};

/* Fill *out with file and (if available) filesystem info for path. */
void data_fill_file_info(const char *path, struct file_info *out);

#endif /* MC_DATA_H */
