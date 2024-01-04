#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_ID_LEN 10

static void sig_alarm(int signo)
{
  return;
}

void get_udp_client(int sockfd, FILE *fp)
{
    int	n, nconnt, i, j, clifd[11], result;
	int     x, y, TCP_servport, local_port, port_no;
	socklen_t clilen, clilen2, servlen, templen; 
	struct sockaddr_in	servaddr, cliaddr, cliaddr2, tempaddr;
	char recvline[MAXLINE], buff[MAXLINE], TCP_servaddr[MAXLINE], TCP_addr2[MAXLINE];
	char student_id[MAX_ID_LEN];	
	FILE *cli_fp;	
	time_t ticks;
    
    /* get requests from UDP clients */
    clilen = sizeof(cliaddr);
	n = Recvfrom(sockfd, recvline, MAXLINE, 0, (SA *) &cliaddr, &clilen);
	recvline[n] = 0;
    ticks = time(NULL);
    Inet_ntop(AF_INET, &cliaddr.sin_addr, TCP_servaddr, MAXLINE);
    fprintf (fp, "\n[%.24s] recvfrom %s, port %d: %s\n",
        ctime(&ticks), TCP_servaddr, ntohs(cliaddr.sin_port), recvline);
    
    if (sscanf(recvline, "%s %d", student_id, &TCP_servport) != 2) {
        fprintf(fp, "Missing parameters (expected two)\n"); 
        return;
    } else {
        fprintf(fp, "Student ID = %s, TCP Serv addr = %s, TCP Serv port = %d\n", 
        student_id, TCP_servaddr, TCP_servport);
        snprintf(buff, sizeof(buff), "%s.log", student_id);
        if ((cli_fp = fopen(buff, "a")) == NULL) {
            fprintf(fp, "log file open error: %s\n", buff);
//        		exit(0);
        } 
    };   
    /* determine the number of TCP connections to create */	           
    srand((int) ticks);
    nconnt = rand() % 10 + 1;
    
    /* reply to the UDP client the student ID, the IP address of the TCP server, and the number of TCP connections to be created */ 
    snprintf(buff, sizeof(buff), "%s %s %d", student_id, TCP_servaddr, nconnt);
    Sendto(sockfd, buff, strlen(buff), 0, (SA *) &cliaddr, clilen);
    if (cli_fp != NULL){
        fprintf(cli_fp, "replied: %s\n", buff);
        fprintf(fp, "replied: %s\n", buff);
    }
    else
        fprintf(fp, "replied: %s\n", buff);
    alarm(5);
    
    clilen2 = sizeof(cliaddr2);
    if ((n = recvfrom(sockfd, recvline, MAXLINE, 0, (SA *) &cliaddr2, &clilen2)) < 0) {
        if (errno == EINTR) {
        if (cli_fp != NULL)
            fprintf(cli_fp, "recvfrom timeout\n");
        else 
            fprintf(fp, "recvfrom timeout\n");
        } else {
            if (cli_fp != NULL) 
            fprintf(cli_fp, "recvfrom error\n");
            else
            fprintf(fp, "recvfrom error\n");
        }
        if (cli_fp != NULL) 
            fflush(cli_fp);
        else
            fflush(fp); 	
        return;
    }
    else {
        alarm(0);
        Inet_ntop(AF_INET, &cliaddr2.sin_addr, TCP_addr2, MAXLINE);
        if (strcmp(TCP_servaddr, TCP_addr2) != 0) {
            sprintf(buff, "conflict");
//              Sendto(sockfd, buff, strlen(buff), 0, (SA *) &cliaddr, clilen);
            Sendto(sockfd, buff, strlen(buff), 0, (SA *) &cliaddr2, clilen2);
            if (cli_fp != NULL) {
            fprintf(cli_fp, "conflict. expect %s but receive %s\n", TCP_servaddr, TCP_addr2);
            fflush(cli_fp);				  
            }
            else { 			  
            fprintf(fp, "conflict. expect %s but receive %s\n", TCP_servaddr, TCP_addr2);
            fflush(fp);
            }
            return;
        }
        else {
            recvline[n] = 0;
            sscanf(recvline, "%d", &result);  
            if (result == 0) {  // from the same source, another program
                sprintf(buff, "conflict");
                Sendto(sockfd, buff, strlen(buff), 0, (SA *) &cliaddr2, clilen2);
                if (cli_fp != NULL) {
                fprintf(cli_fp, "conflict. expect code but receive %s\n", recvline);
                fflush(cli_fp);
                }
                else {
                fprintf(fp, "conflict. expect code but receive %s\n", recvline);
                fflush(fp);
                }
                return;
            } 
            else  if (result != 13) {
                if (cli_fp != NULL) {
                fprintf(cli_fp, "got code %d, quit!\n", result);
                fflush(cli_fp);					  
                }
                else {
                fprintf(fp, "got code %d, quit!\n", result);
                fflush(fp);
                }
            return;                 
            };
            if (cli_fp != NULL)
            fprintf(cli_fp, "got code 13, proceed\n");  
            else 
            fprintf(fp, "got code 13, proceed\n");
        }
    } 
      /* prepare address structure for the TCP server */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(TCP_servport);
	Inet_pton(AF_INET, TCP_servaddr, &servaddr.sin_addr);	
        
    fprintf(fp, "Creating %d connections...\n", nconnt);        
    /* create multiple file descriptors for client sockets */  
    for (i = 0; i < nconnt; i++)
            clifd[i] = Socket(AF_INET, SOCK_STREAM, 0);   
            
    /* create multiple TCP connections to the same TCP server */  
    servlen = sizeof(servaddr);        
    for (i = 0; i < nconnt; i++) {
            if (connect(clifd[i], (SA *) &servaddr, servlen) < 0) {
            if (cli_fp != NULL) {
                fprintf(cli_fp, "connection %d: connect error!\n", i);
                fflush(cli_fp);
            }
            else {
                fprintf(fp, "connection %d: connect error!\n", i);
                fflush(fp);
            };					
            for (j = 0; j < i; j++)
                    close(clifd[j]);
            ticks = time(NULL);
                    
            Sendto(sockfd, "nak", 3, 0, (SA *) &cliaddr, clilen);
            if (cli_fp != NULL) {
                fprintf (cli_fp, "[%.24s] sendto %s, port %d: ",
                    ctime(&ticks), TCP_servaddr, ntohs(cliaddr.sin_port)); 
                fprintf(cli_fp, "nak.\n");
                fflush(cli_fp);					
            }
            fprintf (fp, "[%.24s] sendto %s, port %d: ",
                ctime(&ticks), TCP_servaddr, ntohs(cliaddr.sin_port)); 
            fprintf(fp, "nak.\n");
            fflush(fp);
            return;     
            };
    };     
    /* generate random numbers and send them on these TCP connections */
    
    result = 1;
    templen = sizeof(tempaddr);
    for (i = 0; i < nconnt; i++) {
            x = rand() % 1024 + 1;
            snprintf(buff, sizeof(buff), "%d\n", x);	
            Writen(clifd[i], buff, strlen(buff));

            /* get local port no */
            getsockname(clifd[i], (SA *) &tempaddr, &templen);
            local_port = ntohs(tempaddr.sin_port); 
            if (cli_fp != NULL)
            fprintf (cli_fp, "Connection %d (port #%d): written %s",
                i, local_port, buff); 
            else
            fprintf (fp, "Connection %d (port #%d): written %s",
                i, local_port, buff); 
            if ((n = Readline(clifd[i], recvline, MAXLINE)) == 0) {
                if (cli_fp != NULL)
                    fprintf(cli_fp, "no data received. quitting\n");
                else
                    fprintf(fp, "no data received. quitting\n");
                result = 0;
                continue;
            };   
            recvline[n] = 0;  
            sscanf(recvline, "%d %d", &port_no, &y); 
        
            if (port_no == local_port && x == y) {
                Writen(clifd[i], "ok\n", 3);
                if (cli_fp != NULL)
                    fprintf(cli_fp, "port and value match, ok sent!\n");
                else
                    fprintf(fp, "port and value match, ok sent!\n");
            }              
            else {
                Writen(clifd[i], "nak\n", 4);
                if (cli_fp != NULL) {
                    fprintf(cli_fp, "recv: %s", recvline);             	
                    fprintf(cli_fp, "port or value not match, nak sent!\n");					
            }
            else { 
                fprintf(fp, "recv: %s", recvline);             	
                fprintf(fp, "port or value not match, nak sent!\n");
            }
            result = 0;
            };
    };  
    
    /* close all TCP connections */          
    for (i = 0; i < nconnt; i++)
            close(clifd[i]);       
    
    ticks = time(NULL);
    if (cli_fp != NULL)
        fprintf (cli_fp, "[%.24s] sendto %s, port %d: ",
            ctime(&ticks), TCP_servaddr, ntohs(cliaddr.sin_port));			
    fprintf (fp, "[%.24s] sendto %s, port %d: ",
            ctime(&ticks), TCP_servaddr, ntohs(cliaddr.sin_port));
    if (result == 1) {
        Sendto(sockfd, "ok", 2, 0, (SA *) &cliaddr, clilen);
        if (cli_fp != NULL)
            fprintf(cli_fp, "ok.\n"); 
        fprintf(fp, "ok.\n");       	
    }
    else {
        Sendto(sockfd, "nak", 3, 0, (SA *) &cliaddr, clilen);
        if (cli_fp != NULL)
            fprintf(cli_fp, "nak.\n");
        fprintf(fp, "nak.\n");
    };
    if (cli_fp != NULL)
        fclose(cli_fp);
    fflush(fp); 	
}

int
main(int argc, char **argv)
{
	int	sockfd;
	struct sockaddr_in	servaddr;
	FILE *fp;

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	
      /* prepare address structure for this UDP server */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT+2);

	Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));
 
    if ((fp = fopen("hw3serv.log", "a")) == NULL) {
        printf("log file open error!\n");
        exit(0);
    };
    Signal(SIGALRM, sig_alarm);
    
    do {
        get_udp_client(sockfd, fp);
    } while (1);
                
}