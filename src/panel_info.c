#include "panel_info.h"
#include "version.h"
#include "ansi.h"
#include <stdio.h>
#include <string.h>

#define INNER_W       (PANEL_WIDTH - 2)
#define LABEL_WIDTH   12   /* "  " + label padded to this = aligned values */

/* Fill row from col_start to col_end (panel-relative) with line character. */
static void fill_hline(int row, int col_start, int col_end, int col_off, const char *line_ch) {
    int c;
    for (c = col_start; c <= col_end; c++)
        mvprintw(row, col_off + c, line_ch);
}

/* Draw line with double vertical, left-aligned content, double vertical. */
static void draw_data_line(int row, const char *content, int col_off) {
    mvprintw(row, col_off + 0, BOX_DOUBLE_VERTICAL);
    mvprintw(row, col_off + 1, "%-*.*s", INNER_W, INNER_W, content);
    mvprintw(row, col_off + PANEL_WIDTH - 1, BOX_DOUBLE_VERTICAL);
}

void panel_info_draw(const struct file_info *info, int side) {
    int col_off = (side == PANEL_RIGHT) ? PANEL_WIDTH : 0;
    int w = PANEL_WIDTH;
    int h = LINES;
    int row = 0;
    char buf[128];

    /* Default: blue background, general text color for all panel content */
    printf(MC_COLOR_BG);
    printf(MC_COLOR_TEXT);

    /* 1. Top: double line, " Information " centered */
    mvprintw(row, col_off + 0, BOX_DOUBLE_DOWN_AND_RIGHT);
    {
        const char *title = " Information ";
        int len = (int)strlen(title);
        int left_pad = (INNER_W - len) / 2;
        if (left_pad > 0)
            fill_hline(row, 1, 1 + left_pad - 1, col_off, BOX_DOUBLE_HORIZONTAL);
        mvprintw(row, col_off + 1 + left_pad, "%s", title);
        if (1 + left_pad + len < w - 1)
            fill_hline(row, 1 + left_pad + len, w - 2, col_off, BOX_DOUBLE_HORIZONTAL);
    }
    mvprintw(row, col_off + w - 1, BOX_DOUBLE_DOWN_AND_LEFT);
    row++;

    /* 2. Version line: double vertical */
    {
        int len, left_pad, c;
        snprintf(buf, sizeof buf, "Moose Commander %s", MC_VERSION);
        len = (int)strlen(buf);
        left_pad = (INNER_W - len) >= 0 ? (INNER_W - len) / 2 : 0;
        mvprintw(row, col_off + 0, BOX_DOUBLE_VERTICAL);
        for (c = 1; c < 1 + left_pad; c++)
            mvprintw(row, col_off + c, " ");
        mvprintw(row, col_off + 1 + left_pad, "%s%s%s", MC_COLOR_ACTIVE_SEL, buf, MC_COLOR_TEXT);
        for (c = 1 + left_pad + len; c < w - 1; c++)
            mvprintw(row, col_off + c, " ");
        mvprintw(row, col_off + w - 1, BOX_DOUBLE_VERTICAL);
        row++;
    }

    /* 3. Separator: vertical single and right/left double (╟ ╢), light horizontal */
    mvprintw(row, col_off + 0, BOX_VERTICAL_SINGLE_AND_RIGHT_DOUBLE);
    fill_hline(row, 1, w - 2, col_off, BOX_LIGHT_HORIZONTAL);
    mvprintw(row, col_off + w - 1, BOX_VERTICAL_SINGLE_AND_LEFT_DOUBLE);
    row++;

    /* 4. Data lines: first "File:" with one space indent; rest with two spaces */
    if (info->not_found) {
        snprintf(buf, sizeof buf, " File: %s", info->file.file_name);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  (not found: %s)", info->error_msg);
        draw_data_line(row++, buf, col_off);
    } else {
        snprintf(buf, sizeof buf, " File: %s", info->file.file_name);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  %-*s %s (%s)", LABEL_WIDTH, "Mode:", info->file.mode, info->file.mode_oct);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Links:", info->file.links);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Owner:", info->file.owner);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Group:", info->file.group);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Size:", info->file.size);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Changed:", info->file.changed);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Modified:", info->file.modified);
        draw_data_line(row++, buf, col_off);
        snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Accessed:", info->file.accessed);
        draw_data_line(row++, buf, col_off);
        if (info->fs.has_fs) {
            snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Filesystem:", info->fs.filesystem);
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Device:", info->fs.device);
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Type:", info->fs.fstype);
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Total space:", info->fs.total_space);
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Free space:", info->fs.free_space);
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Total nodes:", info->fs.total_nodes);
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s %s", LABEL_WIDTH, "Free nodes:", info->fs.free_nodes);
            draw_data_line(row++, buf, col_off);
        } else {
            snprintf(buf, sizeof buf, "  %-*s -", LABEL_WIDTH, "Filesystem:");
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s -", LABEL_WIDTH, "Device:");
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s -", LABEL_WIDTH, "Type:");
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s -", LABEL_WIDTH, "Total space:");
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s -", LABEL_WIDTH, "Free space:");
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s -", LABEL_WIDTH, "Total nodes:");
            draw_data_line(row++, buf, col_off);
            snprintf(buf, sizeof buf, "  %-*s -", LABEL_WIDTH, "Free nodes:");
            draw_data_line(row++, buf, col_off);
        }
    }

    /* Empty lines with double vertical until bottom */
    for ( ; row < h - 1; row++)
        draw_data_line(row, "", col_off);

    /* 5. Bottom: double line */
    mvprintw(h - 1, col_off + 0, BOX_DOUBLE_UP_AND_RIGHT);
    fill_hline(h - 1, 1, w - 2, col_off, BOX_DOUBLE_HORIZONTAL);
    mvprintw(h - 1, col_off + w - 1, BOX_DOUBLE_UP_AND_LEFT);
}
