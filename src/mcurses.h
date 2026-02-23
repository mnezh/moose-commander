#ifndef MC_MCURSES_H
#define MC_MCURSES_H

/* ncurses-like API (only what main uses) */

#define ERR  (-1)

/* Screen dimensions (typical ELKS/CGA) */
#define LINES  25
#define COLS   80

/* Lifecycle */
void initscr(void);
void endwin(void);
void cbreak(void);
void noecho(void);

/* Output */
void clear(void);
int mvprintw(int row, int col, const char *fmt, ...);
void refresh(void);

/* Input */
int getch(void);

#endif /* MC_MCURSES_H */
