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
char name[16][len_name];
void *doit(void *);
const char start[] = "start\n";
const char no_one[] = "no\n";
const char nobody[] = "empty\n";
int main(int argc, char **argv)
{
	srand(time(NULL));
	int listenfd, connfd, tmp, flag, i;
	char str[len_name];
	const char sorry[] = "sorry\n";
	const char waiting[] = "waiting\n";
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

	for (int i = 0; i < 16; i++)
	{
		participant[i] = -1;
	}
	// Pthread_create(&tidA, NULL, &doit, NULL);
	// Pthread_create(&tidB, NULL, &doit, NULL);
	for (;;)
	{

		clilen = sizeof(cliaddr);
		tmp = Accept(listenfd, (SA *)&cliaddr, &clilen);
		readline(tmp, str, sizeof(str));
		flag = 0;
		writen(tmp, waiting, sizeof(waiting));
		Pthread_mutex_lock(&mutex1);

		for (i = ROOM1; i < ROOM1 + 4; i++)
		{
			if (participant[i] == -1)
			{
				participant[i] = tmp;
				flag = 1;
				id[i] = counter;
				++counter;
				strcpy(name[i], str);
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
					strcpy(name[i], str);
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
					strcpy(name[i], str);
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
					strcpy(name[i], str);
					break;
				}
			}
			Pthread_mutex_unlock(&mutex4);
		}
		if (flag == 0)
		{
			writen(tmp, sorry, sizeof(sorry));
			close(tmp);
		}
	}
}

void *
room1(void *vptr)
{
	const int ROOM = 0;
	fd_set fd;
	const char your_turn[] = "flip\n";
	const char not_your_turn[] = "don't_flip\n";
	char user_time[100], mes[100];
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
		cards = 0;
		answer = 0;
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
				sleep(2);
			}
			else
			{
				tv.tv_sec = 0;
				tv.tv_usec = 0;
				Select(maxfdp1 + 1, &fd, NULL, NULL, &tv);
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1 && FD_ISSET(participant[i], &fd))
					{
						participant[i] = -1;
						people--;
					}
				}

				Pthread_mutex_unlock(&mutex1);
				if (people == 4)
				{
					break;
				}
				else
				{
					sleep(2);
				}
			}
		}

		while (1)
		{
			for (int i = 0; i < 4; i++)
			{
				who_quit[i] = 0;
			}
			quit = 0;

			cards = rand() % 13 + 1;
			color = rand() % 4;
			answer++;

			if (answer == 14)
			{
				answer = 1;
			}
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
				int who = -1;
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
							sscanf(user_time, "%f", &tmp_f);
							if (tmp_f < time)
							{
								who = i;
							}
						}
					}
				}
				char left[200] = "";
				if (quit != 0)
				{
					for (int i = 0; i < 4; i++)
					{
						if (who_quit[i] == 1)
						{
							strcat(left, " ");
							strcat(left, name[i + ROOM]);
						}
					}
				}
				else
				{
					strcat(left, "nobody");
				}
				strcat(left, " has left.\n");

				if (quit >= 3)
				{
					strcat(left, " You won!\n");
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && who_quit[i - ROOM] != 1)
						{

							writen(participant[i], left, strlen(left));
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
				if (who != -1)
				{
					score[who - ROOM]++;
					if (score[who - ROOM] >= 10)
					{

						strcat(left, name[who]);
						strcat(left, " won\n");

						for (int i = ROOM; i < ROOM + 4; i++)
						{
							if (participant[i] != -1 && who_quit[i - ROOM] != 1)
							{

								writen(participant[i], left, strlen(left));
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
				}
				else
				{
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && who_quit[i - ROOM] != 1)
						{

							writen(participant[i], left, strlen(left));
						}
						else if (who_quit[i - ROOM] == 1)
						{
							score[i - ROOM] = 0;
							participant[i] = -1;
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

				char left[200];
				if (quit != 0)
				{
					for (int i = 0; i < 4; i++)
					{
						if (who_quit[i] == 1)
						{
							strcat(left, " ");
							strcat(left, name[i + ROOM]);
						}
					}
				}
				else
				{
					strcat(left, "nobody");
				}
				strcat(left, " has left.\n");
				if (quit >= 3)
				{
					strcat(left, " You won!\n");
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && who_quit[i - ROOM] != 1)
						{

							writen(participant[i], left, strlen(left));

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
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (participant[i] != -1 && who_quit[i - ROOM] != 1)
						{

							writen(participant[i], left, strlen(left));
							// score?
						}
					}
				}
			}
			Pthread_mutex_unlock(&mutex1);
		}
	}

	return (NULL);
}
