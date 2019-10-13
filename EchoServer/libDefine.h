#ifndef LIBDEFINE_H_
#define LIBDEFINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>

#define SIZEBUFFER	1024
#define LISTENQ		10
#define MESSAGE		1000
#define EPOLLSIZE		100
#define MAXCLIENT		100

/* 서버 socket */
int serverSocket(int nPort);

/* 클라이언트 */
int clientSocket(int nPort, char *pszAddr);

/* 에러 예외 처리 */
void errHandle(int nErrorCode, char *pszMsg, ...);

#endif

