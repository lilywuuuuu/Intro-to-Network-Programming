include ../Make.defines

PROGS =	tcpcli01 tcpcli04 tcpcli05 tcpcli06 \
		tcpcli07 tcpcli08 tcpcli09 tcpcli10 \
		tcpserv01 tcpserv02 tcpserv03 tcpserv04 \
		tcpserv08 tcpserv09 tcpservselect01 tcpservpoll01 tsigpipe \
		hw1cli hw1serv hw2cli hw2serv hw3cli hw3serv hw4cli hw4serv \
		finalcli finalserv

all:	${PROGS}

finalcli: 	finalcli.o
		${CC} ${CFLAGS} -o $@ finalcli.o ${LIBS} -lncurses

finalserv: finalserv.o
		${CC} ${CFLAGS} -o $@ finalserv.o ${LIBS}

hw1cli: 	hw1cli.o
		${CC} ${CFLAGS} -o $@ hw1cli.o ${LIBS}

hw1serv: 	hw1serv.o
		${CC} ${CFLAGS} -o $@ hw1serv.o ${LIBS}

hw2cli: 	hw2cli.o
		${CC} ${CFLAGS} -o $@ hw2cli.o ${LIBS}

hw2serv: 	hw2serv.o
		${CC} ${CFLAGS} -o $@ hw2serv.o ${LIBS}

hw3cli: 	hw3cli.o
		${CC} ${CFLAGS} -o $@ hw3cli.o ${LIBS}

hw3serv: 	hw3serv.o
		${CC} ${CFLAGS} -o $@ hw3serv.o ${LIBS}

hw4cli: 	hw4cli.o
		${CC} ${CFLAGS} -o $@ hw4cli.o ${LIBS}

hw4serv: 	hw4serv.o
		${CC} ${CFLAGS} -o $@ hw4serv.o ${LIBS}

hw5cli: 	hw5cli.o
		${CC} ${CFLAGS} -o $@ hw5cli.o ${LIBS}

hw5serv: 	hw5serv.o
		${CC} ${CFLAGS} -o $@ hw5serv.o ${LIBS}

tcpcli01:	tcpcli01.o
		${CC} ${CFLAGS} -o $@ tcpcli01.o ${LIBS}

tcpcli04:	tcpcli04.o
		${CC} ${CFLAGS} -o $@ tcpcli04.o ${LIBS}

tcpcli05:	tcpcli05.o
		${CC} ${CFLAGS} -o $@ tcpcli05.o ${LIBS}

tcpcli06:	tcpcli06.o
		${CC} ${CFLAGS} -o $@ tcpcli06.o ${LIBS}

tcpcli07:	tcpcli07.o
		${CC} ${CFLAGS} -o $@ tcpcli07.o ${LIBS}

tcpcli08:	tcpcli08.o str_cli08.o
		${CC} ${CFLAGS} -o $@ tcpcli08.o str_cli08.o ${LIBS}

tcpcli09:	tcpcli09.o str_cli09.o
		${CC} ${CFLAGS} -o $@ tcpcli09.o str_cli09.o ${LIBS}

tcpcli10:	tcpcli10.o
		${CC} ${CFLAGS} -o $@ tcpcli10.o ${LIBS}

tcpcli11:	tcpcli11.o str_cli11.o
		${CC} ${CFLAGS} -o $@ tcpcli11.o str_cli11.o ${LIBS}

tcpserv01:	tcpserv01.o
		${CC} ${CFLAGS} -o $@ tcpserv01.o ${LIBS}

tcpserv02:	tcpserv02.o sigchldwait.o
		${CC} ${CFLAGS} -o $@ tcpserv02.o sigchldwait.o ${LIBS}

tcpserv03:	tcpserv03.o sigchldwait.o
		${CC} ${CFLAGS} -o $@ tcpserv03.o sigchldwait.o ${LIBS}

tcpserv04:	tcpserv04.o sigchldwaitpid.o
		${CC} ${CFLAGS} -o $@ tcpserv04.o sigchldwaitpid.o ${LIBS}

tcpserv08:	tcpserv08.o str_echo08.o sigchldwaitpid.o
		${CC} ${CFLAGS} -o $@ tcpserv08.o str_echo08.o sigchldwaitpid.o \
			${LIBS}

tcpserv09:	tcpserv09.o str_echo09.o sigchldwaitpid.o
		${CC} ${CFLAGS} -o $@ tcpserv09.o str_echo09.o sigchldwaitpid.o \
			${LIBS}

tcpservselect01:	tcpservselect01.o
		${CC} ${CFLAGS} -o $@ tcpservselect01.o ${LIBS}

tcpservpoll01:	tcpservpoll01.o
		${CC} ${CFLAGS} -o $@ tcpservpoll01.o ${LIBS}

tsigpipe:	tsigpipe.o
		${CC} ${CFLAGS} -o $@ tsigpipe.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
