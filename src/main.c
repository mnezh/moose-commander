#include "mcurses.h"
#include "keyboard.h"
#include <string.h>

int main(void) {
    const char *text = "Hello, Moose!";
    const char *prompt = "[ Press ESC ]";

    initscr();
    cbreak();
    noecho();

    clear();
    mvprintw(LINES / 2, (COLS - (int)strlen(text)) / 2, "%s", text);
    mvprintw(LINES / 2 + 2, (COLS - (int)strlen(prompt)) / 2, "%s", prompt);
    refresh();

    while (getch() != KEY_ESC)
        ;

    endwin();
    return 0;
}
