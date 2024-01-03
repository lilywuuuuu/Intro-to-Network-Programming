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
// int a;
int sep_room[4];
int room_status[4];
pthread_mutex_t mutex[4];

const char kick[200] = "You won\n";
int participant[16];
int id[16];
char name[16][len_name];
void *room1(void *);
const char start[200] = "start\n";
const char no_one[200] = "no\n";
const char nobody[200] = "empty\n";
int main(int argc, char **argv)
{
	for (int i = 0; i < 4; i++)
	{
		sep_room[i] = i * 4;
		for (int i = 0; i < 4; i++)
		{
			sep_room[i] = i * 4;
			pthread_mutex_init(&mutex[i], NULL);
		}
		room_status[i] = 0;
	}
	signal(SIGPIPE, SIG_IGN);

	int counter = 1; /* incremented by threads */
	srand(time(NULL));
	int listenfd, tmp, flag;
	char str[MAXLINE];
	// const char sorry[200] = "sorry\n";
	const char waiting[200] = "waiting\n";
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	pthread_t tid[4];
	char how_many[MAXLINE] = "";
	for (int i = 0; i < 16; i++)
	{
		participant[i] = -1;
	}
	for (int i = 0; i < 4; i++)
	{
		int ttt = i;
		Pthread_create(&(tid[i]), NULL, &room1, (void *)(long)(ttt));
	}

	sleep(1);
	// Pthread_create(&tidB, NULL, &doit, NULL);
	for (;;)
	{
		// printf("OK\n");

		clilen = sizeof(cliaddr);
		tmp = Accept(listenfd, (SA *)&cliaddr, &clilen);
		// sprintf(name[i], "%s", str);
		printf("accepting new client\n");

		readline(tmp, str, sizeof(str));

		str[strlen(str) - 1] = '\0';
		flag = 0;
		sprintf(how_many, "%d\n", counter);
		if (writen(tmp, how_many, strlen(how_many)) <= 0)
		{
			continue;
		}
		if (writen(tmp, waiting, strlen(waiting)) <= 0)
		{
			continue;
		}
		if (room_status[0] == 0)
		{
			Pthread_mutex_lock(&(mutex[0]));

			// writen(participant[i], how_many, strlen(how_many));
			for (int i = ROOM1; i < ROOM1 + 4; i++)
			{
				if (participant[i] == -1)
				{
					participant[i] = tmp;
					flag = 1;
					id[i] = counter;
					++counter;
					// printf("OK\n");
					sprintf(name[i], "%s", str);
					// printf("OK\n");
					// writen(participant[i], how_many, strlen(how_many));
					// printf("OK\n");
					break;
				}
			}
			Pthread_mutex_unlock(&(mutex[0]));
		}

		if (flag == 0)
		{
			if (room_status[1] == 0)
			{
				Pthread_mutex_lock(&(mutex[1]));

				for (int i = ROOM2; i < ROOM2 + 4; i++)
				{
					if (participant[i] == -1)
					{
						participant[i] = tmp;
						flag = 1;
						id[i] = counter;
						++counter;
						sprintf(name[i], "%s", str);
						// writen(participant[i], how_many, strlen(how_many));
						break;
					}
				}
				Pthread_mutex_unlock(&(mutex[1]));
			}
		}
		if (flag == 0)
		{
			if (room_status[2] == 0)
			{
				Pthread_mutex_lock(&(mutex[2]));

				for (int i = ROOM3; i < ROOM3 + 4; i++)
				{
					if (participant[i] == -1)
					{
						participant[i] = tmp;
						flag = 1;
						id[i] = counter;
						++counter;
						sprintf(name[i], "%s", str);
						// writen(participant[i], how_many, strlen(how_many));
						break;
					}
				}
				Pthread_mutex_unlock(&(mutex[2]));
			}
		}
		if (flag == 0)
		{
			if (room_status[3] == 0)
			{
				Pthread_mutex_lock(&(mutex[3]));
				for (int i = ROOM4; i < ROOM4 + 4; i++)
				{
					if (participant[i] == -1)
					{
						participant[i] = tmp;
						flag = 1;
						id[i] = counter;
						++counter;
						sprintf(name[i], "%s", str);
						// writen(participant[i], how_many, strlen(how_many));
						break;
					}
				}
				Pthread_mutex_unlock(&(mutex[3]));
			}
		}
		if (flag == 0)
		{
			sprintf(how_many, "sorry\n");
			printf("sorry\n");
			if (writen(tmp, how_many, strlen(how_many)) <= 0)
			{
				continue;
			}
			close(tmp);
		}
	}
	// printf("OK\n");
}

void *
room1(void *vptr)
{
	int room_num = (int)(long)vptr;
	const int ROOM = sep_room[room_num];

	fd_set fd;
	const char your_turn[200] = "flip\n";
	const char not_your_turn[200] = "don't_flip\n";
	char user_time[MAXLINE], mes[MAXLINE];
	int maxfdp1, people = 0, score[4] = {0}, num_ans, k, cards = 0, turn = 0, answer = 0, color, quit;
	double tmp_f;

	struct timeval tv, timeout;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;

	while (1)
	{
	re:
		cards = 1;
		answer = 0;
		turn = 3;

		for (int i = 0; i < 4; i++)
		{
			score[i] = 0;
		}

		while (1)
		{
			FD_ZERO(&fd);
			maxfdp1 = -1;
			people = 0;
			// flag = 0;
			Pthread_mutex_lock(&(mutex[room_num]));

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
				room_status[room_num] = 0;
				Pthread_mutex_unlock(&(mutex[room_num]));
				sleep(2);
			}
			else
			{
				tv.tv_sec = 0;
				tv.tv_usec = 0;
				Select(maxfdp1 + 1, &fd, NULL, NULL, &tv);
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1)
					{
						if (FD_ISSET(participant[i], &fd))
						{
							participant[i] = -1;
							people--;
						}
						else
						{
							if (people != 4)
							{
								char how_many[200];
								sprintf(how_many, "%d\n", people);
								if (writen(participant[i], how_many, strlen(how_many)) <= 0)
								{
									participant[i] = -1;
									people--;
								}
							}
						}
					}
				}

				if (people == 4)
				{
					const char four[200] = "4\n";
					// sprintf(how_many, "4\n");

					// card_num round name id score
					// char st[MAXLINE];
					// sprintf(st, "%s %s %s %s %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3]);
					// printf("%s\n", st);
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (writen(participant[i], four, strlen(four)) <= 0)
						{
							participant[i] = -1;
							people--;
							sprintf(name[i], "-");
							id[i] = 0;
						}
						// if (writen(participant[i], st, strlen(st)) <= 0)
						// {
						// 	participant[i] = -1;
						// 	people--;
						// }
					}

					if (people == 4)
					{
						room_status[room_num] = 1;
					}
					sleep(2);
					Pthread_mutex_unlock(&(mutex[room_num]));
					break;
				}
				else
				{
					room_status[room_num] = 0;
					Pthread_mutex_unlock(&(mutex[room_num]));
					sleep(2);
					// printf("OK\n");
				}
			}
		}

		while (1)
		{
			// printf("room1st\n");
			int win = 0;
			int who = -1;

			quit = 0;

			cards = rand() % 13;
			color = rand() % 4;
			answer = (answer + 1) % 13;

			int pre_turn = turn;
			maxfdp1 = -1;
			Pthread_mutex_lock(&(mutex[room_num]));
			char st[MAXLINE];
			sprintf(st, "%s %s %s %s %d %d %d %d %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3], score[0], score[1], score[2], score[3]);
			int people_flag = 0;
			printf("%s\n", st);
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] == -1)
				{
					people_flag = 1;
				}
				if (participant[i] != -1)
				{
					if (writen(participant[i], st, strlen(st)) <= 0)
					{
						participant[i] = -1;
					}
				}
			}
			if (people_flag == 1)
			{
				room_status[room_num] = 0;
			}
			else
			{
				room_status[room_num] = 1;
			}
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
				// room_status[room_num] = 0;d
				Pthread_mutex_unlock(&(mutex[room_num]));
				goto re;
			}

			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (i - ROOM != turn && participant[i] != -1)
				{
					if (writen(participant[i], not_your_turn, strlen(not_your_turn)) <= 0)
					{
						participant[i] = -1;
					}
				}
			}
			FD_ZERO(&fd);
			FD_SET(participant[turn], &fd);
			if (writen(participant[ROOM + turn], your_turn, strlen(your_turn)) <= 0)
			{
				participant[ROOM + turn] = -1;
			}
			num_ans = 0;

			timeout.tv_sec = 3;
			timeout.tv_usec = 200;
			num_ans = select(participant[ROOM + turn] + 1, &fd, NULL, NULL, &timeout);
			if (num_ans != 0)
			{
				if (readline(participant[ROOM + turn], user_time, MAXLINE) <= 0)
				{
					participant[ROOM + turn] = -1;
				}
			}

			maxfdp1 = -1;
			sprintf(mes, "%d %d %d\n", cards, color, answer);
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1)
				{
					if (writen(participant[i], mes, strlen(mes)) <= 0)
					{
						participant[i] = -1;
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
				if (participant[i] != -1)
				{
					FD_SET(participant[i], &fd);
				}
			}
			num_ans = 0;
			sleep(4);
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			num_ans = select(maxfdp1 + 1, &fd, NULL, NULL, &tv);
			// printf("num_ans: %d\n", num_ans);
			if (answer == cards)
			{
				if (num_ans != 0)
				{
					double time = 100000;

					who = -1;
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && FD_ISSET(participant[i], &fd))
						{
							if (readline(participant[i], user_time, MAXLINE) <= 0)
							{
								participant[i] = -1;
							}
							else
							{

								sscanf(user_time, "%lf", &tmp_f);
								if (tmp_f < time)
								{
									time = tmp_f;
									who = i;
								}
							}
							printf("accept: id:%d time%s", id[i], user_time);
						}
					}

					if (who != -1)
					{
						score[who - ROOM]++;
						if (score[who - ROOM] >= 3)
						{
							win = 1;
						}
					}
				}
			}
			else
			{

				if (num_ans != 0)
				{
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && FD_ISSET(participant[i], &fd))
						{
							if (readline(participant[i], user_time, MAXLINE) <= 0)
							{
								participant[i] = -1;
							}
							else
							{

								printf("wrong accept %s wrong\n", user_time);
								// score[i]--;

								score[i] = max(score[i] - 1, 0);
							}
						}
					}
				}
			}
			// 4 : continue
			// 1 : three left
			// 2 : someone win ,send name
			// char st[MAXLINE];
			quit = 0;
			for (int i = ROOM; i < ROOM + 4; i++)
			{

				if (participant[i] == -1)
				{
					score[i - ROOM] = 0;
					// participant[i] = -1;
					id[i] = 0;
					sprintf(name[i], "-");
					++quit;
				}
			}

			// sprintf(st, "%s %s %s %s %d %d %d %d %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3], score[0], score[1], score[2], score[3]);
			// for (int i = ROOM; i < ROOM + 4; i++)
			// {
			// 	if (participant[i] != -1)
			// 	{
			// 		if (writen(participant[i], st, strlen(st)) <= 0)
			// 		{
			// 			participant[i] = -1;
			// 			id[i] = 0;
			// 			sprintf(name[i], "-");
			// 			++quit;
			// 		}
			// 	}
			// }
			int flag = 0;
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (strcmp(name[i], "-") == 0)
				{
					flag++;
				}
			}
			if (flag >= 3)
			{
				// printf("situation1\n");
				// sprintf(st, "1\n");
				sprintf(st, "1\n%s %s %s %s %d %d %d %d %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3], score[0], score[1], score[2], score[3]);
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1)
					{
						if (writen(participant[i], st, strlen(st)) <= 0)
						{
							;
						}
						close(participant[i]);
					}
				}
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					participant[i] = -1;
					id[i] = 0;
					sprintf(name[i], "-");
					score[i - ROOM] = 0;
				}
				room_status[room_num] = 0;
				Pthread_mutex_unlock(&(mutex[room_num]));
				goto re;
			}
			else if (win == 1)
			{
				// printf("situation2\n");
				// sprintf(st, "2\n%s\n", name[who]);
				sprintf(st, "2\n%s\n%s %s %s %s %d %d %d %d %d %d %d %d\n", name[who], name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3], score[0], score[1], score[2], score[3]);
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1)
					{
						if (writen(participant[i], st, strlen(st)) <= 0)
						{
							;
						}
						close(participant[i]);
					}
				}
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					participant[i] = -1;
					id[i] = 0;
					score[i - ROOM] = 0;
					sprintf(name[i], "-");
				}
				room_status[room_num] = 0;
				Pthread_mutex_unlock(&(mutex[room_num]));
				goto re;
			}
			else
			{
				// printf("situation3\n");
				// sprintf(st, "4\n%s %s %s %s %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3]);
				sprintf(st, "4\n");
				int flag = 0;
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1)
					{
						if (writen(participant[i], st, strlen(st)) <= 0)
						{
							score[i - ROOM] = 0;
							participant[i] = -1;
							id[i] = 0;
							sprintf(name[i], "-");
						}
					}
					else
					{
						flag = 1;
					}
				}
				if (flag == 1)
				{
					room_status[room_num] = 0;
				}
			}

			Pthread_mutex_unlock(&(mutex[room_num]));
		}
	}

	return (NULL);
}