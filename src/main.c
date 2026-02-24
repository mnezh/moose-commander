#include "mcurses.h"
#include "keyboard.h"
#include "data.h"
#include "panel_info.h"

int main(void) {
    struct file_info info;

    initscr();

    clear();
    data_fill_file_info("build/mc", &info);
    panel_info_draw(&info, PANEL_LEFT);
    panel_info_draw(&info, PANEL_RIGHT);
    refresh();

    while (getch() != KEY_ESC)
        ;

    endwin();
    return 0;
}
