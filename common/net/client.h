
#include "../platform.h"

#ifdef _SERVER
#include "net.h"

#define CLIENT_TIMEOUT	(60*1000)

class Client
{
public:
	bool m_on;
	struct sockaddr_in m_addr;
	unsigned int m_sendack;
	unsigned int m_recvack;
	long long m_last;
	int m_player;

	Client()
	{
		m_on = false;
		m_sendack = 0;
		m_recvack = 0;
		m_player = -1;
	}
};

#define CLIENTS		256

extern Client g_client[CLIENTS];

int MatchClient(struct sockaddr_in from);
int MatchClient(Client* c);
int PlayerClient(int id);
Client* NewClient(struct sockaddr_in from);
void CheckPackets();
void CheckClients();
//void UnloadPlayer(int i);
void DisconnectClient(int i);
void DisconnectPlayer(int i);	//For use when the player is dissociated from the client
void ClearClientPackets(Client* c);
void DisconnectClients();

#endif


