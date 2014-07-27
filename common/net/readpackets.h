
#ifndef READPACKETS_H
#define READPACKETS_H

#include "../platform.h"

#ifdef _SERVER
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include "packets.h"

#ifdef _SERVER
class Client;
void TranslatePacket(char* buffer, int bytes, struct sockaddr_in from, bool checkprev);
void PacketSwitch(int type, char* buffer, int bytes, struct sockaddr_in from, Client* c);
void ReadLoginPacket(LoginPacket* p, struct sockaddr_in from, Client* c);
void ReadRegistrationPacket(RegistrationPacket* p, struct sockaddr_in from, Client* c);
void ReadAcknowledgmentPacket(AcknowledgmentPacket* ap, struct sockaddr_in from, Client* c);
#else	//_SERVER
void TranslatePacket(char* buffer, int bytes, bool checkprev);
void PacketSwitch(int type, char* buffer, int bytes);
void ReadAcknowledgmentPacket(AcknowledgmentPacket* ap);
void ReadRegisteredPacket(RegisteredPacket* rp);
void ReadSpawnUnitPacket(SpawnUnitPacket* sup);
void ReadHeightPointPacket(HeightPointPacket* hpp);
void ReadBuildingPacket(BuildingPacket* bp);
void ReadGarrisonPacket(GarrisonPacket* gp);
void ReadJoinInfoPacket(JoinInfoPacket* jip);
void ReadDoneLoadingPacket(DoneLoadingPacket* dlp);

void ReadUsernameExistsPacket(UsernameExistsPacket* uep);
void ReadEmailExistsPacket(EmailExistsPacket* eep);
void ReadIncorrectLoginPacket(IncorrectLoginPacket* ilp);
void ReadIncorrectVersionPacket(IncorrectVersionPacket* ivp);
void ReadLoginCorrectPacket(LoginCorrectPacket* lcp);
void ReadTooManyClientsPacket(TooManyClientsPacket* tmcp);  
void ReadRegDBErrorPacket(RegDBErrorPacket* rdbep);
void ReadConnectionResetPacket(ConnectionResetPacket* crp);
void ReadDisconnectPacket(DisconnectPacket* dp);
#endif	//_SERVER

#endif	//READPACKETS_H