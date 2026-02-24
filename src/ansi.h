#ifndef MC_ANSI_H
#define MC_ANSI_H

/* Control Sequence Introducer */
#define ANSI_CSI  "\033["

/* Display */
#define ANSI_CLS          ANSI_CSI "2J"
#define ANSI_RESET        ANSI_CSI "0m"

/* Cursor position: printf(ANSI_CUP_FMT, row, col) */
#define ANSI_CUP_FMT      ANSI_CSI "%d;%dH"

/* Cursor visibility */
#define ANSI_HIDE_CURSOR  "\033[?25l"
#define ANSI_SHOW_CURSOR  "\033[?25h"

/* Alternate screen buffer (save/restore visible screen) */
#define ANSI_ALT_BUF_ON   "\033[?1049h"
#define ANSI_ALT_BUF_OFF  "\033[?1049l"

/* Box-drawing light (single line), names from Unicode / ASCII-table style */
#define BOX_LIGHT_DOWN_AND_RIGHT   "\xe2\x94\x8c"  /* ┌ */
#define BOX_LIGHT_DOWN_AND_LEFT    "\xe2\x94\x90"  /* ┐ */
#define BOX_LIGHT_UP_AND_RIGHT     "\xe2\x94\x94"  /* └ */
#define BOX_LIGHT_UP_AND_LEFT      "\xe2\x94\x98"  /* ┘ */
#define BOX_LIGHT_VERTICAL         "\xe2\x94\x82"  /* │ */
#define BOX_LIGHT_HORIZONTAL       "\xe2\x94\x80"  /* ─ */
#define BOX_LIGHT_VERTICAL_AND_RIGHT "\xe2\x94\x9c"  /* ├ */
#define BOX_LIGHT_VERTICAL_AND_LEFT  "\xe2\x94\xa4"  /* ┤ */

/* Box-drawing double line */
#define BOX_DOUBLE_DOWN_AND_RIGHT  "\xe2\x95\x94"  /* ╔ */
#define BOX_DOUBLE_DOWN_AND_LEFT   "\xe2\x95\x97"  /* ╗ */
#define BOX_DOUBLE_UP_AND_RIGHT    "\xe2\x95\x9a"  /* ╚ */
#define BOX_DOUBLE_UP_AND_LEFT     "\xe2\x95\x9d"  /* ╝ */
#define BOX_DOUBLE_VERTICAL        "\xe2\x95\x91"  /* ║ */
#define BOX_DOUBLE_HORIZONTAL      "\xe2\x95\x90"  /* ═ */

/* Separator crossing: vertical single and right/left double (╟ ╢) */
#define BOX_VERTICAL_SINGLE_AND_RIGHT_DOUBLE "\xe2\x95\x9f"  /* ╟ */
#define BOX_VERTICAL_SINGLE_AND_LEFT_DOUBLE  "\xe2\x95\xa2"  /* ╢ */

/* Color palette (ANSI) */
#define MC_COLOR_BG         "\033[44m"   /* Background Blue #0000AA */
#define MC_COLOR_TEXT       "\033[37m"   /* Text (General) #AAAAAA */
#define MC_COLOR_ACTIVE_SEL "\033[33m"   /* Active Selection #FFFF55 (no bold) */
#define MC_COLOR_SEL_BAR    "\033[46m"   /* Selection Bar (bg) #00AAAA */
#define MC_COLOR_HEADER     "\033[37m"   /* Headers/Frame #FFFFFF (no bold) */
#define MC_COLOR_SHADOW     "\033[30m"   /* Shadows #000000 */

#endif /* MC_ANSI_H */
