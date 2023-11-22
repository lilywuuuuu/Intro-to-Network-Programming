#include	"unp.h"
#include	<stdlib.h>
#include	<stdio.h>
#include    <string.h>

void
sig_chld(int signo){
    pid_t   pid;
    int     stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0);
    return;
}

int main(int argc, char **argv){
	int			        listenfd, connfd1, connfd2, n, client_count, maxfdp1;
    int                 flag1, flag2; 
	pid_t			    childpid;
	socklen_t		    clilen1, clilen2;
	struct sockaddr_in	cliaddr1, cliaddr2, servaddr;
    char                buff[MAXLINE], recvline[MAXLINE], sendline[MAXLINE];
    char                name1[20], name2[20], compare[MAXLINE]; 
    time_t			    ticks;
    fd_set              rset; 

    // for TCP server
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT + 3);
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_chld);      /* must call waitpid() */

	while (1) {
        // Receive IDs from 2 clients
        client_count = 0;
        while (client_count != 2){
            if (client_count == 0){ // first user
                clilen1 = sizeof(cliaddr1);
                if ((connfd1 = accept(listenfd, (SA *) &cliaddr1, &clilen1)) < 0) {
                    if (errno == EINTR) continue; // try again
                    else err_sys("accept error");
                }
                printf("Client1 connected.\n");
                n = Read(connfd1, name1, 20);
                name1[n] = '\0';
                printf("Recv: %s\n", name1);
                Writen(connfd1, "You are the 1st user. Wait for the second one!\n", 48);
                printf("Sent: %s is the 1st user.\n", name1);
                Writen(connfd1, "\n", 2); // ?
                getpeername(connfd1, (SA *) &cliaddr1, &clilen1);
                client_count++;
            } else if (client_count == 1){ // second user
                clilen2 = sizeof(cliaddr2);
                if ((connfd2 = accept(listenfd, (SA *) &cliaddr2, &clilen2)) < 0) {
                    if (errno == EINTR) continue; // try again
                    else err_sys("accept error");
                }
                printf("Client2 connected.\n");
                n = Read(connfd2, name2, 20);
                name2[n] = '\0';
                printf("Recv: %s\n", name2);
                Writen(connfd2, "You are the 2nd user.\n", 23); 
                printf("Sent: %s is the 2nd user.\n", name2);
                Writen(connfd2, "\n", 2); // ?
                getpeername(connfd2, (SA *) &cliaddr2, &clilen2);
                client_count++;
            }
        }

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
            ticks = time(NULL);
            printf ("===================\n%.24s: connected from %s, port %d and from %s, port %d.\n===================\n",
                ctime(&ticks),
                Inet_ntop(AF_INET, &cliaddr1.sin_addr, buff, sizeof (buff)),
                ntohs(cliaddr1.sin_port), 
                Inet_ntop(AF_INET, &cliaddr2.sin_addr, buff, sizeof (buff)),
                ntohs(cliaddr2.sin_port));
            srand((int) ticks);
            // chat
            flag1 = 0; 
            flag2 = 0; 
            FD_ZERO(&rset);
            while(1){
                FD_SET(connfd1, &rset);
                FD_SET(connfd2, &rset);
                maxfdp1 = max(connfd1, connfd2) + 1;
                Select(maxfdp1, &rset, NULL, NULL, NULL);
                // client 1 is readable 
                if (FD_ISSET(connfd1, &rset)){ 
                    // Receive from client 1
                    n = Readline(connfd1, recvline, MAXLINE);
                    if (n <= 0){
                        printf("Client 1 disconnected.\n");
                        flag1 = 1;
                    } else { // Send to client 2
                        snprintf(sendline, sizeof(sendline), "(%s) %s", name1, recvline);
                        snprintf(compare, sizeof(compare), "(%s) \n", name1);
                        if (strcmp(sendline, compare) != 0){
                            Writen(connfd2, sendline, strlen(sendline));
                            printf("Sent client1's message to client2.\n");
                        }
                    }
                }
                // client 2 is readable
                if (FD_ISSET(connfd2, &rset)){ 
                    // Receive from client 2
                    n = Readline(connfd2, recvline, MAXLINE);
                    if (n <= 0){
                        printf("Client 2 disconnected.\n");
                        flag2 = 1;
                    } else { // Send to client 1
                        snprintf(sendline, sizeof(sendline), "(%s) %s", name2, recvline);
                        snprintf(compare, sizeof(compare), "(%s) \n", name2);
                        if (strcmp(sendline, compare) != 0){
                            Writen(connfd1, sendline, strlen(sendline));
                            printf("Sent client2's message to client1.\n");
                        }
                    }
                }
                // client 1 disconnected
                if (flag1){
                    snprintf(sendline, sizeof(sendline), "(%s left the room. Press Ctrl + D to leave.)\n", name1);
                    Writen(connfd2, sendline, strlen(sendline));
                    printf("Informed client2 that client1 has disconnected.\n");
                    shutdown(connfd2, SHUT_WR); // send FIN to client 2
                    printf("Shutting down client2's connection.\n");
                    Readn(connfd2, recvline, MAXLINE);
                    snprintf(sendline, sizeof(sendline), "(%s left the room.)\n", name2);
                    Writen(connfd1, sendline, strlen(sendline));
                    printf("Informed client1 that client2 has disconnected.\n");
                    shutdown(connfd1, SHUT_WR); // send FIN to client 1
                    printf("Shutting down client1's connection.\n");
                    Close(connfd2);
                    Close(connfd1);
                    break; 
                }
                // client 2 disconnected 
                if (flag2){
                    snprintf(sendline, sizeof(sendline), "(%s left the room. Press Ctrl + D to leave.)\n", name2);
                    Writen(connfd1, sendline, strlen(sendline));
                    printf("Informed client1 that client2 has disconnected.\n");
                    shutdown(connfd1, SHUT_WR); // send FIN to client 1
                    printf("Shutting down client1's connection.\n");
                    Readn(connfd1, recvline, MAXLINE);
                    snprintf(sendline, sizeof(sendline), "(%s left the room.)\n", name1);
                    Writen(connfd2, sendline, strlen(sendline));
                    printf("Informed client2 that client1 has disconnected.\n");
                    shutdown(connfd2, SHUT_WR); // send FIN to client 2
                    printf("Shutting down client2's connection.\n");
                    Close(connfd2);
                    Close(connfd1);
                    break; 
                }
            }
			exit(0);
		}
        // tell the other client about peer's info
        snprintf(sendline, sizeof(sendline), "The second user is %s from %s\n", name2, 
            Inet_ntop(AF_INET, (SA *) &cliaddr2.sin_addr, buff, sizeof(buff))); 
        Writen(connfd1, sendline, strlen(sendline));
        printf("Sent: %s", sendline);

        snprintf(sendline, sizeof(sendline), "The first user is %s from %s\n", name1, 
            Inet_ntop(AF_INET, (SA *) &cliaddr1.sin_addr, buff, sizeof(buff))); 
        Writen(connfd2, sendline, strlen(sendline));
        printf("Sent: %s", sendline);

		Close(connfd1);			/* parent closes connected socket */
        Close(connfd2);
	}
    printf("Server terminated.\n");
    return 0;
}