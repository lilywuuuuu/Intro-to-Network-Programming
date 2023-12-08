#include	"unp.h"
#include	<stdlib.h>
#include	<stdio.h>

void
sig_chld(int signo){
    pid_t   pid;
    int     stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0);
    return;
}

int main(int argc, char **argv){
	int			        TCPlistenfd, TCPconnfd, UDPsockfd, n, buff_num;
	pid_t			    childpid;
	socklen_t		    clilen;
	struct sockaddr_in	cliaddr, TCPservaddr, UDPservaddr;
    char                buff[MAXLINE], recvline[MAXLINE], sendline[MAXLINE];
    time_t			    ticks;

    // for TCP server
	TCPlistenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&TCPservaddr, sizeof(TCPservaddr));
	TCPservaddr.sin_family      = AF_INET;
	TCPservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	TCPservaddr.sin_port        = htons(SERV_PORT + 3);
	Bind(TCPlistenfd, (SA *) &TCPservaddr, sizeof(TCPservaddr));
	Listen(TCPlistenfd, LISTENQ);
    Signal(SIGCHLD, sig_chld);      /* must call waitpid() */

    // for UDP client
    if (argc != 2){
        err_quit("Usage: hw3cli <Server IP Address>\n");
    }
    bzero(&UDPservaddr, sizeof(UDPservaddr));
    UDPservaddr.sin_family      = AF_INET;
    UDPservaddr.sin_port        = htons(SERV_PORT + 2);
    Inet_pton(AF_INET, argv[1], &UDPservaddr.sin_addr.s_addr);
    UDPsockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    // send studentID + port num to UDP server
    snprintf(sendline, sizeof(sendline), "110550091 %d", SERV_PORT + 3);
    Sendto(UDPsockfd, sendline, strlen(sendline), 0, (SA *) &UDPservaddr, sizeof(UDPservaddr));
    printf("Sent: %s\n", sendline);

    // receive info from UDP server
    Recvfrom(UDPsockfd, recvline, MAXLINE, 0, NULL, NULL);
    printf("Recv: %s\n", recvline);
    sscanf(recvline, "%d %s %d", &buff_num, buff, &n);

    // send status code to UDP server
    Sendto(UDPsockfd, "13", 2, 0, (SA *) &UDPservaddr, sizeof(UDPservaddr));
    printf("Sent: 13\n");

    // fork n TCP server child and do things
	for (int i=0; i<n; i++) {
		clilen = sizeof(cliaddr);
        if ((TCPconnfd = accept(TCPlistenfd, (SA *) &cliaddr, &clilen)) < 0) {
            if (errno == EINTR)
                continue;               /* back to for() */
            else
                err_sys("accept error");
        }
		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(TCPlistenfd);	/* close listening socket */
            ticks = time(NULL);
            printf ("===================\n%.24s: connected from %s, port %d\n",
                ctime(&ticks),
                Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof (buff)),
                ntohs(cliaddr.sin_port));
            srand((int) ticks);
            
            // receive number x
            Readline(TCPconnfd, recvline, MAXLINE);
            printf("Recv: %s", recvline);
            
            // get peer IP address
            clilen = sizeof(cliaddr);
            getpeername(TCPconnfd, (SA *) &cliaddr, &clilen);

            // send peer port num + number x
            snprintf(sendline, sizeof(sendline), "%d %s", ntohs(cliaddr.sin_port), recvline); 
            printf("Sent: %s", sendline);
            Writen(TCPconnfd, sendline, strlen(sendline));

            // read result
            Readline(TCPconnfd, recvline, MAXLINE);
            printf("Recv: %s", recvline);

            // terminate child
			exit(0);
		}
		Close(TCPconnfd);			/* parent closes connected socket */
	}
}