#include <arpa/inet.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>

#include "unp.h"

void handle_alarm(int sig, int sockfd){
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

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    int maxfdp1, stdineof, peer_exit, n, id;
    fd_set rset;
    char readbuffer[MAXLINE], recvline[MAXLINE];
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
    snprintf(username, MAXLINE, "%s\n", argv[2]);
    Writen(sockfd, username, strlen(username));  // name
    printf("Welcome to Slapjack, %s!\n", username);
    readline(sockfd, id, MAXLINE);  // id
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
            n = read(sockfd, recvline, MAXLINE);
            if (n == 0) {
                if (stdineof == 1)
                    return;  // normal termination
                else {
                    printf("Server has shutdown.\n");
                    peer_exit = 1;
                }
            } else if (n > 0) {
                recvline[n] = '\0';
                if (recvline == "sorry\n") {
                    printf("Sorry, the room is full. Please try again later.\n");
                    return;  // disconnect
                } else if (recvline == "waiting\n") {
                    printf("You are in a room! Please wait for the game to start.\n");
                } else if (recvline == "start\n") {
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
    initscr();              // Initialize ncurses
    cbreak();               // Line buffering disabled, pass characters immediately
    noecho();               // Don't echo characters to the screen
    keypad(stdscr, TRUE);   // Enable the keypad for special keys
    // nodelay(stdscr, TRUE);  // Don't wait for input

    int sock_flags = fcntl(sockfd, F_GETFL, 0);
    int card_num = 0, round = 0; 
    int score[5] = {0, 0, 0, 0};
    int player_id[5] = {0, 0, 0, 0};
    char name[5][15] = {"", "", "", ""};

    // fcntl(sockfd, F_SETFL, O_NONBLOCK);  // set socket to non-blocking

    char ch;
    while (1) {      // Exit loop on 'q' keypress
        // flipper?
        readline(sockfd, recvline, MAXLINE);
        if (recvline == "flip\n") {  // your turn
            move(10, 0);
            printw("It's your turn! Press any key to flip a card.\n");
            alarm(5);
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

        // update scoreboard from server and get card number (readline)
        readline(sockfd, recvline, MAXLINE);
        sscanf(recvline, "%d %d %s %s %s %s %d %d %d %d %d %d %d %d", 
               &card_num, &round, 
               name[0], name[1], name[2], name[3], 
               &player_id[0], &player_id[1], &player_id[2], &player_id[3],
               &score[0], &score[1], &score[2], &score[3]);

        // print scoreboard
        move(0, 0);
        scoreboard(score, id, name);

        // read input
        flushinp();
        ch = getch();
        if (ch == 'q') {
            printw("bye!\n");
            break;
        } else if (ch == '\n')
            printw("you pressed enter!\n");
        else if (ch == ' ')
            printw("you pressed space!\n");
        else
            printw("you pressed %c!\n", ch);

        // print message

        refresh();
    }
    endwin();
    exit(0);
}