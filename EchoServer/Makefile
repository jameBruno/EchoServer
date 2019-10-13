.SUFFIXES : .o .c

CC = gcc

CFLAGS = -g -c

OBJS = libDefine.o echoservers.o echoclients.o

LIBS = -lprint

all: echoservers echoclients

libDefine.o: libDefine.c
	$(CC) $(CFLAGS) -c libDefine.c

echoservers.o: echoservers.c
	$(CC) $(CFLAGS) -c echoservers.c

echoclients.o: echoclients.c
	$(CC) $(CFLAGS) -c echoclients.c

echoservers: echoservers.o libDefine.o
	$(CC) -o $@ echoservers.o libDefine.o

echoclients: echoclients.o libDefine.o
	$(CC) -o $@ echoclients.o libDefine.o

clean:
	rm -rf $(OBJS) echoservers echoclients *.o *.core

