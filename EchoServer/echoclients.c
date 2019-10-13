#include "libDefine.h"

void signalHandlerClient(int nSignal);

int g_nClientSocket = -1;
//int g_nClientSocket[MAXCLIENT] = {0, };
//int g_nClientCount = 0;

//int g_nClientSocket[EPOLLSIZE] = {0, };

int main(int argc, char *argv[])
{
	int nPort = -1;
	char *pszAddr = NULL;					/* ip 문자 입력받을 포인터 변수 */
	char szMsg[SIZEBUFFER + 1] = {0,};	/* 서버에 보낼 메시지를 저장할 문자열 버퍼 */
	int nReadLength = 0;						/* 송수신 메시지의 문자열 길이 */
	int nWriteLength = 0;
	//int nEpollFileDescriptor = -1;
	//struct epoll_event *pstEpollEvent = {0, };
	//struct epoll_event stEpollEvent[EPOLLSIZE] = {0, };
	//struct epoll_event stEvent = {0, };
	//int nIndex = 0;
	//int nReady = 0;

	signal(SIGINT, signalHandlerClient);

	/* port */
	if (argc != 3)
	{
		printf("usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	}

	pszAddr = argv[1];

	/* port 사용 범위 예외 처리 */
	if(atoi(argv[2]) < 1024 || atoi(argv[2]) > 49151)
	{
		errHandle(errno, "port incorrect \n");
		exit(1);
	}
	else
	{
		nPort = atoi(argv[2]);
	}

	g_nClientSocket = clientSocket(nPort, pszAddr);

	if(g_nClientSocket < 0)
	{
		errHandle(errno, "socket() error \n");
		exit(1);
	}

	while (1)
	{
		memset(szMsg, 0, sizeof(szMsg));

		fputs("입력 메시지(Q는 종료, 버퍼(1024)까지 입력할 수 있습니다.) : ", stdout);
		fgets(szMsg, sizeof(szMsg), stdin);

		//if(fgets(szMsg, sizeof(szMsg), stdin) == )

		//szMsg[strlen(szMsg) - 1] = '\0';

		/*
		if(fgets(szMsg, sizeof(szMsg), stdin) == NULL)
		{
			fputs("입력 메시지 ", stdout);
			fgets(szMsg, sizeof(szMsg), stdin);
			fputs("입력 ", stdout);
		}
		*/

		if(!strcasecmp(szMsg, "q\n"))
		{
			close(g_nClientSocket);
			g_nClientSocket = -1;
			break;
		}

		if((nWriteLength = write(g_nClientSocket, szMsg, strlen(szMsg))) < 0)
		{
			errHandle(errno, "write error \n");
			close(g_nClientSocket);
			g_nClientSocket = -1;
			exit(1);
		}

		/*---Wait for data---*/

		/*
		nReady = epoll_wait(nEpollFileDescriptor, pstEpollEvent, EPOLLSIZE, 1000);

		for(nIndex = 0; nIndex < nReady; nIndex++)
		{
			if(pstEpollEvent[nIndex].events & EPOLLIN)
			{
				printf("data %d\n", pstEpollEvent[nIndex].data.fd);
				memset(szMsg, 0, sizeof(szMsg));
				read(g_nClientSocket, szMsg, sizeof(szMsg));
				printf("Received %s\n", szMsg);
			}
			else if(pstEpollEvent[nIndex].events & EPOLLRDHUP)
			{
				printf("connect close %d\n", pstEpollEvent[nIndex].data.fd);
				epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_DEL, pstEpollEvent[nIndex].data.fd, NULL); // 종료된 클라이언트들의 소켓 디스크립터
				close(pstEpollEvent[nIndex].data.fd);
				printf("closed client: %d \n", pstEpollEvent[nIndex].data.fd);
			}
		}
		*/

		//num_ready = epoll_wait(epfd, events, MAX_EPOLL_EVENTS, 1000/*timeout*/);

		memset(szMsg, 0, sizeof(szMsg));

		if((nReadLength = read(g_nClientSocket, szMsg, nWriteLength)) < 0)
		{
			errHandle(errno, "read error \n");
			close(g_nClientSocket);
			g_nClientSocket = -1;
			exit(1);
		}

		szMsg[nReadLength] = 0;
		printf("서버로 받은 메시지 : %s\n", szMsg);
	}
	return 0;
}

void signalHandlerClient(int nSignal)
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
				g_nClientSocket = -1;
				printf("연결 종료되었습니다.\n");
			}
			exit(0);
		}
	}
}


