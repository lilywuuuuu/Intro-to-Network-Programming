#include <arpa/inet.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "unp.h"
#define xmove 2
#define ymove 2

void handle_alarm(int sig);
void scoreboard(int score[5], int id[5], char name[5][15]);
void card();
void draw(int mousex, int mousey, int blank);
void counter(int num);
void show_card(int kind, int num);
void flip_card(WINDOW *cardwin);
void before_flip();
void title();
void welcomeframe(char name[15], int id);
void gameover();
void endframe(char winner[15]);
void endscoreboard(int score[5], int id[5], char name[5][15]);

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    int maxfdp1, stdineof, peer_exit, n, id;
    fd_set rset;
    char readbuffer[MAXLINE], recvline[MAXLINE], sendline[MAXLINE];
    char username[MAXLINE];
    char ch;
    FILE *fp = stdin;

    // Initialize ncurses
    initscr();
    cbreak();              // Line buffering disabled, pass characters immediately
    noecho();              // Don't echo characters to the screen
    keypad(stdscr, TRUE);  // Enable the keypad for special keys
    start_color();
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);

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
    if (strlen(username) > 15)
        err_quit("Username is longer than 15 characters, please try again.");
    Writen(sockfd, sendline, strlen(sendline));  // name
    readline(sockfd, recvline, MAXLINE);         // id
    sscanf(recvline, "%d", &id);
    title();
    frame(username, id);
    refresh();

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
                    move(20, 30);
                    printw("Server has shutdown.\n");
                    peer_exit = 1;
                }
            } else if (n > 0) {
                recvline[n] = '\0';
                if (strcmp(recvline, "sorry\n") == 0) {
                    move(20, 23);
                    printw("Sorry, the rooms are full. Please try again later.\n");
                    move(21, 23);
                    printw("Press any key to quit.\n");
                    ch = getchar();
                    refresh();
                    return 0;  // disconnect
                } else if (strcmp(recvline, "waiting\n") == 0) {
                    move(20, 22);
                    printw("You are in a room! Please wait for the game to start.\n");
                    refresh();
                } else if (strcmp(recvline, "1\n") == 0) {
                    move(21, 31);
                    printw("The room currently has 1 player...\n");
                    refresh();
                } else if (strcmp(recvline, "2\n") == 0) {
                    move(21, 31);
                    printw("The room currently has 2 players...\n");
                    refresh();
                } else if (strcmp(recvline, "3\n") == 0) {
                    move(21, 31);
                    printw("The room currently has 3 players...\n");
                    refresh();
                } else if (strcmp(recvline, "4\n") == 0) {
                    move(21, 31);
                    printw("         Game is starting!         ");
                    refresh();
                    move(0, 0);
                    sleep(2);
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
                    move(20, 30);
                    printw("Thank you for playing, see you next time!\n");
                    stdineof = 1;
                    return 0;  // disconnect
                };
            }
        }
    }

    // game starts
    int card_num = 0, pattern = 0, round = 0;
    int score[4] = {0, 0, 0, 0};
    int player_id[4] = {0, 0, 0, 0};
    char name[4][15] = {"", "", "", ""};

    // track time
    struct timeval start, end;
    long seconds, useconds;
    double elapsed;

    // clear screen
    clear();
    refresh();

    while (1) {
        // update scoreboard
        move(22, 0);
        printw("                                       ");
        before_flip();
        readline(sockfd, recvline, MAXLINE);
        sscanf(recvline, "%s %s %s %s %d %d %d %d %d %d %d %d",
               name[0], name[1], name[2], name[3],
               &player_id[0], &player_id[1], &player_id[2], &player_id[3],
               &score[0], &score[1], &score[2], &score[3]);
        scoreboard(score, player_id, name);
        refresh();

        // flipper?
        readline(sockfd, recvline, MAXLINE);
        if (strcmp(recvline, "flip\n") == 0) {  // your turn
            move(xmove + 11, ymove + 2);
            printw("It's your turn!");
            move(xmove + 12, ymove + 2);
            printw("Press any key to flip a card.");
            alarm(3);
            // read input
            flushinp();
            ch = getch();
            if (ch == 'q') {
                refresh();
                move(0, 0);
                printw("Bye!\n");
                break;
            } else if (ch != -1) {
                Writen(sockfd, "flip\n", strlen("flip\n"));
            }
        }

        // get card number and round number from server
        readline(sockfd, recvline, MAXLINE);
        sscanf(recvline, "%d %d %d", &card_num, &pattern, &round);

        // print card
        WINDOW *cardwin = newwin(17, 49, 1 + xmove, 40 + ymove);
        flip_card(cardwin);
        counter(round);
        show_card(pattern, card_num);
        gettimeofday(&start, NULL);
        refresh();

        // read input
        int hit = 1;
        alarm(3);
        flushinp();
        ch = getch();
        if (ch == 'q') {
            printf("Bye!\n");
            break;
        } else {
            move(xmove + 22, ymove + 2);
            if (ch == '\n')
                printw("You pressed enter!");
            else if (ch == ' ')
                printw("You pressed space!");
            else if (ch == -1) {  // didn't hit
                hit = 0;
            } else
                printw("You pressed %c!", ch);
            refresh();
            gettimeofday(&end, NULL);
        }

        // calculate time if hit
        if (hit) {
            seconds = end.tv_sec - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            elapsed = seconds + useconds / 1000000.0;
            snprintf(sendline, MAXLINE, "%.6f\n", elapsed);
            move(xmove + 22, ymove + 2);
            printw("You hit the card in %.6f seconds.", elapsed);
            refresh();
            Writen(sockfd, sendline, strlen(sendline));
        }

        // check if game is over
        readline(sockfd, recvline, MAXLINE);
        if (strcmp(recvline, "1\n") == 0) {  // 3 players left
            endscoreboard(score, id, name);
            endframe(username);
            gameover();
            move(xmove + 15, ymove + 2);
            printw("Other players quit, you are the winner!\n");
            move(xmove + 16, ymove + 2);
            printw("Press any key to quit.");
            flushinp();
            ch = getch();
            refresh();
            break;
        } else if (strcmp(recvline, "2\n") == 0) {  // somebody won
            readline(sockfd, recvline, MAXLINE);
            endscoreboard(score, id, name);
            endframe(recvline);
            gameover();
            sscanf(recvline, "%s\n", name[0]);
            move(xmove + 15, ymove + 2);
            printw("%s won the game!", name[0]);
            move(xmove + 16, ymove + 2);
            printw("Press any key to quit.");
            flushinp();
            ch = getch();
            refresh();
            break;
        }
    }
    endwin();
    exit(0);
}

void handle_alarm(int sig) {
    return;
}
void scoreboard(int score[5], int id[5], char name[5][15]) {
    move(0, 0);
    printw("=====================================================================================\n");
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
    printw("=====================================================================================\n");
}
void card() {
    attron(COLOR_PAIR(2));
    move(1, 40);
    printw("|----------------------|");
    move(2, 40);
    printw("|                      |");
    move(3, 40);
    printw("|                      |");
    move(4, 40);
    printw("|                      |");
    move(5, 40);
    printw("|                      |");
    move(6, 40);
    printw("|                      |");
    move(7, 40);
    printw("|                      |");
    move(8, 40);
    printw("|                      |");
    move(9, 40);
    printw("|                      |");
    move(10, 40);
    printw("|                      |");
    move(11, 40);
    printw("|                      |");
    move(12, 40);
    printw("|                      |");
    move(13, 40);
    printw("|                      |");
    move(14, 40);
    printw("|                      |");
    move(15, 40);
    printw("|                      |");
    move(16, 40);
    printw("|                      |");
    move(17, 40);
    printw("|----------------------|");
    attroff(COLOR_PAIR(2));
    return;
}
void draw(int mousex, int mousey, int blank) {
    move(mousex, mousey);
    for (int i = 0; i < blank; i++) {
        printw(" ");
    }
    return;
}
void counter(int num) {
    move(xmove + 11, ymove + 2);
    printw("Counter:       ");
    move(xmove + 12, ymove + 2);
    printw("                             ");
    attron(COLOR_PAIR(2));
    if (num == 1) {
        draw(14, 13, 2);
        draw(15, 13, 2);
        draw(16, 6 + 7, 2);
        draw(17, 6 + 7, 2);
        draw(18, 13, 2);
    } else if (num == 2) {
        draw(2 + 12, 41 - 31, 9);
        draw(3 + 12, 48 - 31, 2);
        draw(4 + 12, 41 - 31, 9);
        draw(5 + 12, 41 - 31, 2);
        draw(6 + 12, 41 - 31, 9);
    } else if (num == 3) {
        draw(2 + 12, 41 - 31, 9);
        draw(3 + 12, 48 - 31, 2);
        draw(4 + 12, 41 - 31, 9);
        draw(5 + 12, 48 - 31, 2);
        draw(6 + 12, 41 - 31, 9);
    } else if (num == 4) {
        draw(2 + 12, 41 - 31, 2);
        draw(2 + 12, 46 - 31, 2);
        draw(3 + 12, 41 - 31, 2);
        draw(3 + 12, 46 - 31, 2);
        draw(4 + 12, 41 - 31, 9);
        draw(5 + 12, 46 - 31, 2);
        draw(6 + 12, 46 - 31, 2);
    } else if (num == 5) {
        draw(2 + 12, 41 - 31, 9);
        draw(3 + 12, 41 - 31, 2);
        draw(4 + 12, 41 - 31, 9);
        draw(5 + 12, 48 - 31, 2);
        draw(6 + 12, 41 - 31, 9);
    } else if (num == 6) {
        draw(2 + 12, 41 - 31, 9);
        draw(3 + 12, 41 - 31, 2);
        draw(4 + 12, 41 - 31, 9);
        draw(5 + 12, 41 - 31, 2);
        draw(5 + 12, 48 - 31, 2);
        draw(6 + 12, 41 - 31, 9);
    } else if (num == 7) {
        draw(2 + 12, 41 - 31, 9);
        draw(3 + 12, 47 - 31, 2);
        draw(4 + 12, 46 - 31, 2);
        draw(5 + 12, 45 - 31, 2);
        draw(6 + 12, 45 - 31, 2);
    } else if (num == 8) {
        draw(2 + 12, 42 - 31, 7);
        draw(3 + 12, 41 - 31, 2);
        draw(3 + 12, 48 - 31, 2);
        draw(4 + 12, 42 - 31, 7);
        draw(5 + 12, 41 - 31, 2);
        draw(5 + 12, 48 - 31, 2);
        draw(6 + 12, 42 - 31, 7);
    } else if (num == 9) {
        draw(2 + 12, 41 - 31, 9);
        draw(3 + 12, 48 - 31, 2);
        draw(3 + 12, 41 - 31, 2);
        draw(4 + 12, 41 - 31, 9);
        draw(5 + 12, 48 - 31, 2);
        draw(6 + 12, 41 - 31, 9);
    } else if (num == 10) {
        draw(2 + 12, 41 - 31, 2);
        draw(3 + 12, 41 - 31, 2);
        draw(4 + 12, 41 - 31, 2);
        draw(5 + 12, 41 - 31, 2);
        draw(6 + 12, 41 - 31, 2);
        draw(2 + 12, 45 - 31, 7);
        draw(3 + 12, 45 - 31, 2);
        draw(3 + 12, 50 - 31, 2);
        draw(4 + 12, 45 - 31, 2);
        draw(4 + 12, 50 - 31, 2);
        draw(5 + 12, 45 - 31, 2);
        draw(5 + 12, 50 - 31, 2);
        draw(6 + 12, 45 - 31, 7);
    } else if (num == 11) {
        draw(2 + 12, 41 - 31, 2);
        draw(3 + 12, 41 - 31, 2);
        draw(4 + 12, 41 - 31, 2);
        draw(5 + 12, 41 - 31, 2);
        draw(6 + 12, 41 - 31, 2);
        draw(2 + 12, 47 - 31, 2);
        draw(3 + 12, 47 - 31, 2);
        draw(4 + 12, 47 - 31, 2);
        draw(5 + 12, 47 - 31, 2);
        draw(6 + 12, 47 - 31, 2);
    } else if (num == 12) {
        draw(2 + 12, 41 - 31, 2);
        draw(3 + 12, 41 - 31, 2);
        draw(4 + 12, 41 - 31, 2);
        draw(5 + 12, 41 - 31, 2);
        draw(6 + 12, 41 - 31, 2);
        draw(2 + 12, 45 - 31, 7);
        draw(3 + 12, 50 - 31, 2);
        draw(4 + 12, 45 - 31, 7);
        draw(5 + 12, 45 - 31, 2);
        draw(6 + 12, 45 - 31, 7);
    } else if (num == 0) {
        draw(2 + 12, 41 - 31, 2);
        draw(3 + 12, 41 - 31, 2);
        draw(4 + 12, 41 - 31, 2);
        draw(5 + 12, 41 - 31, 2);
        draw(6 + 12, 41 - 31, 2);
        draw(2 + 12, 45 - 31, 7);
        draw(3 + 12, 50 - 31, 2);
        draw(4 + 12, 45 - 31, 7);
        draw(5 + 12, 50 - 31, 2);
        draw(6 + 12, 45 - 31, 7);
    }
    move(20, 0);
    attroff(COLOR_PAIR(2));
    return;
}
void show_card(int kind, int num) {
    card();
    if (kind == 0) {
        attron(COLOR_PAIR(3));
        draw(12 - 4, 51, 2);
        draw(13 - 4, 48, 8);
        draw(14 - 4, 49, 6);
        draw(15 - 4, 45, 14);
        draw(16 - 4, 43, 18);
        draw(17 - 4, 45, 5);
        draw(13, 51, 2);
        draw(13, 54, 5);
        draw(18 - 4, 51, 2);
        draw(15, 49, 6);
        attroff(COLOR_PAIR(3));
    } else if (kind == 1) {
        attron(COLOR_PAIR(1));
        draw(8, 51, 2);
        draw(9, 50, 4);
        draw(10, 48, 8);
        draw(11, 46, 12);
        draw(12, 44, 16);
        draw(13, 46, 12);
        draw(14, 48, 8);
        draw(15, 50, 4);
        draw(16, 51, 2);
        attroff(COLOR_PAIR(1));
    } else if (kind == 2) {
        attron(COLOR_PAIR(1));
        draw(8, 44, 4);
        draw(8, 56, 4);
        draw(9, 43, 7);
        draw(9, 54, 7);
        draw(10, 42, 9);
        draw(10, 53, 9);
        draw(11, 43, 18);
        draw(12, 45, 14);
        draw(13, 46, 12);
        draw(14, 47, 10);
        draw(15, 49, 6);
        draw(16, 51, 2);
        attroff(COLOR_PAIR(1));
    } else if (kind == 3) {
        attron(COLOR_PAIR(3));
        draw(8, 51, 2);
        draw(9, 49, 6);
        draw(10, 47, 10);
        draw(11, 45, 14);
        draw(12, 43, 18);
        draw(13, 43, 6);
        draw(13, 51, 2);
        draw(13, 55, 6);
        draw(14, 51, 2);
        draw(15, 50, 4);
        draw(16, 49, 6);
        attroff(COLOR_PAIR(3));
    }
    attron(COLOR_PAIR(3));
    if (num == 1) {
        // 45 center
        draw(2, 42, 7);
        draw(3, 41, 2);
        draw(3, 48, 2);
        draw(4, 41, 9);
        draw(5, 41, 2);
        draw(5, 48, 2);
        draw(6, 41, 2);
        draw(6, 48, 2);
    } else if (num == 2) {
        draw(2, 41, 9);
        draw(3, 48, 2);
        draw(4, 41, 9);
        draw(5, 41, 2);
        draw(6, 41, 9);
    } else if (num == 3) {
        draw(2, 41, 9);
        draw(3, 48, 2);
        draw(4, 41, 9);
        draw(5, 48, 2);
        draw(6, 41, 9);
    } else if (num == 4) {
        draw(2, 41, 2);
        draw(2, 46, 2);
        draw(3, 41, 2);
        draw(3, 46, 2);
        draw(4, 41, 9);
        draw(5, 46, 2);
        draw(6, 46, 2);
    } else if (num == 5) {
        draw(2, 41, 9);
        draw(3, 41, 2);
        draw(4, 41, 9);
        draw(5, 48, 2);
        draw(6, 41, 9);
    } else if (num == 6) {
        draw(2, 41, 9);
        draw(3, 41, 2);
        draw(4, 41, 9);
        draw(5, 41, 2);
        draw(5, 48, 2);
        draw(6, 41, 9);
    } else if (num == 7) {
        draw(2, 41, 9);
        draw(3, 47, 2);
        draw(4, 46, 2);
        draw(5, 45, 2);
        draw(6, 45, 2);
    } else if (num == 8) {
        draw(2, 42, 7);
        draw(3, 41, 2);
        draw(3, 48, 2);
        draw(4, 42, 7);
        draw(5, 41, 2);
        draw(5, 48, 2);
        draw(6, 42, 7);
    } else if (num == 9) {
        draw(2, 41, 9);
        draw(3, 48, 2);
        draw(3, 41, 2);
        draw(4, 41, 9);
        draw(5, 48, 2);
        draw(6, 41, 9);
    } else if (num == 10) {
        draw(2, 41, 2);
        draw(3, 41, 2);
        draw(4, 41, 2);
        draw(5, 41, 2);
        draw(6, 41, 2);
        draw(2, 45, 7);
        draw(3, 45, 2);
        draw(3, 50, 2);
        draw(4, 45, 2);
        draw(4, 50, 2);
        draw(5, 45, 2);
        draw(5, 50, 2);
        draw(6, 45, 7);
    } else if (num == 11) {
        draw(2, 41, 9);
        draw(3, 46, 2);
        draw(4, 46, 2);
        draw(5, 41, 2);
        draw(5, 46, 2);
        draw(6, 42, 5);
    } else if (num == 12) {
        draw(2, 41, 7);
        draw(3, 41, 2);
        draw(3, 46, 2);
        draw(4, 41, 2);
        draw(4, 45, 3);
        draw(5, 41, 8);
        draw(6, 47, 2);
    } else if (num == 0) {
        draw(2, 41, 2);
        draw(2, 47, 2);
        draw(3, 41, 2);
        draw(3, 45, 3);
        draw(4, 41, 5);
        draw(5, 41, 2);
        draw(5, 45, 3);
        draw(6, 41, 2);
        draw(6, 47, 2);
    }
    attroff(COLOR_PAIR(3));
    move(20, 0);
    return;
}
void flip_card(WINDOW *cardwin) {
    wattron(cardwin, COLOR_PAIR(2));
    attron(COLOR_PAIR(4));
    for (int y = 1; y <= 16; y = y + 3) {
        mvwprintw(cardwin, 0, y, "|----------------------|");
        mvwprintw(cardwin, 1, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 2, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 3, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 4, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 5, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 6, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 7, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 8, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 9, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 10, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 11, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 12, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 13, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 14, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 15, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 16, y, "|----------------------|");
        wrefresh(cardwin);
        usleep(80000);
        wclear(cardwin);
    }
    for (int y = 16; y >= 1; y = y - 3) {
        mvwprintw(cardwin, 0, y, "|----------------------|");
        mvwprintw(cardwin, 1, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 2, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 3, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 4, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 5, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 6, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 7, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 8, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 9, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 10, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 11, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 12, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 13, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 14, y, "|* * * * * * * * * * * |");
        mvwprintw(cardwin, 15, y, "| * * * * * * * * * * *|");
        mvwprintw(cardwin, 16, y, "|----------------------|");
        wrefresh(cardwin);
        usleep(20000);
        wclear(cardwin);
    }
    wrefresh(cardwin);
    delwin(cardwin);
    wattroff(cardwin, COLOR_PAIR(2));
    attroff(COLOR_PAIR(4));
    move(20, 0);
    return;
}
void before_flip() {
    attron(COLOR_PAIR(2));
    move(1, 40);
    printw("|----------------------|");
    move(2, 40);
    printw("| * * * * * * * * * * *|");
    move(3, 40);
    printw("|* * * * * * * * * * * |");
    move(4, 40);
    printw("| * * * * * * * * * * *|");
    move(5, 40);
    printw("|* * * * * * * * * * * |");
    move(6, 40);
    printw("| * * * * * * * * * * *|");
    move(7, 40);
    printw("|* * * * * * * * * * * |");
    move(8, 40);
    printw("| * * * * * * * * * * *|");
    move(9, 40);
    printw("|* * * * * * * * * * * |");
    move(10, 40);
    printw("| * * * * * * * * * * *|");
    move(11, 40);
    printw("|* * * * * * * * * * * |");
    move(12, 40);
    printw("| * * * * * * * * * * *|");
    move(13, 40);
    printw("|* * * * * * * * * * * |");
    move(14, 40);
    printw("| * * * * * * * * * * *|");
    move(15, 40);
    printw("|* * * * * * * * * * * |");
    move(16, 40);
    printw("| * * * * * * * * * * *|");
    move(17, 40);
    printw("|----------------------|");
    attroff(COLOR_PAIR(2));
    return;
}
void title() {
    move(0, 21);
    printw("  ____   _                    _               _     \n");
    move(1, 21);
    printw(" / ___| | |  __ _  _ __      | |  __ _   ___ | | __ \n");
    move(2, 21);
    printw(" \\___ \\ | | / _` || '_ \\  _  | | / _` | / __|| |/ / \n");
    move(3, 21);
    printw("  ___) || || (_| || |_) || |_| || (_| || (__ |   <  \n");
    move(4, 21);
    printw(" |____/ |_| \\__,_|| .__/  \\___/  \\__,_| \\___||_|\\_\\ \n");
    move(5, 21);
    printw("                  |_|                               \n");
    move(0, 0);
}
void welcomeframe(char name[15], int id) {
    move(6, 0);
    char m[30];
    sprintf(m, "Welcome to SlapJack, %s.", name);
    int start = (53 - strlen(m)) / 2;
    move(6, 21);
    printw("|---------------------------------------------------|\n");
    move(7, 21);
    printw("|                                                   |\n");
    move(8, 21);
    printw("|                                                   |\n");
    move(8, start + 21);
    printw("%s", m);
    ;
    move(8, 52 + 21);
    printw("|");
    move(9, 21);
    printw("|                  Your ID is %-2d                    |\n", id);
    move(10, 21);
    printw("| Please wait for the server to put you in a room!  |\n");
    move(11, 21);
    printw("|---------------------------------------------------|\n");
}
void gameover() {
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
void endframe(char winner[15]) {
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
    start = (49 - strlen(m)) / 2 + 35;
    move(10, start);
    printw("%s", m);
}
void endscoreboard(int score[5], int id[5], char name[5][15]) {
    move(6, 0);
    printw("|---------------------------|\n");
    printw("| Score   Name           ID |\n");
    for (int i = 0; i < 4; i++) {
        if (score[i] == 10) {
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