#include	"unp.h"
#include 	<string.h>

void exchange_data(FILE *fp, int sockfd){
	char	recvline[MAXLINE];
	char 	student_id[11] = "110550091\n";
	char 	*nums[10];
	int		first_num;
	int 	second_num;
	int 	sum;
	char 	sum_str[10]; 
	int 	i = 0, n;

	printf("sent: %s\n", student_id);
	Writen(sockfd, student_id, strlen(student_id));

	while (1){
		if ((n = Readline(sockfd, recvline, MAXLINE)) == 0) {
			printf("no numbers received, quitting\n");
			return;
		}
		char* token = strtok(recvline, " ");
		while (token != NULL){
			nums[i++] = token;
			token = strtok(NULL, " ");
		}
		sscanf(nums[0], "%d", &first_num);
		sscanf(nums[1], "%d", &second_num);
		printf("recv: %d %d\n", first_num, second_num);
		sum = first_num + second_num; 
		snprintf(sum_str, sizeof(sum_str), "%d\n", sum); // turn int to str
		printf("sent: %s\n", sum_str);
		Writen(sockfd, sum_str, strlen(sum_str));
		Readline(sockfd, recvline, MAXLINE);
		printf("recv: %s\n", recvline);
		if (strcmp(recvline, "nak\n") == 0){
			continue;
		}
		else if (strcmp(recvline, "ok\n") == 0 || strcmp(recvline, "wrong\n") == 0){
			break;
		}
	}
}

int main(int argc, char **argv){
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	exchange_data(stdin, sockfd);		/* do it all */

	exit(0);
}
