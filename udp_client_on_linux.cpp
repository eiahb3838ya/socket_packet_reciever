#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <time.h>

#define GET_LAST_SOCK_ERROR() errno
typedef unsigned int   DWORD;
typedef unsigned short WORD;
typedef unsigned int   UNINT32;
typedef unsigned char  BYTE;
#define ioctlsocket ioctl

void ParseMarketData(char* buffer,int nCount);

#define SOCKADDRLEN	socklen_t


int main()
{
	/// Create udp client
	int PORT=29234;
	

	struct sockaddr_in servaddr; //IPv4杶諉諳華硊隅砱
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //膘蕾UDP杶諉趼
	/* set reuse and non block for this socket */
	int son=1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&son, sizeof(son));
// #if defined(HP_UX) || defined(AIX)
	// setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&son, sizeof(son));
	memset(&servaddr, 0, sizeof(servaddr)); //華硊賦凳錨
	servaddr.sin_family = AF_INET; //IPv4衪祜
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//囀瞄硌隅華硊
	///servaddr.sin_addr.s_addr = inet_addr("192.168.0.255");
	servaddr.sin_port = htons(PORT); //傷諳
	printf("creat socketaddr ...done\n");



	//煦饜衪祜華硊,堂隅傷諳
	if (bind(sockfd, (sockaddr *) &servaddr, sizeof(servaddr)) != 0)
	{
		printf("bind port fail\n");
		getchar();
		exit(1);
	}
	
	else{
		printf("bind socketaddr ...done\n");
	}
	//set  fionbio
	for (;;)
	{
//#ifdef LINUX
		int on=1;
		if (ioctlsocket(sockfd, FIONBIO, (char *)&on) < 0){
//#endif
// #ifdef WIN32
// 		unsigned long on_windows=1;
// 		if (ioctlsocket(sockfd, FIONBIO, &on_windows) < 0){
// #endif
// #ifdef VOS
// 		int off=0;
// 		if (ioctlsocket(sockfd, FIONBIO, (char *)&off) <0){
// #endif
			if (GET_LAST_SOCK_ERROR() == 4)
				continue;
			printf("Can not set FIONBIO\n");

		}
		else
			break;
	}
	printf("set fionbio ...done");
	int rcvbufsize = 1*1024*1024;
	SOCKADDRLEN paramlen = sizeof(rcvbufsize);
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char *)&rcvbufsize, paramlen);
	if(ret != 0)
	{
		printf("Can not setsockopt revbuf\n");
		getchar();
	}
	else{
		printf("setsockopt revbuf ...done\n");
	}

	int on = 1;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	if(ret != 0)
	{
		printf("Can not setsockopt\n");
		getchar();
	}
	else{
		printf("setsockopt broadcast ...done\n");
	}

	int number = 4000;
	char buffer[4096];
	while(1)
	{
		sockaddr_in fromAddr;
		int nFromLen = sizeof(sockaddr_in);
		int nCount = recvfrom(sockfd, buffer, 4000, 0, (sockaddr *)&fromAddr,(socklen_t *) &nFromLen);
		if(nCount == 0)
			continue;
		if (nCount == -1)
		{
			int nErrno = GET_LAST_SOCK_ERROR();
			if (nErrno == 0 || nErrno == 251 || nErrno == EWOULDBLOCK)	/*251 for PARisk */	//20060224 IA64 add 0
				continue;
			else
			{
				printf("Error in recvFrom,ErrNo[%d]\n",nErrno);
				continue;
				//return -1;
			}
		}

		printf("get packet from %s\n",inet_ntoa(fromAddr.sin_addr));
		ParseMarketData(buffer,nCount);
		printf("end parse ...done\n");
	}

	return 0;
}

#pragma pack(push) 
#pragma pack(1)
struct CMBLMarketDataField
{
	///磁埮測鎢
	char content[10];
	///鎗闖源砃
	// char	Direction;
	// ///歎跡
	// double	Price;
	// ///杅講
	// int	Volume;
};
#pragma pack(pop)

void ParseMarketData(char* buffer,int nCount)
{
	if(nCount < sizeof(CMBLMarketDataField))
		return ;
	CMBLMarketDataField theMarketData;
	memcpy(&theMarketData,buffer,sizeof(theMarketData));
	printf("Content[%s]\n",theMarketData.content);
}