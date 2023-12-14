#include "unp.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ncurses.h>

int
main(int argc, char **argv){
	int					sockfd;
	struct sockaddr_in	servaddr;
    int         maxfdp1, stdineof, peer_exit, n, id;
    fd_set      rset;
    char        readbuffer[MAXLINE], recvline[MAXLINE];
    char        name[MAXLINE];
    FILE        *fp = stdin;

    // initialize connection
	if (argc != 3)
		err_quit("usage: tcpcli <IPaddress> <ID>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT+3);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	strcpy(name, argv[2]);
	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

    // welcome message
    snprintf(name, MAXLINE, "%s\n", name);
    Writen(sockfd, name, strlen(name)); // name
    printf("Welcome to Slapjack, %s!\n", name);
    readline(sockfd, id, MAXLINE); // id
    printf("Your ID is %d.\n", id);
    printf("Please wait for the server to put you in a room...\n");

    stdineof = 0;
	peer_exit = 0;

    // wait for server to put you in a room
    while(1) {	
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
		if (FD_ISSET(sockfd, &rset)) { // socket is readable
			n = read(sockfd, recvline, MAXLINE);
			if (n == 0) {
 		   		if (stdineof == 1) return; // normal termination
		   		else {
					printf("Server has shutdown.\n");
					peer_exit = 1;
				}
            }
			else if (n > 0) {
				recvline[n] = '\0';
				if (recvline == "sorry\n"){
                    printf("Sorry, the room is full. Please try again later.\n");
                    return; // disconnect
                }
                else if (recvline == "waiting\n"){
                    printf("You are in a room! Please wait for the game to start.\n");
                }
                else if (recvline == "start\n"){
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
					return 0; // disconnect
				};
            }
        }
    }

    // game starts
    initscr(); // Initialize ncurses
    cbreak(); // Line buffering disabled, pass characters immediately
    noecho(); // Don't echo characters to the screen
    keypad(stdscr, TRUE); // Enable the keypad for special keys

    char ch;
    while (1) { // Exit loop on 'q' keypress
        ch = getch();
        move(0, 0); // move the cursor to the beginning of the line
        if (ch == 'q') {
            printf("bye!\n");
            break;
        }
        else if (ch == '\n') printf("you pressed enter!\n");
        else if (ch == ' ') printf("you pressed space!\n");
        else printf("you pressed %c!\n", ch);
        fflush(stdout);
    }
    endwin();

	exit(0);
}