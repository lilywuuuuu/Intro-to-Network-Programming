#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

void card(){
    move(3,71);
    printw("|------|");
    move(4,71);
    printw("|      |");
    move(5,71);
    printw("|      |");
    move(6,71);
    printw("|      |");
    move(7,71);
    printw("|      |");
    move(8,71);
    printw("|------|");
    return;
}
void line(){
    printw("=============================================================================================\n");
}

void scoreboard(int score[5], int id[5], char name[5][15]){
    line();
    printw("|---------------------------|\n");
    printw("|\t Score Board\t    |\n");
    printw("|\t\t\t    |\n");
    printw("| Score   Name           ID |\n");
    for (int i = 0; i < 4; i++){
        printw("|  %-2d     %-15s%-2d |\n", score[i], name[i], id[i]);
    }
    printw("|\t\t\t    |\n");
    printw("|---------------------------|\n");
    printw("\n\n\n\n\n\n\n");
    line();
}

int main(void) {
    initscr();             // Initialize ncurses
    cbreak();              // Line buffering disabled, pass characters immediately
    noecho();              // Don't echo characters to the screen
    keypad(stdscr, TRUE);  // Enable the keypad for special keys

    char ch;
    while (1) {      // Exit loop on 'q' keypress
        move(0, 0);  // move the cursor to the beginning of the line
        int score[5] = {10, 3, 8, 1};
        int id[5] = {9, 5, 3, 1};
        char name[5][15] = {"Bartholomew", "Lily", "Johnathan", "Mary"};
        WINDOW *cardwin=newwin(7,48,2,35);
        
        // print scoreboard 
        scoreboard(score, id, name);
        for(int y=1;y<=34;y=y+3){
            mvwprintw(cardwin,1,y,"|------|");
            mvwprintw(cardwin,2,y,"|* * * |");
            mvwprintw(cardwin,3,y,"|* * * |");
            mvwprintw(cardwin,4,y,"|* * * |");
            mvwprintw(cardwin,5,y,"|* * * |");
            mvwprintw(cardwin,6,y,"|------|");

            wrefresh(cardwin);
            usleep(80000);
            wclear(cardwin);
        }
        endwin();
        card();
        // read input 
        ch = getch();
        if (ch == 'q') {
            printw("bye!\n");
            break;
        } 
        // else if (ch == '\n')
        //     printw("you pressed enter!\n");
        // else if (ch == ' ')
        //     printw("you pressed space!\n");
        else
            printw("you pressed %c!\n", ch);

        // print message

        refresh();
    }
    endwin();
    return 0;
}
