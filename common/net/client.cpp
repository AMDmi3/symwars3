


#include "../platform.h"

#ifdef _SERVER
#include "client.h"
#include "sendpackets.h"
#include "net.h"
#include "packets.h"
#include "../sim/player.h"
#include "../utils.h"

Client g_client[CLIENTS];

int MatchClient(struct sockaddr_in from)
{
	for(int i=0; i<CLIENTS; i++)
	{
		Client* c = &g_client[i];

		if(!c->m_on)
			continue;

		if(memcmp((void*)&c->m_addr, (void*)&from, sizeof(struct sockaddr)) == 0)
			return i;
	}

	return -1;
}

int MatchClient(Client* c)
{
	//for(int i=0; i<CLIENTS; i++)
	//	if(&g_client[i] == c)
	//		return i;
	
	//return -1;

	return (int)(c - g_client);
}

int PlayerClient(int ID)
{
	/*
	Player* p;

	for(int i=0; i<PLAYERS; i++)
	{
		p = &g_player[i];
		
		if(!p->on)
			continue;

		if(p->id != id)
			continue;

		return p->client;
	}

	return -1;*/

	return g_player[ID].m_client;
}

int NewClient()
{
	for(int i=0; i<CLIENTS; i++)
		if(!g_client[i].m_on)
			return i;

	return -1;
}

Client* NewClient(struct sockaddr_in from)
{
	int client = NewClient();
	if(client < 0)
		return NULL;

	Client* c = &g_client[client];

	c->m_on = true;
	c->m_last = GetTickCount64();
	memcpy((void*)&c->m_addr, (void*)&from, sizeof(struct sockaddr_in));
	c->m_sendack = 0;
	c->m_recvack = UINT_MAX;
	c->m_player = -1;

	return c;
}

//For use when the player is dissociated from the client
void DisconnectPlayer(int i)
{
	Player* p = &g_player[i];

	// TO DO: Save player state to disk

	DisconnectPacket dp;
	dp.header.type = PACKET_DISCONNECT;
	//dp.id = p->id;
	dp.id = i;
	SendAll(i, (char*)&dp, sizeof(DisconnectPacket), true);

	/*
	CEntity* e = NULL;
	if(p->entity >= 0)
	{
		e = &g_entity[p->entity];
		e->on = false;
		e->controller = -1;
	}
	*/

	Client* c = NULL;
	if(p->m_client >= 0)
	{
		c = &g_client[p->m_client];
		c->m_player = -1;
	}

	//p->m_on = false;
	//p->m_entity = -1;
	p->m_client = -1;
}

void CheckPackets()
{
	static long long last;
	long long now = timeGetTime();

	if(now - last < 60*1000)
		return;

	last = now;

	OldPacket* pack;

	auto i=g_recv.begin();
	while(i!=g_recv.end())
	{
		pack = &*i;

		if(MatchClient(pack->addr) >= 0)
		{
			i++;
			continue;
		}

		pack->freemem();
		i = g_recv.erase(i);
	}

	i=g_sent.begin();
	while(i!=g_sent.end())
	{
		pack = &*i;

		if(MatchClient(pack->addr) >= 0)
		{
			i++;
			continue;
		}

		pack->freemem();
		i = g_sent.erase(i);
	}
}

void ClearClientPackets(Client* c)
{
	OldPacket* pack;

	auto i=g_recv.begin();
	while(i!=g_recv.end())
	{
		pack = &*i;

		if(memcmp((void*)&pack->addr, (void*)&c->m_addr, sizeof(struct sockaddr_in)) != 0)
		{
			i++;
			continue;
		}

		pack->freemem();
		i = g_recv.erase(i);
	}
	
	i=g_sent.begin();
	while(i!=g_sent.end())
	{
		pack = &*i;

		if(memcmp((void*)&pack->addr, (void*)&c->m_addr, sizeof(struct sockaddr_in)) != 0)
		{
			i++;
			continue;
		}

		pack->freemem();
		i = g_sent.erase(i);
	}
}

void DisconnectClient(int i)
{
	Client* c = &g_client[i];

	ClearClientPackets(c);

	ConnectionResetPacket crp;
	crp.header.type = PACKET_CONNECTION_RESET;
	crp.header.ack = 0;
	SendData((char*)&crp, sizeof(ConnectionResetPacket), &c->m_addr, false, c);

	if(c->m_player >= 0)
		DisconnectPlayer(c->m_player);
	
	c->m_on = false;
}

void DisconnectClients()
{
	for(int i=0; i<CLIENTS; i++)
	{
		if(!g_client[i].m_on)
			continue;

		DisconnectClient(i);
	}
}

void CheckClients()
{
	Client* c;
	static long long last;
	long long now = GetTickCount64();

	if(now - last < 60*1000)
		return;

	last = now;

	for(int i=0; i<CLIENTS; i++)
	{
		c = &g_client[i];

		if(!c->m_on)
			continue;

		if(now - c->m_last < CLIENT_TIMEOUT)
			continue;

		DisconnectClient(i);
	}
}

#endif

