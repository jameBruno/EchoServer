#include "libDefine.h"

void signalHandlerServer(int nSignal);

int setNonblockSocket(int nFileDescriptor);

int g_nServerSocket = -1;					/* server socket file descriptor */
int g_nClientSocket = -1;
//int g_nClient[MAXCLIENT] = {0, };		/* client 관리 배열 */
//int g_nClientCount = 0;

//int g_nClientSocket[MAXCLIENT] = {0, };
//int nClientCount = 0;
//int g_nClientSocket = -1;							/* client socket file descriptor */

int main(int argc, char *argv[])
{
	int nPort = -1;
	char szMsg[SIZEBUFFER + 1] = {0,};			/* 수신용 버퍼 */
	int nReadLength = 0;								/* 받은 메시지 길이 저장 변수 */
	struct sockaddr_in stClientAddr = {0,};	/* 클라이언트 주소 저장할 구조체 */
	unsigned int uClientAddrSize = sizeof(stClientAddr);			/* 클라이언트 주소 구조체의 크기 */
	int nWriteLength = 0;
	struct epoll_event stEvent = {0, };
	struct epoll_event *pstEpollEvent = NULL;
	//struct epoll_event stEpollEvent[MAXCLIENT] = {0, };
	int nEpollFileDescriptor = -1;
	int nEventCnt = 0;
	int nIndex = 0;

	/* signal 처리 */
	signal(SIGINT, signalHandlerServer);
	
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	/* port 예외 처리 port 범위 */
	if(atoi(argv[1]) < 1024 || atoi(argv[1]) > 49151)
	{
		errHandle(errno, "port 사용하지 않는 포트입니다.\n");
		exit(1);
	}
	else
	{
		nPort = atoi(argv[1]);
	}
	
	printf("서버 연결 중입니다.\n");
	g_nServerSocket = serverSocket(nPort);

	/* server socket 예외 처리 */
	if(g_nServerSocket < 0)
	{
		errHandle(errno, "socket() error \n");
		exit(1);
	}

	nEpollFileDescriptor = epoll_create(EPOLLSIZE);

	if(nEpollFileDescriptor < 0)
	{
		errHandle(errno, "epoll file error \n");
		close(g_nServerSocket);
		g_nServerSocket = -1;
		exit(1);
	}

	/* calloc() 메모리 할당 및 초기화 */
	if((pstEpollEvent = calloc(EPOLLSIZE, sizeof(struct epoll_event))) == NULL)
	{
		errHandle(errno, "할당되지 않았습니다. \n");
		free(pstEpollEvent);
		pstEpollEvent = NULL;
		close(g_nServerSocket);
		g_nServerSocket = -1;
		exit(1);
		//close(nEpollFileDescriptor);
		//nEpollFileDescriptor = -1;
	}

	setNonblockSocket(g_nServerSocket);						/* server socket 에서 비동기 IO 설정 */
	stEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLET;	/* event 발생 시 알림 */
	stEvent.data.fd = g_nServerSocket;						/* server socket 추가 */

	/* server socket 상태 변화를 nEpollFileDescriptor 통해 알 수 있다. */
	/* 예외 처리 추가 */
	if(epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_ADD, g_nServerSocket, &stEvent) < 0)
	{
		errHandle(errno, "server socket 상태 변화를 nEpollFileDescriptor 통해 관찰할 수 없습니다.");
		close(nEpollFileDescriptor);
		nEpollFileDescriptor = -1;
		close(g_nServerSocket);
		g_nServerSocket = -1;
		exit(1);
	}

	while(1)
	{
		/* nEpollFileDescriptor 의 사건 발생 시 pstEpollEvent 에 fd 채운다. */
		nEventCnt = epoll_wait(nEpollFileDescriptor, pstEpollEvent, EPOLLSIZE, -1);

		/* neventCnt 예외 처리 */
		if(nEventCnt < 0)
		{
			errHandle(errno, "epoll wait error");
			break;
		}
		else if(nEventCnt == 0)
		{
			break;
		}

		/* event 발생 횟수 출력 */
		printf("return epoll 발생 \n");

		for(nIndex = 0; nIndex < nEventCnt; nIndex++)
		{
			if(pstEpollEvent[nIndex].data.fd == g_nServerSocket)
			{
				g_nClientSocket = accept(g_nServerSocket, (struct sockaddr*) &stClientAddr, &uClientAddrSize);

				//g_nClient[g_nClientCount++] = g_nClientSocket;

				/* client socket 예외 처리 client 배열 */
				if(g_nClientSocket < 0)
				{
					errHandle(errno, "accept error");
					continue;
				}
				else
				{
					printf("클라이언트 접속 IP : %s\n", inet_ntoa(stClientAddr.sin_addr));
					printf("accept() client 접속 요청 수락");
				}

				setNonblockSocket(g_nClientSocket);		/* client 소켓 넌블로킹으로 */
				stEvent = {0, };
				stEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLET;	/* event */
				//stEvent.events = EPOLLIN | EPOLLET;
				stEvent.data.fd = g_nClientSocket;						/* client socket */
				/* epoll_ctl() 로 pstEpollEvent 상태 변화를 nEpollFileDescriptor로 확인 */
				if(epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_ADD, g_nClientSocket, &stEvent) < 0)
				{
					errHandle(errno, "client socket 상태 변화가 되지 않았습니다.");
					close(g_nClientSocket);
					g_nClientSocket = -1;
					continue;
				}
				printf("연결된 클라이언트 : %d\n", g_nClientSocket);
			}
			else
			{
				//nReadLength = read(pstEpollEvent[nIndex].data.fd, szMsg, SIZEBUFFER);
				nReadLength = read(pstEpollEvent[nIndex].data.fd, szMsg, SIZEBUFFER);

				printf("받은 메시지 %s\n", szMsg);

				if(nReadLength == 0)
				{
					epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, &stEvent);
					close(pstEpollEvent[nIndex].data.fd);
					close(g_nClientSocket);
					g_nClientSocket = -1;
					printf("연결 종료된 클라이언트 %d\n", pstEpollEvent[nIndex].data.fd);
					continue;
					//break;
				}
				else if(nReadLength < 0)
				{
					errHandle(errno, "read error \n");

					errHandle(errno, "error \n");
					epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, &stEvent);
					close(pstEpollEvent[nIndex].data.fd);
					close(g_nClientSocket);
					g_nClientSocket = -1;
					continue;

					/*
					if(errno != EAGAIN || errno == EAGAIN)
					{
						errHandle(errno, "error \n");
						epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, &stEvent);
						close(pstEpollEvent[nIndex].data.fd);
						close(g_nClientSocket);
						continue;
					}
					*/

					/*
					if(errno != EAGAIN)			/* EAGAIN 제외한 예외 처리 추가 */
					/*
					{
						errHandle(errno, "error \n");
						epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, &stEvent);
						close(pstEpollEvent[nIndex].data.fd);
						continue;
					}
					else if(errno == EAGAIN)	/* EAGAIN 예외 처리 */
//					{

					/*
						errHandle(errno, "error \n");
						epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, &stEvent);
						close(pstEpollEvent[nIndex].data.fd);
						continue;
					}
					*/
					/* 에러 예외 처리 추가 error */
					/*
					if(errno == EAGAIN || errno == EINVAL || errno == EBADF || errno == EEXIST || errno == ELOOP || 
						errno == ENOENT || errno == ENOMEM || errno == ENOSPC || errno == EPERM)	/* error 예외 처리 */
					/*
					{
						errHandle(errno, "error \n");
						epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, &stEvent);
						//epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, &stEvent);
						//close(nEpollFileDescriptor);
						//nEpollFileDescriptor = -1;
						close(pstEpollEvent[nIndex].data.fd);
						break;
					}
					*/
				}
				else
				{
					for(nIndex=0; nIndex < g_nClientSocket; nIndex++)//클라이언트 개수만큼
					{
						nWriteLength = write(pstEpollEvent[nIndex].data.fd, szMsg, nReadLength);		//클라이언트들에게 메시지를 전달한다.
					}

					if(nWriteLength < 0)
					{
						errHandle(errno, "write error \n");
						epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, &stEvent);
						//close(nEpollFileDescriptor);
						//nEpollFileDescriptor = -1;
						close(pstEpollEvent[nIndex].data.fd);
						close(g_nClientSocket);
						g_nClientSocket = -1;
						continue;
					}
					memset(szMsg, 0, sizeof(szMsg));
				}
			}
			/*
			for(nIndex = 0; nIndex < g_nClientCount; nIndex++)			//배열의 갯수만큼
			{
				if(g_nClientSocket == g_nClient[nIndex])	//만약 현재 clientSock값이 배열의 값과 같다면
				{
					while(nIndex++ < g_nClientCount - 1)				//클라이언트 개수 만큼
					{
						g_nClient[nIndex] = g_nClient[nIndex + 1];	//앞으로 땡긴다.
					}
					continue;
				}
			}
			*/
		}
	}

	free(pstEpollEvent);
	pstEpollEvent = NULL;
	close(nEpollFileDescriptor);
	nEpollFileDescriptor = -1;
	//g_nClientCount--;				//클라이언트 개수 하나 감소
	//close(g_nClientSocket);
	//g_nClientSocket = -1;
	close(g_nServerSocket);
	g_nServerSocket = -1;
	printf("연결이 끊어졌습니다. \n");
	return 0;
}

void signalHandlerServer(int nSignal)
{
	char szMessage[SIZEBUFFER] = {0,};

	if (nSignal == SIGINT)
	{
		fputs("종료하시겠습니까?(y 입력) : ", stdout);
		fgets(szMessage, SIZEBUFFER, stdin);
		if(!strcasecmp(szMessage, "y\n"))
		{
			if(g_nClientSocket >= 0)
			{
				close(g_nClientSocket);
				//close(g_nClientSocket[MAXCLIENT]);
				g_nClientSocket = -1;
				printf("client socket 종료되었습니다.\n");
			}

			if(g_nServerSocket >= 0)
			{
				close(g_nServerSocket);
				g_nServerSocket = -1;
				printf("server socket 종료되었습니다.\n");
			}
			exit(0);
		}
	}
}

int setNonblockSocket(int nFileDescriptor)
{
	int nFlag = fcntl(nFileDescriptor, F_GETFL, 0);

	/* nFlag 예외 처리 */
	if(nFlag < 0)
	{
		errHandle(errno, "flag error");
		exit(1);
	}

	fcntl(nFileDescriptor, F_SETFL, nFlag | O_NONBLOCK);

	return 0;
}

