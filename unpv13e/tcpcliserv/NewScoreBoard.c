#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
void before_flip(){
    move(1,40);
    printw("|----------------------|");
    move(2,40);
    printw("| * * * * * * * * * * *|");
    move(3,40);
    printw("|* * * * * * * * * * * |");
    move(4,40);
    printw("| * * * * * * * * * * *|");
    move(5,40);
    printw("|* * * * * * * * * * * |");
    move(6,40);
    printw("| * * * * * * * * * * *|");
    move(7,40);
    printw("|* * * * * * * * * * * |");
    move(8,40);
    printw("| * * * * * * * * * * *|");
    move(9,40);
    printw("|* * * * * * * * * * * |");
    move(10,40);
    printw("| * * * * * * * * * * *|");
    move(11,40);
    printw("|* * * * * * * * * * * |");
    move(12,40);
    printw("| * * * * * * * * * * *|");
    move(13,40);
    printw("|* * * * * * * * * * * |");
    move(14,40);
    printw("| * * * * * * * * * * *|");
    move(15,40);
    printw("|* * * * * * * * * * * |");
    move(16,40);
    printw("| * * * * * * * * * * *|");
    move(17,40);
    printw("|----------------------|");
    return;
}
// Solution to flip_card()
// Add "wrefresh(cardwin)" after two for loops.

void card(){
    move(1,40);
    printw("|----------------------|");
    move(2,40);
    printw("|                      |");
    move(3,40);
    printw("|                      |");
    move(4,40);
    printw("|                      |");
    move(5,40);
    printw("|                      |");
    move(6,40);
    printw("|                      |");
    move(7,40);
    printw("|                      |");
    move(8,40);
    printw("|                      |");
    move(9,40);
    printw("|                      |");
    move(10,40);
    printw("|                      |");
    move(11,40);
    printw("|                      |");
    move(12,40);
    printw("|                      |");
    move(13,40);
    printw("|                      |");
    move(14,40);
    printw("|                      |");
    move(15,40);
    printw("|                      |");
    move(16,40);
    printw("|                      |");
    move(17,40);
    printw("|----------------------|");
    return;
}

void draw(int mousex, int mousey, int blank){
    move(mousex,mousey);
    for(int i=0;i<blank;i++){
        printw(" ");
    }
    return;
}
void counter(int num){
    if(num==1){
        draw(14,13,2);
        draw(15,13,2);
        draw(16,6+7,2);
        draw(17,6+7,2);
        draw(18,13,2);
    }else if(num==2){
        draw(2+12,41-31,9);
        draw(3+12,48-31,2);
        draw(4+12,41-31,9);
        draw(5+12,41-31,2);
        draw(6+12,41-31,9);
    }else if(num==3){
        draw(2+12,41-31,9);
        draw(3+12,48-31,2);
        draw(4+12,41-31,9);
        draw(5+12,48-31,2);
        draw(6+12,41-31,9);
    }else if(num==4){
        draw(2+12,41-31,2);
        draw(2+12,46-31,2);
        draw(3+12,41-31,2);
        draw(3+12,46-31,2);
        draw(4+12,41-31,9);
        draw(5+12,46-31,2);
        draw(6+12,46-31,2);
    }else if(num==5){
        draw(2+12,41-31,9);
        draw(3+12,41-31,2);
        draw(4+12,41-31,9);
        draw(5+12,48-31,2);
        draw(6+12,41-31,9);
    }else if(num==6){
        draw(2+12,41-31,9);
        draw(3+12,41-31,2);
        draw(4+12,41-31,9);
        draw(5+12,41-31,2);
        draw(5+12,48-31,2);
        draw(6+12,41-31,9);
    }else if(num==7){
        draw(2+12,41-31,9);
        draw(3+12,47-31,2);
        draw(4+12,46-31,2);
        draw(5+12,45-31,2);
        draw(6+12,45-31,2);
    }else if(num==8){
        draw(2+12,42-31,7);
        draw(3+12,41-31,2);
        draw(3+12,48-31,2);
        draw(4+12,42-31,7);
        draw(5+12,41-31,2);
        draw(5+12,48-31,2);
        draw(6+12,42-31,7);
    }else if(num==9){
        draw(2+12,41-31,9);
        draw(3+12,48-31,2);
        draw(3+12,41-31,2);
        draw(4+12,41-31,9);
        draw(5+12,48-31,2);
        draw(6+12,41-31,9);
    }else if(num==10){
        draw(2+12,41-31,2);
        draw(3+12,41-31,2);
        draw(4+12,41-31,2);
        draw(5+12,41-31,2);
        draw(6+12,41-31,2);
        draw(2+12,45-31,7);
        draw(3+12,45-31,2);
        draw(3+12,50-31,2);
        draw(4+12,45-31,2);
        draw(4+12,50-31,2);
        draw(5+12,45-31,2);
        draw(5+12,50-31,2);
        draw(6+12,45-31,7);
    }else if(num==11){
        draw(2+12,41-31,2);
        draw(3+12,41-31,2);
        draw(4+12,41-31,2);
        draw(5+12,41-31,2);
        draw(6+12,41-31,2);
        draw(2+12,47-31,2);
        draw(3+12,47-31,2);
        draw(4+12,47-31,2);
        draw(5+12,47-31,2);
        draw(6+12,47-31,2);
    }else if(num==12){
        draw(2+12,41-31,2);
        draw(3+12,41-31,2);
        draw(4+12,41-31,2);
        draw(5+12,41-31,2);
        draw(6+12,41-31,2);
        draw(2+12,45-31,7);
        draw(3+12,50-31,2);
        draw(4+12,45-31,7);
        draw(5+12,45-31,2);
        draw(6+12,45-31,7);
    }else if(num==0){
        draw(2+12,41-31,2);
        draw(3+12,41-31,2);
        draw(4+12,41-31,2);
        draw(5+12,41-31,2);
        draw(6+12,41-31,2);
        draw(2+12,45-31,7);
        draw(3+12,50-31,2);
        draw(4+12,45-31,7);
        draw(5+12,50-31,2);
        draw(6+12,45-31,7);
    }
    move(20,0);
    return;
}
void show_card(int kind, int num){
    card();
    if(kind==0){
        attron(COLOR_PAIR(3));
        draw(12-4,51,2);
        draw(13-4,48,8);
        draw(14-4,49,6);
        draw(15-4,45,14);
        draw(16-4,43,18);
        draw(17-4,45,5);
        draw(13,51,2);
        draw(13,54,5);
        draw(18-4,51,2);
        draw(15,49,6);
        attroff(COLOR_PAIR(3));
    } else if(kind==1){
        attron(COLOR_PAIR(1));
        draw(8,51,2);
        draw(9,50,4);
        draw(10,48,8);
        draw(11,46,12);
        draw(12,44,16);
        draw(13,46,12);
        draw(14,48,8);
        draw(15,50,4);
        draw(16,51,2);
        attroff(COLOR_PAIR(1));
    } else if(kind==2){
        attron(COLOR_PAIR(1));
        draw(8,44,4);
        draw(8,56,4);
        draw(9,43,7);
        draw(9,54,7);
        draw(10,42,9);
        draw(10,53,9);
        draw(11,43,18);
        draw(12,45,14);
        draw(13,46,12);
        draw(14,47,10);
        draw(15,49,6);
        draw(16,51,2);
        attroff(COLOR_PAIR(1));
    } else if(kind==3){
        attron(COLOR_PAIR(3));
        draw(8,51,2);
        draw(9,49,6);
        draw(10,47,10);
        draw(11,45,14);
        draw(12,43,18);
        draw(13,43,6);
        draw(13,51,2);
        draw(13,55,6);
        draw(14,51,2);
        draw(15,50,4);
        draw(16,49,6);
        attroff(COLOR_PAIR(3));
    }
    attron(COLOR_PAIR(3));
    if(num==1){
        //45 center
        draw(2,42,7);
        draw(3,41,2);
        draw(3,48,2);
        draw(4,41,9);
        draw(5,41,2);
        draw(5,48,2);
        draw(6,41,2);
        draw(6,48,2);
        
    }else if(num==2){
        draw(2,41,9);
        draw(3,48,2);
        draw(4,41,9);
        draw(5,41,2);
        draw(6,41,9);
    }else if(num==3){
        draw(2,41,9);
        draw(3,48,2);
        draw(4,41,9);
        draw(5,48,2);
        draw(6,41,9);
    }else if(num==4){
        draw(2,41,2);
        draw(2,46,2);
        draw(3,41,2);
        draw(3,46,2);
        draw(4,41,9);
        draw(5,46,2);
        draw(6,46,2);
    }else if(num==5){
        draw(2,41,9);
        draw(3,41,2);
        draw(4,41,9);
        draw(5,48,2);
        draw(6,41,9);
    }else if(num==6){
        draw(2,41,9);
        draw(3,41,2);
        draw(4,41,9);
        draw(5,41,2);
        draw(5,48,2);
        draw(6,41,9);
    }else if(num==7){
        draw(2,41,9);
        draw(3,47,2);
        draw(4,46,2);
        draw(5,45,2);
        draw(6,45,2);
    }else if(num==8){
        draw(2,42,7);
        draw(3,41,2);
        draw(3,48,2);
        draw(4,42,7);
        draw(5,41,2);
        draw(5,48,2);
        draw(6,42,7);
    }else if(num==9){
        draw(2,41,9);
        draw(3,48,2);
        draw(3,41,2);
        draw(4,41,9);
        draw(5,48,2);
        draw(6,41,9);
    }else if(num==10){
        draw(2,41,2);
        draw(3,41,2);
        draw(4,41,2);
        draw(5,41,2);
        draw(6,41,2);
        draw(2,45,7);
        draw(3,45,2);
        draw(3,50,2);
        draw(4,45,2);
        draw(4,50,2);
        draw(5,45,2);
        draw(5,50,2);
        draw(6,45,7);
    }else if(num==11){
        draw(2,41,9);
        draw(3,46,2);
        draw(4,46,2);
        draw(5,41,2);
        draw(5,46,2);
        draw(6,42,5);
    }else if(num==12){
        draw(2,41,7);
        draw(3,41,2);
        draw(3,46,2);
        draw(4,41,2);
        draw(4,45,3);
        draw(5,41,8);
        draw(6,47,2);
    }else if(num==13){
        draw(2,41,2);
        draw(2,47,2);
        draw(3,41,2);
        draw(3,45,3);
        draw(4,41,5);
        draw(5,41,2);
        draw(5,45,3);
        draw(6,41,2);
        draw(6,47,2);
    }
    attroff(COLOR_PAIR(3));
    move(20,0);
    return;
}
void flip_card(WINDOW* cardwin){
    wattron(cardwin,COLOR_PAIR(2));
    attron(COLOR_PAIR(4));
    for(int y=0;y<=15;y=y+3){
        mvwprintw(cardwin,0, y,"|----------------------|");
        mvwprintw(cardwin,1, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,2, y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,3, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,4, y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,5, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,6, y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,7, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,8, y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,9, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,10,y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,11,y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,12,y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,13,y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,14,y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,15,y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,16,y,"|----------------------|");
        wrefresh(cardwin);
        usleep(80000);
        wclear(cardwin);
    }
    for(int y=15;y>=0;y=y-3){
        mvwprintw(cardwin,0, y,"|----------------------|");
        mvwprintw(cardwin,1, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,2, y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,3, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,4, y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,5, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,6, y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,7, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,8, y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,9, y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,10,y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,11,y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,12,y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,13,y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,14,y,"|* * * * * * * * * * * |");
        mvwprintw(cardwin,15,y,"| * * * * * * * * * * *|");
        mvwprintw(cardwin,16,y,"|----------------------|");
        wrefresh(cardwin);
        usleep(150000);
        wclear(cardwin);
    }
    wrefresh(cardwin);
    move(20,0);
    return;
}
void line(){
    printw("=============================================================================================\n");
    return;
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
    return;
}

int main(void) {
    initscr();             // Initialize ncurses
    cbreak();              // Line buffering disabled, pass characters immediately
    noecho();              // Don't echo characters to the screen
    keypad(stdscr, TRUE);  // Enable the keypad for special keys
    start_color();
    init_pair(1,COLOR_RED,COLOR_RED);
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
    init_pair(3,COLOR_WHITE,COLOR_BLACK);
    char ch;
    while (1) {      // Exit loop on 'q' keypress
        move(0, 0);  // move the cursor to the beginning of the line
        int score[5] = {10, 3, 8, 1};
        int id[5] = {9, 5, 3, 1};
        char name[5][15] = {"Bartholomew", "Lily", "Johnathan", "Mary"};
        
        // print scoreboard 
        scoreboard(score, id, name);
        WINDOW *cardwin=newwin(17,48,1,40);
        attron(COLOR_PAIR(2));
        card();
        attroff(COLOR_PAIR(2));
        flip_card(cardwin);
        //card();
        attron(COLOR_PAIR(2));
        show_card(2,13);
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(3));
        move(11,2);
        printw("Messege from server.");
        move(13,2);
        printw("Counter:");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(2));
        counter(1);
        attroff(COLOR_PAIR(2));
        move(20,0);
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
        move(0,0);
        refresh();
        before_flip();
    }
    endwin();
    return 0;
}
