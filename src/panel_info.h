#ifndef MC_PANEL_INFO_H
#define MC_PANEL_INFO_H

#include "mcurses.h"
#include "data_info.h"

/* Panel width (half screen) */
#define PANEL_WIDTH  (COLS / 2)

/* Panel side: pass to panel_info_draw */
#define PANEL_LEFT   0
#define PANEL_RIGHT  1

/* Draw the Info panel with pre-filled file info (side = PANEL_LEFT or PANEL_RIGHT). */
void panel_info_draw(const struct file_info *info, int side);

#endif /* MC_PANEL_INFO_H */
