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

#endif /* MC_ANSI_H */
