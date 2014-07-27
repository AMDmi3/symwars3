
#include "../platform.h"
#include "net.h"
#include "readpackets.h"
#include "../utils.h"

#ifdef _SERVER
#include "../server/svmain.h"
#endif

SOCKET g_socket;
list<OldPacket> g_sent;
list<OldPacket> g_recv;
struct sockaddr_in g_sockaddr;

#ifndef _SERVER
unsigned int g_recvack = 0;
unsigned int g_sendack = 0;
long long g_lastS;  //last sent
long long g_lastR;  //last recieved
#endif

unsigned int NextAck(unsigned int ack)
{
	//if(ack == UINT_MAX)
	//	ack = 0;
	//else
	//	ack ++;

	ack++;

	return ack;
}

unsigned int PrevAck(unsigned int ack)
{
	//if(ack == 0)
	//	ack = UINT_MAX;
	//else
	//	ack --;

	ack--;
	
	return ack;
}

bool PastAck(unsigned int test, unsigned int current)
{
	return ((current >= test) && (current - test <= UINT_MAX/2)) 
		|| ((test > current) && (test - current > UINT_MAX/2));
}

void InitNet()
{

#ifdef _SERVER
	g_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if(g_socket < 0)
		g_error<<"Error creating socket errno="<<errno<<endl;

	// set SO_REUSEADDR on a socket to true (1):
	int optval = 1;
	setsockopt(g_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	g_sockaddr.sin_family = AF_INET;
	g_sockaddr.sin_addr.s_addr = INADDR_ANY;
	g_sockaddr.sin_port = htons(PORT);

	int length = sizeof(struct sockaddr_in);

	if(bind(g_socket, (struct sockaddr *)&g_sockaddr, length) < 0) 
		g_error<<"Error binding socket errno="<<errno<<endl;

	int flags = fcntl(g_socket, F_GETFL);
	flags |= O_NONBLOCK;

	fcntl(g_socket, F_SETFL, flags);

	//g_log<<"Net inited."<<endl;
#else
#ifdef _WIN

	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);

	g_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
   struct sockaddr_in clientaddr;
   int socklen = sizeof(struct sockaddr);

   memset((char*)&clientaddr, 0, sizeof(clientaddr));
   clientaddr.sin_family = AF_INET;
   clientaddr.sin_addr.s_addr = INADDR_ANY;
   clientaddr.sin_port = htons((u_short)PORT);

	struct hostent* hp = gethostbyname(SV_ADDR);
	g_sockaddr.sin_family = AF_INET;
	memcpy(&(g_sockaddr.sin_addr.s_addr), hp->h_addr, hp->h_length);
	//g_matchAd.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	g_sockaddr.sin_port = htons(PORT);

	/*
	g_svAd.sin_family = AF_INET;
	g_svAd.sin_addr.s_addr = htonl(INADDR_ANY);
	g_svAd.sin_port = htons(PORT);
	bind(g_socket, &g_svAd, sizeof(g_svAd));
	*/

	//g_socket = socket(PF_INET, SOCK_DGRAM, 0);

	if(g_socket < 0)
	{
		g_log<<"socket creation failed"<<endl;
		return;
	}

	if(bind(g_socket, (LPSOCKADDR)&clientaddr, socklen)<0)
	{
		g_log<<"bind failed"<<endl;
		return;
	}

	unsigned long int nonBlockingMode = 1;
	int result = ioctlsocket(g_socket, FIONBIO, &nonBlockingMode);

	if(result)
	{
		g_log<<"ioctlsocket failed"<<endl;
		return;
	}

#elif defined(_IOS)
	InitNetIOS();
#endif
#endif
}

void DeinitNet()
{
#ifdef _SERVER
	close(g_socket);
#else
#ifdef _WIN
	WSACleanup();
#elif defined(_IOS)

#endif
#endif
}

//Net input
void NetIn()
{
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(struct sockaddr_in);
	char buffer[1024];
	int bytes;

	do
	{
		bytes = recvfrom(g_socket, buffer, 1024, 0, (struct sockaddr *)&from, &fromlen);

		if(bytes > 0)
		{
#ifdef _SERVER
			TranslatePacket(buffer, bytes, from, true);
#else
			if(memcmp((void*)&from, (void*)&g_sockaddr, sizeof(struct sockaddr_in)) != 0)
				continue;

			TranslatePacket(buffer, bytes, true);
#endif
		}
	}while(bytes > 0);
}

void ClearPackets()
{
	for(auto i=g_sent.begin(); i!=g_sent.end(); i++)
		i->freemem();
	
	for(auto i=g_recv.begin(); i!=g_recv.end(); i++)
		i->freemem();
}

