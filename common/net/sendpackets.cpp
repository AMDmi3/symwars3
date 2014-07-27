


#include "sendpackets.h"


#include "net.h"
#include "../sim/player.h"
#include "packets.h"

#ifdef _SERVER
#include "../server/svmain.h"
#include "client.h"
#include "../utils.h"
#endif

#ifdef _SERVER
void SendData(char* data, int size, struct sockaddr_in* paddr, bool reliable, Client* c)
#else
void SendData(char* data, int size, struct sockaddr_in* paddr, bool reliable)
#endif
{
#ifndef _SERVER
	g_lastS = GetTickCount64();
#endif

	if(reliable)
	{
#ifdef _SERVER
		((PacketHeader*)data)->ack = c->m_sendack;
#else
		((PacketHeader*)data)->ack = g_sendack;
#endif
		OldPacket p;
		p.buffer = new char[ size ];
		p.len = size;
		memcpy(p.buffer, data, size);
#ifdef _SERVER
		memcpy((void*)&p.addr, (void*)paddr, sizeof(struct sockaddr_in));
#endif
		p.last = GetTickCount64();
		p.first = p.last;
		g_sent.push_back(p);
#ifdef _SERVER
		c->m_sendack = NextAck(c->m_sendack);
#else
		g_sendack = NextAck(g_sendack);
#endif
	}

#ifdef _SERVER
	sendto(g_socket, data, size, 0, (struct sockaddr *)paddr, sizeof(struct sockaddr_in));
#elif defined( _IOS )
	// Address is NULL and the data is automatically sent to g_hostAddr by virtue of the fact that the socket is connected to that address
	ssize_t bytes = sendto(sock, data, size, 0, NULL, 0);
	
	int err;
	int sock;
	socklen_t addrLen;
	sock = CFSocketGetNative(g_cfSocket);

	if(bytes < 0)
		err = errno;
	else if(bytes == 0)
		err = EPIPE;
	else
		err = 0;
	
	if (err != 0)
		NetError([NSError errorWithDomain:NSPOSIXErrorDomain code:err userInfo:nil]);
#else
	sendto(g_socket, data, size, 0, (struct sockaddr *)paddr, sizeof(struct sockaddr_in));
#endif
}

void ResendPackets()
{
	OldPacket* p;
	long long now = GetTickCount64();
	long long due = now - RESEND_DELAY;
	long long expire = now - RESEND_EXPIRE;
	
	auto i=g_sent.begin();
	while(i!=g_sent.end())
	{
		p = &*i;
		if(p->last > due)
			continue;

		if(p->first < expire)
		{
			i = g_sent.erase(i);

#ifdef _SERVER
			g_log<<"expire at "<<DateTime()<<" dt="<<expire<<"-"<<p->first<<"="<<(expire-p->first)<<"(>"<<RESEND_EXPIRE<<") left = "<<g_sent.size()<<endl;
			g_log.flush();
#endif

			continue;
		}
		
#ifdef _SERVER
		SendData(p->buffer, p->len, &p->addr, false, NULL);
#else
		SendData(p->buffer, p->len, &g_sockaddr, false);
#endif
		p->last = now;
#ifdef _IOS
		NSLog(@"Resent at %lld", now);
#endif

#ifdef _SERVER
			g_log<<"resent at "<<DateTime()<<" left = "<<g_sent.size()<<endl;
			g_log.flush();
#endif

		i++;
	}
}

#ifdef _SERVER
void Acknowledge(unsigned int ack, struct sockaddr_in from)
#else
void Acknowledge(unsigned int ack)
#endif
{
	AcknowledgmentPacket p;
	p.header.type = PACKET_ACKNOWLEDGMENT;
	p.header.ack = ack;
	
#ifdef _SERVER
	SendData((char*)&p, sizeof(AcknowledgmentPacket), &from, false, NULL);
#else
	SendData((char*)&p, sizeof(AcknowledgmentPacket), &g_sockaddr, false);
#endif
}

#ifdef _SERVER

void SendAll(int player, char* data, int size, bool reliable)
{
	Client* c;
	Player* p;

	for(int i=0; i<CLIENTS; i++)
	{
		c = &g_client[i];

		if(!c->m_on)
			continue;

		if(c->m_player < 0)
			continue;

		p = &g_player[c->m_player];

		if(player >= 0 && c->m_player == player)
			continue;

		SendData(data, size, &c->m_addr, reliable, c);
	}
}

void JoinInfo(Client* c)
{
	g_log<<"ji 1"<<endl;
	g_log.flush();

	JoinInfoPacket jip;
	jip.header.type = PACKET_JOIN_INFO;
	jip.playerid = c->m_player;
	for(int i=0; i<PLANETS; i++)
	{
		Planet* p = &g_planet[i];
		Heightmap* hm = &p->m_hmap;
		jip.hmapwidth[i].x = hm->m_widthx;
		jip.hmapwidth[i].y = hm->m_widthz;
	}
	SendData((char*)&jip, sizeof(struct JoinInfoPacket), &c->m_addr, true, c);
	
	/*
	g_log<<"ji 2"<<endl;
	g_log.flush();

	for(int i=0; i<PLANETS; i++)
	{
		Planet* p = &g_planet[i];
		Heightmap* hm = &p->m_hmap;
		
		for(int x=0; x<=hm->m_widthx; x++)
			for(int z=0; z<=hm->m_widthz; z++)
			{
				HeightPointPacket hpp;
				hpp.header.type = PACKET_HEIGHT_POINT;
				hpp.planetid = i;
				hpp.tx = x;
				hpp.tz = z;
				hpp.height = hm->getheight(x, z);
				SendData((char*)&hpp, sizeof(struct HeightPointPacket), &c->m_addr, true, c);
			}
	}
	
	g_log<<"ji 3"<<endl;
	g_log.flush();*/

	for(int i=0; i<PLANETS; i++)
	{
		Player* p = &g_player[c->m_player];
		Camera* cam = &p->m_planetcam[i];
		PlanetCamPacket pcp;
		pcp.header.type = PACKET_PLANET_CAM;
		pcp.planetid = i;
		pcp.cam = *cam;
		SendData((char*)&pcp, sizeof(struct PlanetCamPacket), &c->m_addr, true, c);
	}
	
	g_log<<"ji 4"<<endl;
	g_log.flush();

	// send buildings
	for(int i=0; i<PLANETS; i++)
	{
		Planet* p = &g_planet[i];

		for(int j=0; j<BUILDINGS; j++)
		{
			Building* b = &p->m_building[j];

			if(!b->m_on)
				continue;

			/*
			struct BuildingPacket
{
	PacketHeader header;
	int planetid;
	int bid;
	bool on;
	int type;
	int state;
	Vec3f pos;
	float yaw;
	int conmat[RESOURCES];
	int stock[RESOURCES];
	int conwage;	//construction wage
};
*/		
			BuildingPacket bp;
			bp.header.type = PACKET_BUILDING;
			bp.planetid = i;
			bp.bid = j;
			bp.on = b->m_on;
			bp.type = b->m_type;
			bp.state = b->m_state;
			bp.pos = b->m_pos;
			bp.yaw = b->m_yaw;
			for(int k=0; k<RESOURCES; k++)
			{
				bp.conmat[i] = b->m_conmat[i];
				bp.stock[i] = b->m_stock[i];
			}
			bp.conwage = b->m_conwage;
			SendData((char*)&bp, sizeof(struct BuildingPacket), &c->m_addr, true, c);

			/*
struct GarrisonPacket
{
	PacketHeader header;
	int planetid;
	int bid;
	int uid;
};
*/

			for(auto k=b->m_garrison.begin(); k!=b->m_garrison.end(); k++)
			{
				
	g_log<<"ji 5"<<endl;
	g_log.flush();

				GarrisonPacket gp;
				gp.header.type = PACKET_GARRISON;
				gp.planetid = i;
				gp.bid = j;
				gp.uid = *k;
				SendData((char*)&gp, sizeof(struct GarrisonPacket), &c->m_addr, true, c);
			}
		}
	}

	
	g_log<<"ji 6"<<endl;
	g_log.flush();

	// send units
	
	for(int i=0; i<PLANETS; i++)
	{
		Planet* p = &g_planet[i];

		for(int j=0; j<UNITS; j++)
		{
			Unit* u = &p->m_unit[j];

			if(!u->m_on)
				continue;

			SpawnUnitPacket sup;
			sup.header.type = PACKET_SPAWN_UNIT;
			sup.planetid = i;
			sup.uid = j;
			sup.owner = u->m_owner;
			sup.type = u->m_type;
			sup.camera = u->m_camera;
			sup.mode = u->m_mode;
			sup.goal = u->m_goal;
			sup.subgoal = u->m_subgoal;
			sup.step = u->m_step;
			sup.target = u->m_target;
			sup.target2 = u->m_target2;
			sup.targtype = u->m_targtype;
			sup.driver = u->m_driver;
			sup.targetunit = u->m_targetunit;
			sup.underorder = u->m_underorder;
			sup.fuel = u->m_fuel;
			sup.labour = u->m_labour;
			sup.transportres = u->m_transportres;
			sup.transportamt = u->m_transportamt;
			sup.actpoints = u->m_actpoints;
			sup.hitpoints = u->m_hitpoints;
			sup.passive = u->m_passive;
			sup.distaccum = u->m_distaccum;
			sup.drivewage = u->m_drivewage;

			SendData((char*)&sup, sizeof(struct SpawnUnitPacket), &c->m_addr, true, c);
		}
	}

	
	g_log<<"ji 7"<<endl;
	g_log.flush();

	DoneLoadingPacket dlp;
	dlp.header.type = PACKET_DONE_LOADING;
	SendData((char*)&dlp, sizeof(struct DoneLoadingPacket), &c->m_addr, true, c);
	
	g_log<<"ji 8"<<endl;
	g_log.flush();
}

#else

void Register(char* username, char* password, char* email)
{
	RegistrationPacket p;
	p.header.type = PACKET_REGISTRATION;
	//strcpy(p.username, [username UTF8String]);
	//strcpy(p.email, [email UTF8String]);
	//strcpy(p.password, [password UTF8String]);
	strcpy(p.username, username);
	strcpy(p.email, email);
	strcpy(p.password, password);

	SendData((char*)&p, sizeof(struct RegistrationPacket), &g_sockaddr, true);
}

void Login(char* username, char* password)
{
	LoginPacket p;
	p.header.type = PACKET_LOGIN;
	p.version = VERSION;
	strcpy(p.username, username);
	strcpy(p.password, password);

	SendData((char*)&p, sizeof(struct LoginPacket), &g_sockaddr, true);
}

#endif


