// windows socket.cpp: 定義主控台應用程式的進入點。
//
#pragma comment(lib,"wsock32.lib")

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
//#include "winsock.h"
#include <winsock.h>
#include <string>

#define SOCKADDRLEN int
#define GET_LAST_SOCK_ERROR() h_errno


typedef int socklen_t;
void ParseMarketData(char* buffer, int nCount);
int main() {

	//init WSAData
	WSAData wsaData;
	WORD version = MAKEWORD(2, 2); // 版本
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // 成功回傳 0
	if (iResult != 0) {
		printf("init WSAData fail");
		getchar();
	}
	else {
		printf("init WSAData success\n");
	}

	//build socket 
	SOCKET sListen = INVALID_SOCKET;
	sListen = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sListen == INVALID_SOCKET) {
		// 建立失敗
		printf("build socket fail");
		getchar();
	}
	else {
		printf("build socket success\n");
	}

	//set sockaddr_in
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr)); // 清空,將資料設為 0
									//addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 設定 IP,後面解釋 inet_addr()
	addr.sin_addr.s_addr = INADDR_ANY; // 若設定 INADDR_ANY 表示任何 IP
	addr.sin_family = AF_INET;
	addr.sin_port = htons(37020); // 設定 port,後面解釋 htons()
	printf("set sockaddr_in success\n");

	//bind sListen and sockaddr_in
	int r = bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	if (r) {
		printf("bind sListen and sockaddr_in fail");
		getchar();
	}
	else {
		printf("bind sListen and sockaddr_in success\n");
	}
	//assert(r != SOCKET_ERROR);
	//set ioctlsocket非阻塞式
	unsigned long on_windows = 1;
	if (ioctlsocket(sListen, FIONBIO, &on_windows) < 0) {
		printf("set ioctlsocket fail");
		getchar();
	}
	else
		printf("set ioctlsocket\n");

	//set recv buffer
	int rcvbufsize = 1 * 1024 * 1024;
	SOCKADDRLEN paramlen = sizeof(rcvbufsize);
	int ret = setsockopt(sListen, SOL_SOCKET, SO_RCVBUF, (const char *)&rcvbufsize, paramlen);
	if (ret != 0)
	{
		printf("Can not setsockopt revbuf\n");
		getchar();
	}
	else
		printf("set recv buffer\n");

	//set socket option to broadcast 
	char on = 1;
	ret = setsockopt(sListen, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	if (ret != 0)
	{
		printf("Can not setsockopt\n");
		getchar();
	}
	else
		printf("set socket option to broadcast\n");

	int number = 4000;
	char buffer[4096];

	//start listen
	while (1)
	{
		sockaddr_in fromAddr;
		int nFromLen = sizeof(sockaddr_in);
		int nCount = recvfrom(sListen, buffer, sizeof(buffer), 0, (sockaddr *)&fromAddr, (socklen_t *)&nFromLen);
		if (nCount == 0)
			continue;
		if (nCount == -1)
		{
			int nErrno = GET_LAST_SOCK_ERROR();
			if (nErrno == 0 || nErrno == 251 || nErrno == EWOULDBLOCK)
				continue;
			else
			{
				//printf("Error in recvFrom,ErrNo[%d]\n", nErrno);
				continue;
				//getchar();

				return -1;
			}
		}
		//printf(buffer);
		//printf("from IP address %s\n",inet_ntop(addr.ss_family,((struct sockadd_in *)&addr)->sin_addr ,ipstr, sizeof ipstr);
		sockaddr_in in;
		memcpy(&in.sin_addr, (sockaddr *)&fromAddr.sin_addr, sizeof(fromAddr.sin_addr));
		printf("has address %s\n", inet_ntoa(in.sin_addr));
		ParseMarketData(buffer, nCount);
	}

	return 0;
}


#pragma pack(push) 
#pragma pack(1)
struct CMBLMarketDataField
{
	char content[25];
};
#pragma pack(pop)


void ParseMarketData(char* buffer, int nCount) {
	CMBLMarketDataField theMarketData;
	memcpy(&theMarketData, buffer, sizeof(buffer));
	printf("content:[%s]\n", theMarketData.content);

}
