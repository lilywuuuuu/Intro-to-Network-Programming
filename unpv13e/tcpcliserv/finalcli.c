#include <arpa/inet.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "unp.h"

void handle_alarm(int sig); 
void line();
void scoreboard(int score[5], int id[5], char name[5][15]);
void card();
void draw(int mousex, int mousey, int blank);
void counter(int num);
void show_card(int kind, int num);
void flip_card(WINDOW* cardwin);

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    int maxfdp1, stdineof, peer_exit, n, id;
    fd_set rset;
    char readbuffer[MAXLINE], recvline[MAXLINE], sendline[MAXLINE];
    char username[MAXLINE];
    FILE *fp = stdin;

    // initialize connection
    if (argc != 3)
        err_quit("usage: tcpcli <IPaddress> <ID>");

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    // strcpy(name, argv[2]);
    Connect(sockfd, (SA *)&servaddr, sizeof(servaddr));
    signal(SIGALRM, handle_alarm);

    // welcome message
    snprintf(username, MAXLINE, "%s", argv[2]);
    snprintf(sendline, MAXLINE, "%s\n", username);
    if(strlen(username) > 15)
        err_quit("username too long\n");
    Writen(sockfd, sendline, strlen(sendline));  // name
    printf("Welcome to Slapjack, %s!\n", username);
    readline(sockfd, recvline, MAXLINE);  // id
    sscanf(recvline, "%d", &id);
    printf("Your ID is %d.\n", id);
    printf("Please wait for the server to put you in a room...\n");

    stdineof = 0;
    peer_exit = 0;

    // wait for server to put you in a room
    while (1) {
        // initalize select
        FD_ZERO(&rset);
        maxfdp1 = 0;
        if (stdineof == 0) {
            FD_SET(fileno(fp), &rset);
            maxfdp1 = fileno(fp);
        }
        if (peer_exit == 0) {
            FD_SET(sockfd, &rset);
            if (sockfd > maxfdp1)
                maxfdp1 = sockfd;
        }
        maxfdp1++;

        Select(maxfdp1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &rset)) {  // socket is readable
            n = readline(sockfd, recvline, MAXLINE);
            if (n == 0) {
                if (stdineof == 1)
                    return 0;  // normal termination
                else {
                    printf("Server has shutdown.\n");
                    peer_exit = 1;
                }
            } else if (n > 0) {
                recvline[n] = '\0';
                if (strcmp(recvline, "sorry\n") == 0) {
                    printf("Sorry, the rooms are full. Please try again later.\n");
                    return 0;  // disconnect
                } else if (strcmp(recvline, "waiting\n") == 0) {
                    printf("You are in a room! Please wait for the game to start.\n");
                } else if (strcmp(recvline, "1\n") == 0) {
                    printf("The room currently has 1 player...\n");
                } else if (strcmp(recvline, "2\n") == 0) {
                    printf("The room currently has 2 players...\n");
                } else if (strcmp(recvline, "3\n") == 0) {
                    printf("The room currently has 3 players...\n");
                } else if (strcmp(recvline, "4\n") == 0) {
                    printf("The room currently has 4 players...\n");
                    printf("Game is starting!\n");
                    break;
                }
            }
        }
        if (FD_ISSET(fileno(fp), &rset)) {  // input is readable
            Fgets(readbuffer, MAXLINE, fp);
            if (strcmp(readbuffer, "q\n") == 0) {
                if (peer_exit)
                    return 0;
                else {
                    printf("Thank you for playing, see you next time!\n");
                    stdineof = 1;
                    return 0;  // disconnect
                };
            }
        }
    }

    // game starts
    initscr();             // Initialize ncurses
    cbreak();              // Line buffering disabled, pass characters immediately
    noecho();              // Don't echo characters to the screen
    keypad(stdscr, TRUE);  // Enable the keypad for special keys
    start_color();
    init_pair(1,COLOR_RED,COLOR_RED);
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
    init_pair(3,COLOR_BLACK,COLOR_BLACK);
    // nodelay(stdscr, TRUE);  // Don't wait for input

    // int sock_flags = fcntl(sockfd, F_GETFL, 0);
    int card_num = 0, pattern = 0, round = 0;
    int score[5] = {0, 0, 0, 0};
    int player_id[5] = {0, 0, 0, 0};
    char name[5][15] = {"", "", "", ""};

    // track time
    struct timeval start, end;
    long seconds, useconds;
    double elapsed;

    // initialize scoreboard
    readline(sockfd, recvline, MAXLINE);
    sscanf(recvline, "%s %s %s %s %d %d %d %d",
           name[0], name[1], name[2], name[3],
           &player_id[0], &player_id[1], &player_id[2], &player_id[3]);
    move(0, 0);
    scoreboard(score, player_id, name);

    // fcntl(sockfd, F_SETFL, O_NONBLOCK);  // set socket to non-blocking

    char ch;
    while (1) {  // Exit loop on 'q' keypress
        // flipper?
        readline(sockfd, recvline, MAXLINE);
        if (strcmp(recvline, "flip\n") == 0) {  // your turn
            move(20, 0);
            printw("It's your turn! Press any key to flip a card.\n");
            alarm(3);
            // read input
            flushinp();
            ch = getch();
            if (ch == 'q') {
                refresh();
                move(0, 0);
                printw("bye!\n");
                break;
            } else {
                Writen(sockfd, "flip\n", 5);
            }
        }

        // get card number and round number from server
        readline(sockfd, recvline, MAXLINE);
        sscanf(recvline, "%d %d", &card_num, &round);

        // print card
        WINDOW *cardwin=newwin(19,48,1,35);
        attron(COLOR_PAIR(2));
        counter(round);
        flip_card(cardwin);
        show_card(pattern, card_num);
        attroff(COLOR_PAIR(2));
        gettimeofday(&start, NULL);

        // read input
        int hit = 1;
        flushinp();
        alarm(3);
        ch = getch();
        if (ch == 'q') {
            printw("bye!\n");
            break;
        } else {
            if (ch == '\n')
                printw("you pressed enter!\n");
            else if (ch == ' ')
                printw("you pressed space!\n");
            else if (ch == -1){ // didn't hit 
                hit = 0;
            }
            else
                printw("you pressed %c!\n", ch);

            gettimeofday(&end, NULL);
        }

        // calculate time if hit
        if(hit){
            seconds = end.tv_sec - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            elapsed = seconds + useconds / 1000000.0;
            Writen(sockfd, "%.6f\n", elapsed);
        }

        // update scoreboard 
        readline(sockfd, recvline, MAXLINE);
        sscanf(recvline, "%s %s %s %s %d %d %d %d %d %d %d %d",
               name[0], name[1], name[2], name[3],
               &player_id[0], &player_id[1], &player_id[2], &player_id[3],
               &score[0], &score[1], &score[2], &score[3]);
        move(0, 0);
        scoreboard(score, player_id, name);

        // check if game is over
        readline(sockfd, recvline, MAXLINE);
        if (strcmp(recvline, "1\n") == 0) { // 3 players left
            move(15, 0);
            printw("Other players quit, you are the winner!\n");
            printw("Press any key to quit.\n");
            flushinp();
            ch = getch();
            refresh();
            move(0, 0);
            printw("bye!\n");
            break;
        } 
        else if (strcmp(recvline, "2\n") == 0){ // somebody won 
            readline(sockfd, recvline, MAXLINE);
            sscanf(recvline, "%s\n", name[0]);
            move(15, 0);
            printw("%s won the game!\n", name[0]);
            printw("Press any key to quit.\n");
            flushinp();
            ch = getch();
            refresh();
            move(0, 0);
            printw("bye!\n");
            break;
        }   
        refresh();
    }
    endwin();
    exit(0);
}

void handle_alarm(int sig) {
    return;
}

void line() {
    printw("=====================================================================================\n");
}

void scoreboard(int score[5], int id[5], char name[5][15]) {
    line();
    printw("|---------------------------|\n");
    printw("|\t Score Board\t    |\n");
    printw("|\t\t\t    |\n");
    printw("| Score   Name           ID |\n");
    for (int i = 0; i < 4; i++) {
        printw("|  %-2d     %-15s%-2d |\n", score[i], name[i], id[i]);
    }
    printw("|\t\t\t    |\n");
    printw("|---------------------------|\n");
    printw("\n\n\n\n\n\n\n\n\n\n");
    line();
}
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
        draw(14-1,4+7,7);
        draw(15-1,3+7,2);
        draw(15-1,10+7,2);
        draw(16-1,3+7,9);
        draw(17-1,3+7,2);
        draw(17-1,10+7,2);
        draw(18-1,3+7,2);
        draw(18-1,10+7,2);
    }else if(num==2){
        draw(2+11,41-31,9);
        draw(3+11,48-31,2);
        draw(4+11,41-31,9);
        draw(5+11,41-31,2);
        draw(6+11,41-31,9);
    }else if(num==3){
        draw(2+11,41-31,9);
        draw(3+11,48-31,2);
        draw(4+11,41-31,9);
        draw(5+11,48-31,2);
        draw(6+11,41-31,9);
    }else if(num==4){
        draw(2+11,41-31,2);
        draw(2+11,46-31,2);
        draw(3+11,41-31,2);
        draw(3+11,46-31,2);
        draw(4+11,41-31,9);
        draw(5+11,46-31,2);
        draw(6+11,46-31,2);
    }else if(num==5){
        draw(2+11,41-31,9);
        draw(3+11,41-31,2);
        draw(4+11,41-31,9);
        draw(5+11,48-31,2);
        draw(6+11,41-31,9);
    }else if(num==6){
        draw(2+11,41-31,9);
        draw(3+11,41-31,2);
        draw(4+11,41-31,9);
        draw(5+11,41-31,2);
        draw(5+11,48-31,2);
        draw(6+11,41-31,9);
    }else if(num==7){
        draw(2+11,41-31,9);
        draw(3+11,47-31,2);
        draw(4+11,46-31,2);
        draw(5+11,45-31,2);
        draw(6+11,45-31,2);
    }else if(num==8){
        draw(2+11,42-31,7);
        draw(3+11,41-31,2);
        draw(3+11,48-31,2);
        draw(4+11,42-31,7);
        draw(5+11,41-31,2);
        draw(5+11,48-31,2);
        draw(6+11,42-31,7);
    }else if(num==9){
        draw(2+11,41-31,9);
        draw(3+11,48-31,2);
        draw(3+11,41-31,2);
        draw(4+11,41-31,9);
        draw(5+11,48-31,2);
        draw(6+11,41-31,9);
    }else if(num==10){
        draw(2+11,41-31,2);
        draw(3+11,41-31,2);
        draw(4+11,41-31,2);
        draw(5+11,41-31,2);
        draw(6+11,41-31,2);
        draw(2+11,45-31,7);
        draw(3+11,45-31,2);
        draw(3+11,50-31,2);
        draw(4+11,45-31,2);
        draw(4+11,50-31,2);
        draw(5+11,45-31,2);
        draw(5+11,50-31,2);
        draw(6+11,45-31,7);
    }else if(num==11){
        draw(2+11,41-31,9);
        draw(3+11,46-31,2);
        draw(4+11,46-31,2);
        draw(5+11,41-31,2);
        draw(5+11,46-31,2);
        draw(6+11,42-31,5);
    }else if(num==12){
        draw(2+11,41-31,7);
        draw(3+11,41-31,2);
        draw(3+11,46-31,2);
        draw(4+11,41-31,2);
        draw(4+11,45-31,3);
        draw(5+11,41-31,8);
        draw(6+11,47-31,2);
    }else if(num==13){
        draw(2+11,41-31,2);
        draw(2+11,47-31,2);
        draw(3+11,41-31,2);
        draw(3+11,45-31,3);
        draw(4+11,41-31,5);
        draw(5+11,41-31,2);
        draw(5+11,45-31,3);
        draw(6+11,41-31,2);
        draw(6+11,47-31,2);
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
    for(int y=1;y<=16;y=y+3){
        mvwprintw(cardwin,0, y,"|-----------------------|");
        mvwprintw(cardwin,1, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,2, y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,3, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,4, y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,5, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,6, y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,7, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,8, y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,9, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,10,y,"|* * * * * * * * * * *  |");
        mvwprintw(cardwin,11,y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,12,y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,13,y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,14,y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,15,y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,16,y,"|-----------------------|");
        wrefresh(cardwin);
        usleep(80000);
        wclear(cardwin);
    }
    for(int y=16;y>=1;y=y-3){
        mvwprintw(cardwin,0, y,"|-----------------------|");
        mvwprintw(cardwin,1, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,2, y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,3, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,4, y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,5, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,6, y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,7, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,8, y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,9, y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,10,y,"|* * * * * * * * * * *  |");
        mvwprintw(cardwin,11,y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,12,y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,13,y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,14,y,"|* * * * * * * * * * * *|");
        mvwprintw(cardwin,15,y,"| * * * * * * * * * * * |");
        mvwprintw(cardwin,16,y,"|-----------------------|");
        wrefresh(cardwin);
        usleep(20000);
        wclear(cardwin);
    }
    delwin(cardwin);
    move(20,0);
    return;
}