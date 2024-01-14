#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "unp.h"

void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        ;
    return;
}

int main(int argc, char **argv) {
    int listenfd, n, online_count, total_count, maxfdp1;
    int connfd[100], left[100] = {0};
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char names[100][100], recvline[MAXLINE], sendline[MAXLINE];
    fd_set rset, master_set;

    // set up TCP server
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT + 3);
    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_chld);

    // initialize
    online_count = 0;
    total_count = 0;
    FD_ZERO(&master_set);
    FD_SET(listenfd, &master_set);
    maxfdp1 = listenfd + 1;
    while (1) {
        rset = master_set;
        select(maxfdp1, &rset, NULL, NULL, NULL);
        // accpet clients
        if (FD_ISSET(listenfd, &rset)) {
            if (online_count < 10) {
                printf("========= Client %d connected. =========\n", online_count);
                total_count++;
                online_count++;
                clilen = sizeof(cliaddr);
                connfd[total_count] = Accept(listenfd, (SA *)&cliaddr, &clilen);
                FD_SET(connfd[total_count], &master_set);
                if (connfd[total_count] + 1 > maxfdp1)
                    maxfdp1 = connfd[total_count] + 1;

                // receive client name and send welcome messages
                n = Read(connfd[total_count], names[total_count], 100);
                names[total_count][n] = '\0';
                printf("Recv: %s\n", names[total_count]);
                snprintf(sendline, sizeof(sendline), "You are the #%d user.\n", online_count);
                Writen(connfd[total_count], sendline, strlen(sendline));
                snprintf(sendline, sizeof(sendline), "You may now type in or wait for other users.\n");
                Writen(connfd[total_count], sendline, strlen(sendline));
                printf("Sent: %s is the #%d user.\n", names[total_count], online_count);

                // tell others that a new user has joined
                for (int i = 1; i < total_count; i++) {
                    if (!left[i]) {
                        snprintf(sendline, sizeof(sendline), "(#%d user %s enters.)\n", online_count, names[total_count]);
                        Writen(connfd[i], sendline, strlen(sendline));
                    }
                }
            }
        }
        // see if clients have sent messages
        for (int i = 1; i <= total_count; i++) {
            if (left[i]) continue;
            if (FD_ISSET(connfd[i], &rset)) {
                // receive from client i
                n = Readline(connfd[i], recvline, MAXLINE);
                if (n <= 0) {
                    printf("--------- Client %d disconnected. ---------\n", i);
                    online_count--;

                    // tell client i bye
                    snprintf(sendline, sizeof(sendline), "Bye!\n");
                    Writen(connfd[i], sendline, strlen(sendline));

                    // tell others that client i left
                    if (online_count >= 2) {
                        snprintf(sendline, sizeof(sendline), "(%s left the room. %d users left.)\n", names[i], online_count);
                        for (int j = 1; j <= total_count; j++) {
                            if (j != i && !left[j]) Writen(connfd[j], sendline, strlen(sendline));
                        }
                    } else if (online_count == 1) {
                        snprintf(sendline, sizeof(sendline), "(%s left the room. You are the last one. Press Ctrl+D to leave or wait for a new user.)\n", names[i]);
                        for (int j = 1; j <= total_count; j++) {
                            if (j != i && !left[j]) Writen(connfd[j], sendline, strlen(sendline));
                        }
                    } else
                        printf("No one is left in the room.\n");

                    left[i] = 1;
                    FD_CLR(connfd[i], &master_set);
                    shutdown(connfd[i], SHUT_WR);
                    Close(connfd[i]);
                } else if (n == 1) {  // send "\n" to other clients
                    for (int j = 1; j <= total_count; j++) {
                        if (j != i && !left[j]) Writen(connfd[j], "\n", 1);
                    }
                } else if (n > 1) {  // send to other clients (ignore messages with only "\n")
                    printf("Receiving from client%d. connfd = %d\n", i, connfd[i]);
                    snprintf(sendline, sizeof(sendline), "(%s) %s", names[i], recvline);
                    printf("Sent client%d's message to others.\n", i);
                    for (int j = 1; j <= total_count; j++) {
                        if (j != i && !left[j]) Writen(connfd[j], sendline, strlen(sendline));
                    }
                }
            }
        }
    }
    return 0;
}