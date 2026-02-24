#include "mcurses.h"
#include "keyboard.h"
#include "data.h"
#include "panel_info.h"

int main(int argc, char **argv) {
    struct file_info info;
    const char *path = (argc > 0 && argv[0] && argv[0][0]) ? argv[0] : "mc";

    initscr();

    clear();
    data_fill_file_info(path, &info);
    panel_info_draw(&info, PANEL_LEFT);
    panel_info_draw(&info, PANEL_RIGHT);
    refresh();

    while (getch() != KEY_ESC)
        ;

    endwin();
    return 0;
}
