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
    int listenfd, connfd[100], n, maxfdp1;
    pid_t childpid;
    socklen_t clilen[100];
    struct sockaddr_in cliaddr[100], servaddr;
    char names[100][20], recvline[MAXLINE], sendline[MAXLINE], compare[MAXLINE];
    fd_set rset;
    key_t key1 = ftok("/etc", 65);
    key_t key2 = ftok("/etc", 70);
    int shmid1 = shmget(key1, sizeof(int), 0666 | IPC_CREAT);
    int shmid2 = shmget(key2, sizeof(int), 0666 | IPC_CREAT);
    int *online_count = (int *)shmat(shmid1, (void *)0, 0);  // shared memory
    int *client_count = (int *)shmat(shmid2, (void *)0, 0);  // shared memory
    printf("key1 = %d, key2 = %d\n", key1, key2);
    printf("shmid1 = %d, shmid2 = %d\n", shmid1, shmid2);
    // for TCP server
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT + 3);
    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_chld); /* must call waitpid() */

    *client_count = 0;
    *online_count = 0;
    while (1) {
        // accpet clients
        printf("client count = %d\n", *client_count);
        printf("online count = %d\n", *online_count);
        if (*online_count < 10) {
            clilen[*client_count + 1] = sizeof(cliaddr[*client_count + 1]);
            if ((connfd[*client_count + 1] = accept(listenfd, (SA *)&cliaddr[*client_count + 1], &clilen[*client_count + 1])) < 0) {
                if (errno == EINTR)
                    continue;
                else
                    err_sys("accept error");
            }
            *client_count = *client_count + 1;
            *online_count = *online_count + 1;
            printf("========= Client%d connected. =========\n", *online_count);
            printf("online count = %d\n", *online_count);
        }
        // receive client name
        n = Read(connfd[*client_count], names[*client_count], 20);
        names[*client_count][n] = '\0';
        printf("Recv: %s\n", names[*client_count]);
        snprintf(sendline, sizeof(sendline), "You are the #%d user.\n", *client_count);
        Writen(connfd[*client_count], sendline, strlen(sendline));
        snprintf(sendline, sizeof(sendline), "You may now type in or wait for other users.\n");
        Writen(connfd[*client_count], sendline, strlen(sendline));
        printf("Sent: %s is the #%d user.\n", names[*client_count], *online_count);

        // fork child
        if ((childpid = Fork()) == 0) { /* child process */
            Close(listenfd);            /* close listening socket */

            // tell others that a new user has joined
            for (int i = 1; i < *client_count; i++) {
                snprintf(sendline, sizeof(sendline), "(#%d user %s enters.)\n", *online_count, names[*client_count]);
                Writen(connfd[i], sendline, strlen(sendline));
            }
            FD_ZERO(&rset);
            while (1) {
                // set rset and maxfdp1
                maxfdp1 = -1;
                for (int i = 1; i <= *client_count; i++) {
                    FD_SET(connfd[i], &rset);
                    if (connfd[i] + 1 > maxfdp1) {
                        maxfdp1 = connfd[i] + 1;
                    }
                }
                // chat
                Select(maxfdp1, &rset, NULL, NULL, NULL);
                for (int i = 1; i <= *client_count; i++) {
                    if (FD_ISSET(connfd[i], &rset)) {
                        // receive from client i
                        n = Readline(connfd[i], recvline, MAXLINE);
                        if (n <= 0) {
                            printf("--------- Client %d disconnected. ---------\n", i);
                            *online_count = *online_count - 1;
                            printf("online count = %d\n", *online_count);
                            // tell client i bye
                            snprintf(sendline, sizeof(sendline), "Bye!\n");
                            Writen(connfd[i], sendline, strlen(sendline));
                            // tell others that client i left
                            if (*online_count >= 2) {
                                snprintf(sendline, sizeof(sendline), "(%s left the room. %d users left.)\n", names[i], *online_count);
                                for (int j = 1; j <= *client_count; j++) {
                                    if (j != i) Writen(connfd[j], sendline, strlen(sendline));
                                }
                            } else if (*online_count == 1) {
                                printf("Last one.\n");
                                snprintf(sendline, sizeof(sendline), "(%s left the room. You are the last one. Press Ctrl+D to leave or wait for a new user.)\n", names[i]);
                            }
                            shutdown(connfd[i], SHUT_WR);
                            Close(connfd[i]);
                            exit(0);
                        } else {  // send to other clients
                            snprintf(sendline, sizeof(sendline), "(%s) %s", names[i], recvline);
                            snprintf(compare, sizeof(compare), "(%s) \n", names[i]);
                            if (strcmp(sendline, compare) != 0) {
                                printf("Sent client%d's message to others.\n", i);
                                for (int j = 1; j <= *client_count; j++) {
                                    if (j != i) Writen(connfd[j], sendline, strlen(sendline));
                                }
                            }
                        }
                    }
                }
            }
        }
        // Close(connfd[*client_count]); /* parent closes connected socket */
    }
    printf("Server terminated.\n");
    return 0;
}