#include	"unp.h"
#include	<stdlib.h>
#include	<stdio.h>

void
sig_chld(int signo)
{
        pid_t   pid;
        int     stat;

        while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
                ;
        return;
}

void xchg_data(int sockfd, FILE *fp)
{
    int x, y, z, n, p;
    char line[MAXLINE];
    
    if ((n = Readline(sockfd, line, MAXLINE)) == 0) {
       fprintf(fp, "no id received. quitting\n");
       return;
    };
    fprintf(fp, "recv id: %s\n", line);
    while (1) {
       x = rand() % 65536 + 1;
       y = rand() % 65536 + 1;
       sprintf(line, "%d %d\n", x, y);
       Writen(sockfd, line, strlen(line));
       fprintf(fp, "sent: %s\n", line);
       if ((n = Readline(sockfd, line, MAXLINE)) == 0) {
          fprintf(fp, "no response. quitting\n");
          return;
       };
       z = atoi(line);
       fprintf(fp, "recv %d => ", z);
       p = rand() % 10;
       if (p > 3) {
          Writen(sockfd, "nak\n", 4);
          fprintf(fp, "assuming error! nak sent.\n");       
       }
       else if (z == x + y) {
          Writen(sockfd, "ok\n", 3);
          fprintf(fp, "correct! ok sent.\n");
          return;
       }
       else {
          Writen(sockfd, "wrong\n", 6);
          fprintf(fp, "wrong! result sent.\n");
          return;
       };
    };
} 

int
main(int argc, char **argv)
{
	int			listenfd, connfd;
	pid_t			childpid;
	socklen_t		clilen;
	struct sockaddr_in	cliaddr, servaddr;
        char                    buff[MAXLINE];
        time_t			ticks;
        FILE			*fp;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

        Signal(SIGCHLD, sig_chld);      /* must call waitpid() */

        if ((fp = fopen("hw1serv23.log", "a")) == NULL) {
           printf("log file open error!\n");
           exit(0);
        };
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
                if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
                        if (errno == EINTR)
                                continue;               /* back to for() */
                        else
                                err_sys("accept error");
                }

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
                        ticks = time(NULL);
                        fprintf(fp, "===================\n");
                        fprintf (fp, "%.24s: connected from %s, port %d\n",
                        ctime(&ticks),
                        Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
                        ntohs(cliaddr.sin_port));
                        srand((int) ticks);

			xchg_data(connfd, fp);	/* process the request */
                        ticks = time(NULL);
                        fprintf (fp, "%.24s: disconnected from %s, port %d\n",
                             ctime(&ticks),
                             Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
                             ntohs(cliaddr.sin_port));

			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
	}
}