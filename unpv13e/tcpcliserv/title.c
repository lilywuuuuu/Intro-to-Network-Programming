#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

void title(){
    move(0, 0);
    printw("  ____   _\n");
    printw(" / ___| | |\n");
    printw("| (__   | |  ___\n");
    printw(" \\__ \\  | | |__ \\\n");
    printw(" ___) | | |  _)  |\n");
    printw("|     | | | (  O |\n");
    printw("|____/  |_| \\___/\n");
    
}

int main(void) {
    initscr();             // Initialize ncurses
    cbreak();              // Line buffering disabled, pass characters immediately
    noecho();              // Don't echo characters to the screen
    keypad(stdscr, TRUE);  // Enable the keypad for special keys
    start_color();
    init_pair(1,COLOR_RED,COLOR_RED);
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
    init_pair(3,COLOR_BLACK,COLOR_BLACK);
    
    while(1){
        move(0, 0);  // move the cursor to the beginning of the line
        title();
        refresh();
    }

    endwin();
    return 0;
}
