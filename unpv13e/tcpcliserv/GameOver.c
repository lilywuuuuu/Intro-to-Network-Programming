#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
void End()
{
    move(0, 16);
    printw("    __                           _                 ");
    move(1, 16);
    printw("  / __|   _ _ _ _   __   ___   / _ \\__   __ __ _ __ ");
    move(2, 16);
    printw(" | |  _ / _  | '_ \\/_ \\ / _ \\ | | | \\ \\ / / _ | '__|");
    move(3, 16);
    printw(" | |_| | (_| | | | | | |  __/ | |_| |\\ V |  __| |   ");
    move(4, 16);
    printw("  \\____|\\__,_|_| |_| |_|\\___|  \\___/  \\_/ \\___|_|");
    return;
}

void frame(char winner[15])
{
    move(6, 35);
    printw("|----------------------------------------------|");
    move(7, 35);
    printw("|                                              |");
    move(8, 35);
    printw("|                                              |");
    move(9, 35);
    printw("|                                              |");
    move(10, 35);
    printw("|                                              |");
    move(11, 35);
    printw("|                                              |");
    move(12, 35);
    printw("|                                              |");
    move(13, 35);
    printw("|----------------------------------------------|");
    char m[50];
    sprintf(m, "%s gets 10 points!", winner);
    int start = (49 - strlen(m)) / 2 + 35;
    move(8, start);
    printw("%s", m);
    sprintf(m, "The game is over.");
    start = (49 - strlen(m)) / 2+35;
    move(10, start);
    printw("%s", m);
    // sprintf(m, "Please wait for server to put you in a room.");
    // start = (62 - strlen(m)) / 2;
    // move(12, start + 16);
    // printw("%s", m);
}

void scoreboard(int score[5], int id[5], char name[5][15])
{
    printw("|---------------------------|\n");
    printw("| Score   Name           ID |\n");
    for (int i = 0; i < 4; i++)
    {
        if (score[i] == 10)
        {
            printw("|");
            attron(COLOR_PAIR(1));
            printw("  %-2d     %-15s%-2d ", score[i], name[i], id[i]);
            attroff(COLOR_PAIR(1));
            printw("|\n");
        }
        printw("|  %-2d     %-15s%-2d |\n", score[i], name[i], id[i]);
    }
    printw("|---------------------------|\n");
    return;
}
int main(void)
{
    initscr();            // Initialize ncurses
    cbreak();             // Line buffering disabled, pass characters immediately
    noecho();             // Don't echo characters to the screen
    keypad(stdscr, TRUE); // Enable the keypad for special keys
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    int score[5] = {10, 3, 8, 1};
    int id[5] = {9, 5, 3, 1};
    char name[5][15] = {"Bartholomew", "Lily", "Johnathan", "Mary"};
    while (1)
    {
        move(6, 0); // move the cursor to the beginning of the line
        // title();
        // frame(name, id);
        scoreboard(score, id, name);
        frame(name[0]);
        End();
        refresh();
    }

    endwin();
    return 0;
}