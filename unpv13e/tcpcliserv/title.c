#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

void title(){
    move(0, 0);
    printw("  ____   _                    _               _     \n");
    printw(" / ___| | |  __ _  _ __      | |  __ _   ___ | | __ \n");
    printw(" \\___ \\ | | / _` || '_ \\  _  | | / _` | / __|| |/ / \n");
    printw("  ___) || || (_| || |_) || |_| || (_| || (__ |   <  \n");
    printw(" |____/ |_| \\__,_|| .__/  \\___/  \\__,_| \\___||_|\\_\\ \n");
    printw("                  |_|\n");
    move(0, 0); 
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
