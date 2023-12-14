#include <stdio.h>
#include <ncurses.h>
#include <string.h>

int main(void){
    initscr(); // Initialize ncurses
    cbreak(); // Line buffering disabled, pass characters immediately
    noecho(); // Don't echo characters to the screen
    keypad(stdscr, TRUE); // Enable the keypad for special keys

    char ch;
    while (1) { // Exit loop on 'q' keypress
        move(0, 0); // move the cursor to the beginning of the line
        int score = 8;
        char name[10] = "lily";
        printw("===============================================================================\n");
        printw("|-------------------------|\n");
        printw("|\tScore Board\t  |\n");
        printw("|\t\t\t  |\n");
        printw("| Score   (ID)Name\t  |\n");
        printw("| %d      |\n");
        printw("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        printw("===============================================================================\n");
        ch = getch();
        if (ch == 'q') {
            printw("bye!\n");
            break;
        }
        else if (ch == '\n') printw("you pressed enter!\n");
        else if (ch == ' ') printw("you pressed space!\n");
        else printw("you pressed %c!\n", ch);
        refresh();
    }
    endwin();
    return 0; 
}