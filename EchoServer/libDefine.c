#include "libDefine.h"

int serverSocket(int nPort)
{
	int nServerSocket = -1;
	struct sockaddr_in stServerAddr = {0,};	/* 서버 주소 저장할 구조체  */
	int nOptval = 0;

	if(nPort < 0)
	{
		errHandle(errno, "port error");
		return -1;
	}

	/* TCP 통신용 서버 소켓 생성 */
	nServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (nServerSocket < 0)
	{
		errHandle(errno, "socket() error");
		return -1;
	}
	else
	{
		printf("socket() TCP 통신용 서버 소켓이 생성되었습니다.\n");
	}

	if(setsockopt(nServerSocket, SOL_SOCKET, SO_REUSEADDR, (const void *)&nOptval, sizeof(int)) < 0)
	{
		errHandle(errno, "socket option error");
		close(nServerSocket);
		nServerSocket = -1;
		return -1;
	}

	memset(&stServerAddr, 0, sizeof(stServerAddr));
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	stServerAddr.sin_port = htons(nPort);

	/* bind */
	if (bind(nServerSocket, (struct sockaddr*) &stServerAddr, sizeof(stServerAddr)) < 0)
	{
		errHandle(errno, "bind() error %d\n", nPort);
		close(nServerSocket);
		nServerSocket = -1;
		return -1;
	}
	else
	{
		printf("bind() - socket에 주소와 IP 할당되었습니다.\n");
	}

	/* 소켓을 서버용으로 사용할 수가 있게 한다. */
	if (listen(nServerSocket, LISTENQ) < 0)
	{
		errHandle(errno, "listen() error %d\n", nPort);
		close(nServerSocket);
		nServerSocket = -1;
		return -1;
	}
	else
	{
		printf("listen() 연결 요청 대기하는 중입니다.\n");
	}

	return nServerSocket;
}

int clientSocket(int nPort, char *pszAddr)
{
	int nClientSocket = -1;
	struct sockaddr_in stServerAddr = {0,};			/* 접속할 서버의 주소 저장할 구조체 */
	//struct epoll_event stEvent[EPOLLSIZE] = {0,};
	//int nEpollFileDescriptor = -1;
	//struct epoll_event *pstEpollEvent = NULL;
	//struct epoll_event *pstEpollEvent[EPOLLSIZE] = {0, };
	//struct epoll_event stEvent = {0, };
	//int nIndex = 0;
	//int nReady = 0;

	if(nPort < 0)
	{
		errHandle(errno, "port error");
		return -1;
	}

	if(pszAddr == NULL)
	{
		errHandle(errno, "address error");
		return -1;
	}

	nClientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(nClientSocket < 0)
	{
		errHandle(errno, "socket() error");
		return -1;
	}

	/*
	nEpollFileDescriptor = epoll_create(EPOLLSIZE);
	stEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLET;		/* epoll event */
	//stEvent.data.fd = nClientSocket;
	//epoll_ctl(nEpollFileDescriptor, EPOLL_CTL_ADD, nClientSocket, &stEvent);

	/* 서버 쪽 주소 구조체 0 초기화 */
	memset(&stServerAddr, 0, sizeof(stServerAddr));
	stServerAddr.sin_family = AF_INET;								/* 인터넷 주소체계 사용 */
	stServerAddr.sin_addr.s_addr = inet_addr(pszAddr);			/* 서버 IP 구조체에 저장 */
	stServerAddr.sin_port = htons(nPort);							/* 서버 Port 구조체에 저장 */

	/* connect */
	if (connect(nClientSocket, (struct sockaddr*) &stServerAddr, sizeof(stServerAddr)) < 0)
	{
		errHandle(errno, "connect() error %s\n", pszAddr);
		close(nClientSocket);
		nClientSocket = -1;
		return -1;
	}
	else
	{
		/*
		if(errno != EINPROGRESS)
		{
			errHandle(errno, "connect \n");
		}
		*/
		printf("연결되었습니다.\n");
	}

	/*---Wait for socket connect to complete---*/
	//nReady = epoll_wait(nEpollFileDescriptor, pstEpollEvent, EPOLLSIZE, 1000);
	
	/*
	for(nIndex = 0; nIndex < nReady; nIndex++)
	{
		if(pstEpollEvent[nIndex].events & EPOLLIN)
		{
			printf("connected %d\n", pstEpollEvent[nIndex].data.fd);
		}
	}
	*/
/*
	nReady = epoll_wait(nEpollFileDescriptor, stEvent, EPOLLSIZE, 1000);

	for(nIndex = 0; nIndex < nReady; nIndex++)
	{
		if(stEvent[nIndex].events & EPOLLIN)
		{
			printf("data %d\n", stEvent[nIndex].data.fd);
			memset()
		}
	}
*/

	/*
	if(nClientSocket < 0)
	{
		errHandle(errno, "connect error \n");
		close(nClientSocket);
		nClientSocket = -1;
	}
	*/

	return nClientSocket;
}

/* error 처리 handler */
void errHandle(int nErrorCode, char *pszMsg, ...)
{
	va_list argptr;
	char szBuf[MESSAGE] = {0,};

	if(nErrorCode)
	{
		printf("error = %s\n", strerror(errno));
	}

	if(pszMsg == NULL)
	{
		printf("message error = %s\n", strerror(errno));
	}

	va_start(argptr, pszMsg);
	vsnprintf(szBuf, MESSAGE, pszMsg, argptr);		/* 버퍼에 문자열 쓰고 버퍼 길이 지정하며 va_list 얻는다. */
	va_end(argptr);
	puts(szBuf);
}

