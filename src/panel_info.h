#ifndef MC_PANEL_INFO_H
#define MC_PANEL_INFO_H

#include "mcurses.h"
#include "data.h"

/* Left panel width (half screen) */
#define PANEL_WIDTH  (COLS / 2)

/* Draw the Info panel with pre-filled file info (presentation only). */
void panel_info_draw(const struct file_info *info);

#endif /* MC_PANEL_INFO_H */
