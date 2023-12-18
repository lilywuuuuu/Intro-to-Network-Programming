#include "unp.h"
#include "unpthread.h"
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
int participant[16];
int id[16];
char name[16][len_name];
void *doit(void *);
const char start[] = "start\n";

int main(int argc, char **argv)
{
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
		writen(tmp, waiting, sizeof(waiting));
	}
}

void *
room1(void *vptr)
{
	fd_set fd;
	int maxfdp1, flag = 0, people = 0, round = 0, score[4] = 0, num_ans, k, cards = 0, turn = 0;

	struct timeval tv, timeout;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	while (1)
	{
	re:
		while (1)
		{
			FD_ZERO(&fd);
			maxfdp1 = -1;
			people = 0;
			flag = 0;
			Pthread_mutex_lock(&mutex1);

			for (int i = 0; i < 4; i++)
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

				Select(maxfdp1 + 1, &fd, NULL, NULL, &tv);
				for (int i = 0; i < 4; i++)
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
		for (int i = 0; i < 4; i++)
		{
			writen(participant[i], start, sizeof(start));
		}

		while (1)
		{

			maxfdp1 = -1;
			Pthread_mutex_lock(&mutex1);
			for (k = 1; k < 4; k++)
			{

				if (participant[(ROOM1 + (turn + k) % 4)] == -1)
				{
					turn++;
					turn %= 4;
					break;
				}
				else
				{
					maxfdp1 = max(maxfdp1, participant[k]);
				}
			}

			if (maxfdp1 == -1)
			{
				Pthread_mutex_unlock(&mutex1);
				goto re;
			}
			for (; k < 4; k++)
			{
				maxfdp1 = max(maxfdp1, participant[k]);
			}
			Pthread_mutex_unlock(&mutex1);
			
			num_ans = 0;
			num_ans = select(maxfdp1 + 1, &fd, NULL, NULL, &timeout);
			if (num_ans == 0)
			{
			}
		}
	}

	return (NULL);
}
