#include "panel_info.h"
#include "version.h"
#include "ansi.h"
#include <stdio.h>
#include <string.h>

#define INNER_W  (PANEL_WIDTH - 2)

/* Fill row from col 1 to w-2 with BOX_H. */
static void fill_hline(int row, int col_start, int col_end) {
    int c;
    for (c = col_start; c <= col_end; c++)
        mvprintw(row, c, BOX_H);
}

/* Draw line with BOX_V, left-aligned content (padded/truncated to inner width), BOX_V. */
static void draw_data_line(int row, const char *content) {
    mvprintw(row, 0, BOX_V);
    mvprintw(row, 1, "%-*.*s", INNER_W, INNER_W, content);
    mvprintw(row, PANEL_WIDTH - 1, BOX_V);
}

void panel_info_draw(const struct file_info *info) {
    int w = PANEL_WIDTH;
    int h = LINES;
    int row = 0;
    char buf[128];

    /* Default: blue background, general text color for all panel content */
    printf(MC_COLOR_BG);
    printf(MC_COLOR_TEXT);

    /* 1. Top: BOX_TL, " Information " centered, BOX_TR */
    mvprintw(row, 0, BOX_TL);
    {
        const char *title = " Information ";
        int len = (int)strlen(title);
        int left_pad = (INNER_W - len) / 2;
        if (left_pad > 0)
            fill_hline(row, 1, 1 + left_pad - 1);
        mvprintw(row, 1 + left_pad, "%s", title);
        if (1 + left_pad + len < w - 1)
            fill_hline(row, 1 + left_pad + len, w - 2);
    }
    mvprintw(row, w - 1, BOX_TR);
    row++;

    /* 2. BOX_V, "Moose Commander <version>" centered (Active Selection color), BOX_V */
    {
        int len, left_pad, c;
        snprintf(buf, sizeof buf, "Moose Commander %s", MC_VERSION);
        len = (int)strlen(buf);
        left_pad = (INNER_W - len) >= 0 ? (INNER_W - len) / 2 : 0;
        mvprintw(row, 0, BOX_V);
        for (c = 1; c < 1 + left_pad; c++)
            mvprintw(row, c, " ");
        mvprintw(row, 1 + left_pad, "%s%s%s", MC_COLOR_ACTIVE_SEL, buf, MC_COLOR_TEXT);
        for (c = 1 + left_pad + len; c < w - 1; c++)
            mvprintw(row, c, " ");
        mvprintw(row, w - 1, BOX_V);
        row++;
    }

    /* 3. Separator: BOX_TL_SEP, BOX_H, BOX_TR_SEP */
    mvprintw(row, 0, BOX_TL_SEP);
    fill_hline(row, 1, w - 2);
    mvprintw(row, w - 1, BOX_TR_SEP);
    row++;

    /* 4. Data lines: first "File:" with one space indent; rest with two spaces */
    if (info->not_found) {
        snprintf(buf, sizeof buf, " File: %s", info->file_name);
        draw_data_line(row++, buf);
        snprintf(buf, sizeof buf, "  (not found: %s)", info->error_msg);
        draw_data_line(row++, buf);
    } else {
        snprintf(buf, sizeof buf, " File: %s", info->file_name);
        draw_data_line(row++, buf);
        snprintf(buf, sizeof buf, "  Mode:       %s (%s)", info->mode, info->mode_oct);
        draw_data_line(row++, buf);
        snprintf(buf, sizeof buf, "  Links:      %s", info->links);
        draw_data_line(row++, buf);
        snprintf(buf, sizeof buf, "  Owner:      %s", info->owner);
        draw_data_line(row++, buf);
        snprintf(buf, sizeof buf, "  Size:       %s", info->size);
        draw_data_line(row++, buf);
        snprintf(buf, sizeof buf, "  Changed:    %s", info->changed);
        draw_data_line(row++, buf);
        snprintf(buf, sizeof buf, "  Modified:   %s", info->modified);
        draw_data_line(row++, buf);
        snprintf(buf, sizeof buf, "  Accessed:   %s", info->accessed);
        draw_data_line(row++, buf);
        if (info->has_fs) {
            snprintf(buf, sizeof buf, "  Filesystem: %s", info->filesystem);
            draw_data_line(row++, buf);
            snprintf(buf, sizeof buf, "  Device:     %s", info->device);
            draw_data_line(row++, buf);
            snprintf(buf, sizeof buf, "  Type:       %s", info->fstype);
            draw_data_line(row++, buf);
            snprintf(buf, sizeof buf, "  Free space: %s", info->free_space);
            draw_data_line(row++, buf);
            snprintf(buf, sizeof buf, "  Free nodes: %s", info->free_nodes);
            draw_data_line(row++, buf);
        } else {
            draw_data_line(row++, "  Filesystem: -");
            draw_data_line(row++, "  Device:     -");
            draw_data_line(row++, "  Type:       -");
            draw_data_line(row++, "  Free space: -");
            draw_data_line(row++, "  Free nodes: -");
        }
    }

    /* Empty lines with BOX_V ... BOX_V until bottom */
    for ( ; row < h - 1; row++)
        draw_data_line(row, "");

    /* 5. Bottom: BOX_BL, BOX_H, BOX_BR */
    mvprintw(h - 1, 0, BOX_BL);
    fill_hline(h - 1, 1, w - 2);
    mvprintw(h - 1, w - 1, BOX_BR);
}
