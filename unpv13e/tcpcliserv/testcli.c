#include <arpa/inet.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "unp.h"

int main(int argc, char **argv)
{
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
    // signal(SIGALRM, handle_alarm);

    // welcome message
    snprintf(username, MAXLINE, "%s", argv[2]);
    snprintf(sendline, MAXLINE, "%s\n", username);
    if (strlen(username) > 15)
        err_quit("username too long\n");        // username too long
    Writen(sockfd, sendline, strlen(sendline)); // name
    printf("Welcome to Slapjack, %s!\n", username);
    readline(sockfd, recvline, MAXLINE); // id
    sscanf(recvline, "%d", &id);
    printf("Your ID is %d.\n", id);
    printf("Please wait for the server to put you in a room...\n");
    while (1)
    {
        readline(sockfd, recvline, MAXLINE);
        if (strcmp(recvline, "4\n") == 0)
        {
            printf("Game started!\n");
            break;
        }
        else
        {
            printf("people: %s", recvline);
        }
    }
    int card_num = 0, pattern = 0, round = 0;
    int score[5] = {0, 0, 0, 0};
    int player_id[5] = {0, 0, 0, 0};
    char name[5][15] = {"", "", "", ""};
    readline(sockfd, recvline, MAXLINE);
    sscanf(recvline, "%s %s %s %s %d %d %d %d %d %d %d %d",
           name[0], name[1], name[2], name[3],
           &player_id[0], &player_id[1], &player_id[2], &player_id[3]);
    printf("start game\n\n");
    for (int i = 0; i < 4; i++)
    {
        printf("player: %s id: %d\n", name[i], player_id[i]);
    }

    while (1)
    {
        printf("New round !\n\n");
        readline(sockfd, recvline, MAXLINE);
        printf("my turn ? %s", recvline);
        readline(sockfd, recvline, MAXLINE);
        sscanf(recvline, "%d %d %d", &card_num, &pattern, &round);
        printf("card_num: %d, pattern: %d, round: %d\n", card_num, pattern, round);
        readline(sockfd, recvline, MAXLINE);
        sscanf(recvline, "%s %s %s %s %d %d %d %d %d %d %d %d",
               name[0], name[1], name[2], name[3],
               &player_id[0], &player_id[1], &player_id[2], &player_id[3],
               &score[0], &score[1], &score[2], &score[3]);

        for (int i = 0; i < 4; i++)
        {
            printf("player: %s id: %s score: %d\n", name[i], player_id[i], score[i]);
        }

        printf("\n");
        readline(sockfd, recvline, MAXLINE);
        printf("situation: %s", recvline);
    }
}