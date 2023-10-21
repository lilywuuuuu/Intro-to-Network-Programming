#include	"unp.h"
#include	<stdlib.h>
#include	<stdio.h>

void
sig_chld(int signo)
{
        pid_t   pid;
        int             stat;

        while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
                ;
        return;
}

void xchg_data(int sockfd, FILE *fp)
{
    int i, rd, z, n;
    char line[MAXLINE], id[MAXLINE], addr1[MAXLINE], addr2[MAXLINE];
    time_t ticks;
    
    if ((n = Readline(sockfd, line, MAXLINE)) == 0) {
       fprintf(fp, "no id and IP addr received. quitting\n");
       return;
    };
    ticks = time(NULL);
    fprintf(fp, "%.24s: recv id & IP addr: %s\n", ctime(&ticks), line);
	sscanf(line, "%s %s", id, addr1);
    n = rand() % 10 + 1;
    fprintf(fp, "\t\t\twaiting for %d line(s)\n", n);
    for (i = 0; i < n; i++) {
       if ((rd = Readline(sockfd, line, MAXLINE)) == 0) {
          ticks = time(NULL);
          fprintf(fp, "%.24s: no response. quitting\n", ctime(&ticks));
          return;
       }
       ticks = time(NULL);
       fprintf(fp, "%.24s: recv: %s", ctime(&ticks), line);   
    };
    Writen(sockfd, "stop", 4);
    ticks = time(NULL);
    fprintf(fp, "%.24s: end of input! \"stop\" sent.\n", ctime(&ticks));
    if ((rd = Readline(sockfd, line, MAXLINE)) == 0) {
       ticks = time(NULL);
       fprintf(fp, "%.24s: no line count received. quitting\n", ctime(&ticks));
       return;
    }    
	sscanf(line, "%d %s", &z, addr2);
    //z = atoi(line);
    ticks = time(NULL);
    fprintf(fp, "%.24s: recv line count & IP addr: %s => ", ctime(&ticks), line);
    if (z == n && strcmp(addr1,addr2) == 0) {
       Writen(sockfd, "ok", 2);
       fprintf(fp, "correct! \"ok\" sent.\n");
    }
    else {
       Writen(sockfd, "nak", 3);
       fprintf(fp, "wrong! nak sent.\n");
    };
    return;
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
	servaddr.sin_port        = htons(SERV_PORT+1);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

        Signal(SIGCHLD, sig_chld);      /* must call waitpid() */

        if ((fp = fopen("hw2serv23.log", "a")) == NULL) {
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
						Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof (buff)),
                        ntohs(cliaddr.sin_port));
            srand((int) ticks);

			xchg_data(connfd, fp);	/* process the request */
            ticks = time(NULL);
            fprintf (fp, "%.24s: disconnected from %s, port %d\n",
                        ctime(&ticks),
            Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof (buff)),
                        ntohs(cliaddr.sin_port));

			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
	}
}