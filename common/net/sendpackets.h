

#ifndef SENDPACKETS_H
#define SENDPACKETS_H

#include "../platform.h"

#define RESEND_DELAY	200
#define RESEND_EXPIRE	(3*60*1000)

#ifdef _SERVER
class Client;
void SendData(char* data, int size, struct sockaddr_in* paddr, bool reliable, Client* c);
void Acknowledge(unsigned int ack, struct sockaddr_in from);
void ResendPackets();
void SendAll(int player, char* data, int size, bool reliable);
void JoinInfo(Client* c);
#else
void SendData(char* data, int size, struct sockaddr_in* paddr, bool reliable);
void Acknowledge(unsigned int ack);
void ResendPackets();
void Register(char* username, char* password, char* email);
void Login(char* username, char* password);
#endif

#endif	//SENDPACKETS_H