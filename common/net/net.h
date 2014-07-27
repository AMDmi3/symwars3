

#ifndef NET_H
#define NET_H

#include "../platform.h"
//#include "packets.h"

#define PORT		50400
//#define SV_ADDR		"corpstates.com"	//live server
//#define SV_ADDR		"54.221.229.124"	//corp1 aws
//#define SV_ADDR			"192.168.1.100"		//home local server ip
#define SV_ADDR			"192.168.1.103"		//home local server ip
//#define SV_ADDR			"174.6.61.178"		//home public server ip

unsigned int NextAck(unsigned int ack);
unsigned int PrevAck(unsigned int ack);
bool PastAck(unsigned int test, unsigned int current);

class OldPacket;

extern SOCKET g_socket;
extern list<OldPacket> g_sent;
extern list<OldPacket> g_recv;
extern struct sockaddr_in g_sockaddr;

#ifndef _SERVER
extern unsigned int g_recvack;
extern unsigned int g_sendack;
extern long long g_lastS;  //last sent
extern long long g_lastR;  //last recieved
#endif

void InitNet();
void DeinitNet();
void NetIn();
void ClearPackets();

#endif

