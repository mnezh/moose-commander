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

/* Box-drawing (UTF-8) */
#define BOX_TL  "\xe2\x94\x8c"  /* ┌ */
#define BOX_TR  "\xe2\x94\x90"  /* ┐ */
#define BOX_BL  "\xe2\x94\x94"  /* └ */
#define BOX_BR  "\xe2\x94\x98"  /* ┘ */
#define BOX_V   "\xe2\x94\x82"  /* │ */
#define BOX_H   "\xe2\x94\x80"  /* ─ */
#define BOX_TL_SEP "\xe2\x94\x9c"  /* ├ */
#define BOX_TR_SEP "\xe2\x94\xa4"  /* ┤ */

/* Color palette (ANSI) */
#define MC_COLOR_BG         "\033[44m"   /* Background Blue #0000AA */
#define MC_COLOR_TEXT       "\033[37m"   /* Text (General) #AAAAAA */
#define MC_COLOR_ACTIVE_SEL "\033[33m"   /* Active Selection #FFFF55 (no bold) */
#define MC_COLOR_SEL_BAR    "\033[46m"   /* Selection Bar (bg) #00AAAA */
#define MC_COLOR_HEADER     "\033[37m"   /* Headers/Frame #FFFFFF (no bold) */
#define MC_COLOR_SHADOW     "\033[30m"   /* Shadows #000000 */

#endif /* MC_ANSI_H */
