#include "unp.h"
#include "unpthread.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#define len_name 105
#define ROOM1 0
#define ROOM2 4
#define ROOM3 8
#define ROOM4 12
int counter = 0; /* incremented by threads */
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;
const char kick[] = "You won\n";
int participant[16];
int id[16];
<<<<<<< HEAD
char name[16][len_name];
=======
char name[16][MAXLINE];
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
void *room1(void *);
const char start[] = "start\n";
const char no_one[] = "no\n";
const char nobody[] = "empty\n";
int main(int argc, char **argv)
{
<<<<<<< HEAD
	srand(time(NULL));
	int listenfd, connfd, tmp, flag, i;
	char str[len_name];
	const char sorry[] = "sorry\n";
	const char waiting[] = "waiting\n";
=======
	int counter = 1; /* incremented by threads */
	srand(time(NULL));
	int listenfd, connfd, tmp, flag, i;
	char str[MAXLINE];
	// const char sorry[200] = "sorry\n";
	const char waiting[200] = "waiting\n";
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	pthread_t tidA, tidB;
<<<<<<< HEAD

=======
	char how_many[MAXLINE] = "";
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
	for (int i = 0; i < 16; i++)
	{
		participant[i] = -1;
	}
	Pthread_create(&tidA, NULL, &room1, NULL);
	sleep(1);
	// Pthread_create(&tidB, NULL, &doit, NULL);
	for (;;)
	{
<<<<<<< HEAD

=======
		// printf("OK\n");
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
		clilen = sizeof(cliaddr);
		tmp = Accept(listenfd, (SA *)&cliaddr, &clilen);
		sprintf(name[i], "%s", str);

		readline(tmp, str, sizeof(str));

		str[strlen(str) - 1] = '\0';
		flag = 0;
<<<<<<< HEAD
		writen(tmp, waiting, sizeof(waiting));
		Pthread_mutex_lock(&mutex1);

=======
		sprintf(how_many, "%d\n", counter + 1);
		writen(tmp, how_many, strlen(how_many));
		writen(tmp, waiting, strlen(waiting));
		Pthread_mutex_lock(&mutex1);

		// writen(participant[i], how_many, strlen(how_many));
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
		for (i = ROOM1; i < ROOM1 + 4; i++)
		{
			if (participant[i] == -1)
			{
				participant[i] = tmp;
				flag = 1;
				id[i] = counter;
				++counter;
<<<<<<< HEAD
				strcpy(name[i], str);
=======
				// printf("OK\n");
				sprintf(name[i], "%s", str);
				// printf("OK\n");
				// writen(participant[i], how_many, strlen(how_many));
				// printf("OK\n");
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
				break;
			}
		}
		Pthread_mutex_unlock(&mutex1);
		if (flag == 0)
		{
			Pthread_mutex_lock(&mutex2);

			for (i = ROOM2; i < ROOM2 + 4; i++)
			{
				if (participant[i] == -1)
				{
					participant[i] = tmp;
					flag = 1;
					id[i] = counter;
					++counter;
<<<<<<< HEAD
					strcpy(name[i], str);
=======
					sprintf(name[i], "%s", str);
					// writen(participant[i], how_many, strlen(how_many));
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
					break;
				}
			}
			Pthread_mutex_unlock(&mutex2);
		}
		if (flag == 0)
		{
			Pthread_mutex_lock(&mutex3);
			for (i = ROOM3; i < ROOM3 + 4; i++)
			{
				if (participant[i] == -1)
				{
					participant[i] = tmp;
					flag = 1;
					id[i] = counter;
					++counter;
<<<<<<< HEAD
					strcpy(name[i], str);
=======
					sprintf(name[i], "%s", str);
					// writen(participant[i], how_many, strlen(how_many));
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
					break;
				}
			}
			Pthread_mutex_unlock(&mutex3);
		}
		if (flag == 0)
		{
			Pthread_mutex_lock(&mutex4);
			for (i = ROOM4; i < ROOM4 + 4; i++)
			{
				if (participant[i] == -1)
				{
					participant[i] = tmp;
					flag = 1;
					id[i] = counter;
					++counter;
<<<<<<< HEAD
					strcpy(name[i], str);
=======
					sprintf(name[i], "%s", str);
					// writen(participant[i], how_many, strlen(how_many));
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
					break;
				}
			}
			Pthread_mutex_unlock(&mutex4);
		}
		if (flag == 0)
		{
<<<<<<< HEAD
			writen(tmp, sorry, sizeof(sorry));
=======
			sprintf(how_many, "sorry\n");
			writen(tmp, how_many, strlen(how_many));
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
			close(tmp);
		}
	}
}

void *
room1(void *vptr)
{
	const int ROOM = 0;
	fd_set fd;
<<<<<<< HEAD
	const char your_turn[] = "flip\n";
	const char not_your_turn[] = "don't_flip\n";
	char user_time[100], mes[100];
=======
	const char your_turn[200] = "flip\n";
	const char not_your_turn[200] = "don't_flip\n";
	char user_time[MAXLINE], mes[MAXLINE];
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
	int maxfdp1, flag = 0, people = 0, score[4] = {0}, num_ans, k, cards = 0, turn = 0, answer = 0, color, quit;
	double tmp_f;
	int who_quit[4];
	struct timeval tv, timeout;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;

	while (1)
	{
	re:
		cards = 1;
		answer = 1;
		turn = 0;

		for (int i = 0; i < 4; i++)
		{
			score[i] = 0;
		}

		while (1)
		{
			FD_ZERO(&fd);
			maxfdp1 = -1;
			people = 0;
			flag = 0;
			Pthread_mutex_lock(&mutex1);

			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1)
				{
					FD_SET(participant[i], &fd);
					maxfdp1 = max(participant[i], maxfdp1);
					++people;
				}
			}

			if (maxfdp1 == -1)
			{
				Pthread_mutex_unlock(&mutex1);
				sleep(5);
			}
			else
			{
				tv.tv_sec = 0;
				tv.tv_usec = 0;
				Select(maxfdp1 + 1, &fd, NULL, NULL, &tv);
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1)
						;
					{
						if (FD_ISSET(participant[i], &fd))
						{
							participant[i] = -1;
							people--;
						}
						else
						{
							char how_many[10];
							sprintf(how_many, "%d\n", people);
							writen(participant[i], how_many, strlen(how_many));
						}
					}
				}

				Pthread_mutex_unlock(&mutex1);
				if (people == 4)
				{

					// card_num round name id score
<<<<<<< HEAD
					char st[200];
					sprintf("%s %s %s %s %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3]);
=======
					char st[MAXLINE];
					sprintf(st, "%s %s %s %s %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3]);
					printf("%s\n", st);
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						writen(participant[i], st, strlen(st));
					}
					break;
				}
				else
				{
					sleep(5);
				}
			}
		}

		while (1)
		{
<<<<<<< HEAD
=======
			printf("room1st\n");
			int win = 0;
			int who = -1;
>>>>>>> 099849dec12c6145fe854dcd5315cf65de0da592
			for (int i = 0; i < 4; i++)
			{
				who_quit[i] = 0;
			}
			quit = 0;

			cards = rand() % 13;
			color = rand() % 4;
			answer = (answer + 1) % 13;

			int pre_turn = turn;
			maxfdp1 = -1;
			Pthread_mutex_lock(&mutex1);

			for (k = 1; k < 4; k++)
			{
				if (participant[ROOM + (pre_turn + k) % 4] != -1)
				{
					maxfdp1 = max(maxfdp1, participant[ROOM + ((pre_turn + k) % 4)]);
					if (turn == pre_turn)
					{
						turn = (pre_turn + k) % 4;
					}
				}
			}

			if (maxfdp1 == -1)
			{
				Pthread_mutex_unlock(&mutex1);
				goto re;
			}

			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (i - ROOM != turn && participant[i] != -1)
				{
					if (writen(participant[i], not_your_turn, sizeof(not_your_turn)) == 0)
					{
						who_quit[i - ROOM] = 1;
						++quit;
					}
				}
			}
			FD_ZERO(&fd);
			FD_SET(participant[turn], &fd);
			writen(participant[ROOM + turn], your_turn, sizeof(your_turn));
			num_ans = 0;

			timeout.tv_sec = 3;
			timeout.tv_usec = 0;
			num_ans = select(participant[ROOM + turn] + 1, &fd, NULL, NULL, &timeout);
			maxfdp1 = -1;
			sprintf(mes, "%d %d\n", cards, answer);
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1 && who_quit[i - ROOM] != 1)
				{
					if (writen(participant[ROOM + i], mes, strlen(mes)) == 0)
					{
						who_quit[i - ROOM] = 1;
						++quit;
					}
					else
					{
						maxfdp1 = max(maxfdp1, participant[i]);
					}
				}
			}
			FD_ZERO(&fd);
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1 && who_quit[i - ROOM] != 1)
				{
					FD_SET(participant[i], &fd);
				}
			}
			num_ans = 0;
			sleep(3);
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			num_ans = select(maxfdp1 + 1, &fd, NULL, NULL, &tv);
			if (answer == cards)
			{
				if (num_ans == 0)
				{
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && who_quit[i - ROOM] != 1)
						{
							if (writen(participant[i], no_one, sizeof(no_one)) == 0)
							{
								who_quit[i - ROOM] = 1;
								quit++;
							}
							// score?
						}
					}
				}
				double time = 100000;
				quit = 0;

				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1 && who_quit[i - ROOM] != 1 && FD_ISSET(participant[i], &fd))
					{
						if (readline(participant[i], user_time, sizeof(user_time)) == 0)
						{
							who_quit[i - ROOM] = 1;
							quit++;
						}
						else
						{
							sscanf(user_time, "%lf", &tmp_f);
							if (tmp_f < time)
							{
								who = i;
							}
						}
					}
				}

				if (who != -1)
				{
					score[who - ROOM]++;
					if (score[who - ROOM] >= 10)
					{
						win = 1;
					}
					// if (score[who - ROOM] >= 10)
					// {
					// 	for (int i = ROOM; i < ROOM + 4; i++)
					// 	{
					// 		if (participant[i] != -1 && who_quit[i - ROOM] != 1)
					// 		{

					// 			close(participant[i]);
					// 		}
					// 	}
					// 	for (int i = ROOM; i < ROOM + 4; i++)
					// 	{
					// 		participant[i] = -1;
					// 	}
					// 	Pthread_mutex_unlock(&mutex1);
					// 	goto re;
					// }
				}
			}
			else
			{
				if (num_ans != 0)
				{
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && who_quit[i - ROOM] != 1 && FD_ISSET(participant[i], &fd))
						{
							if (readline(participant[i], user_time, sizeof(user_time)) == 0)
							{
								who_quit[i - ROOM] = 1;
								quit++;
							}
							else
							{
								score[i] = max(score[i] - 1, 0);
							}
						}
					}
				}
			}
			// 0 : continue
			// 1 : three left
			// 2 : someone win ,send name
			char st[MAXLINE];
			quit = 0;
			for (int i = ROOM; i < ROOM + 4; i++)
			{

				if (who_quit[i - ROOM] == 1)
				{
					score[i - ROOM] = 0;
					participant[i] = -1;
					id[i] = 0;
					sprintf(name[i], "-");
					++quit;
				}
			}

			sprintf(st, "%s %s %s %s %d %d %d %d %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3], score[0], score[1], score[2], score[3]);
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1 && who_quit[i - ROOM] != 1)
				{
					writen(participant[i], st, strlen(st));
				}
			}

			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (quit >= 3)
				{
					sprintf(st, "1\n");
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && who_quit[i - ROOM] != 1)
						{
							writen(participant[i], st, strlen(st));
							close(participant[i]);
						}
					}
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						participant[i] = -1;
					}
					Pthread_mutex_unlock(&mutex1);
					goto re;
				}
				else if (win == 1)
				{
					sprintf(st, "2\n%s\n", name[who]);
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && who_quit[i - ROOM] != 1)
						{
							writen(participant[i], st, strlen(st));
							close(participant[i]);
						}
					}
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						participant[i] = -1;
					}
					Pthread_mutex_unlock(&mutex1);
					goto re;
				}
				else
				{
					sprintf(st, "0\n");
					if (participant[i] != -1 && who_quit[i - ROOM] != 1)
					{
						writen(participant[i], st, strlen(not_your_turn));
					}
				}
			}
			Pthread_mutex_unlock(&mutex1);
		}
	}

	return (NULL);
}