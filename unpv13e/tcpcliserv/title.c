#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

void title(){
    move(0, 21);
    printw("  ____   _                    _               _     \n");
    move(1,21);
    printw(" / ___| | |  __ _  _ __      | |  __ _   ___ | | __ \n");
    move(2,21);
    printw(" \\___ \\ | | / _` || '_ \\  _  | | / _` | / __|| |/ / \n");
    move(3,21);
    printw("  ___) || || (_| || |_) || |_| || (_| || (__ |   <  \n");
    move(4,21);
    printw(" |____/ |_| \\__,_|| .__/  \\___/  \\__,_| \\___||_|\\_\\ \n");
    move(5,21);
    printw("                  |_|                               \n");
    move(0, 0); 
}
void frame(char name[15], int id){
    move(6,0);
    char m[30];
    sprintf(m,"Welcome to SlapJack, %s.",name);
    int start=(53-strlen(m))/2;
    move(6,21);
    printw("|---------------------------------------------------|\n");
    move(7,21);
    printw("|                                                   |\n");
    move(8,21);
    printw("|                                                   |\n");
    move(8,start+21);
    printw("%s",m);;
    move(8,52+21);
    printw("|");
    move(9,21);
    printw("|                  Your ID is %-2d.                   |\n", id);
    move(10,21);
    printw("|                                                   |\n");
    move(11,21);
    printw("|---------------------------------------------------|\n");
}

int main(void) {
    initscr();             // Initialize ncurses
    cbreak();              // Line buffering disabled, pass characters immediately
    noecho();              // Don't echo characters to the screen
    keypad(stdscr, TRUE);  // Enable the keypad for special keys
    start_color();
    char name[15]="Testttttttttttt";
    int id=1;
    while(1){
        move(0, 0);  // move the cursor to the beginning of the line
        title();
        frame(name, id);
        refresh();
    }

    endwin();
    return 0;
}
