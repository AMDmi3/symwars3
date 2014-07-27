

#include "readpackets.h"

#include "sendpackets.h"
#include "packets.h"
#include "net.h"
#ifdef _SERVER
#include "client.h"
#include "../server/svmain.h"
#include "server.h"
#include "../database/mysql.h"
#include "../database/database.h"
#include "../debug.h"
#include "../utils.h"
#else
#include "../../game/ggui.h"
#include "../sim/visibility.h"
#include "../../game/spaceview.h"
#include "../../game/gmain.h"
#include "../gui/gui.h"
#endif
#include "../sim/planet.h"
#include "../sim/unitt.h"
#include "../sim/unit.h"
#include "../sim/buildingt.h"
#include "../sim/building.h"
#include "../sim/player.h"
#include "../utils.h"

#define REGCRASH_DBG

/*
What this function does is take a range of packet ack's (acknowledgment number for reliable UDP transmission) 
and executes that range of buffered received packets. This is needed because packets might arrive out of order, 
be missing some in between, and I execute them only after a whole range up to the latest ack has been received.

The out-of-order packets are stored in the g_recv vector.

Notice that there is preprocessor check if we are compiling this for the master server _SERVER (because I'm 
making this for a persistent, online world) or client. If server, there's extra parameters to match the packets 
to the right client; we're only interested in processing the packet range for a certain client.

Each packet goes to the PacketSwitch function, that is like a switch-table that executes the right 
packet-execution function based on the packet type ID. The switch-table could probably be turned into 
an array of function pointers to improve performance, probably only slightly.

The function takes a time of log(O) to execute, because it has to search through all the buffered packets 
several times to execute them in the right order. And before that, there's a check to see if we even have 
the whole range of packets from the last "recvack" before calling this function.

I keep a "sendack" and "recvack" for each client, for sent packets and received packets. I only update the 
recvack up to the latest one once a continuous range has been received, with no missing packets. Recvack 
is thus the last executed received packet.
*/

#ifdef _SERVER
void ParseRecieved(unsigned int first, unsigned int last, struct sockaddr_in addr, Client* c)
#else
void ParseRecieved(unsigned int first, unsigned int last)
#endif
{
	OldPacket* p;
	PacketHeader* header;
	unsigned int current = first;
	unsigned int afterlast = NextAck(last);
	
	do
	{
		for(auto i=g_recv.begin(); i!=g_recv.end(); i++)
		{
			p = &*i;
			header = (PacketHeader*)&p->buffer;

			if(header->ack != current)
				continue;

#ifdef _SERVER
			if(memcmp((void*)&p->addr, (void*)&addr, sizeof(struct sockaddr_in)) != 0)
				continue;
			
			PacketSwitch(header->type, p->buffer, p->len, addr, c);
#else
			PacketSwitch(header->type, p->buffer, p->len);
#endif
		
			p->freemem();
			i = g_recv.erase(i);
			current = NextAck(current);
			break;
		}
	}while(current != afterlast);
}

#ifdef _SERVER
bool Recieved(unsigned int first, unsigned int last, struct sockaddr_in addr)
#else
bool Recieved(unsigned int first, unsigned int last)
#endif
{
	OldPacket* p;
	PacketHeader* header;
	unsigned int current = first;
	unsigned int afterlast = NextAck(last);
	bool missed;
	
	do
	{
		missed = true;
		for(auto i=g_recv.begin(); i!=g_recv.end(); i++)
		{
			p = &*i;
			header = (PacketHeader*)&p->buffer;

			if(header->ack != current)
				continue;

#ifdef _SERVER
			if(memcmp((void*)&p->addr, (void*)&addr, sizeof(struct sockaddr_in)) != 0)
				continue;
#endif

			current = NextAck(current);
			missed = false;
			break;
		}
		
		if(missed)
			return false;
	}while(current != afterlast);
	
	return true;
}

#ifdef _SERVER
void AddRecieved(char* buffer, int len, struct sockaddr_in addr)
#else
void AddRecieved(char* buffer, int len)
#endif
{
	OldPacket p;
	p.buffer = new char[ len ];
	p.len = len;
	memcpy((void*)p.buffer, (void*)buffer, len);
#ifdef _SERVER
	memcpy((void*)&p.addr, (void*)&addr, sizeof(struct sockaddr_in));
#endif
	g_recv.push_back(p);
}

#ifdef _SERVER
void TranslatePacket(char* buffer, int bytes, struct sockaddr_in from, bool checkprev)
#else
void TranslatePacket(char* buffer, int bytes, bool checkprev)
#endif
{
	PacketHeader* header = (PacketHeader*)buffer;

#ifdef _SERVER

	PacketHeader* phtemp = (PacketHeader*)buffer;

	g_log<<"tp t"<<phtemp->type<<endl;
	g_log.flush();

	int client = MatchClient(from);
	Client* c = NULL;
	if(client >= 0)
	{
		c = &g_client[client];
		c->m_last = GetTickCount64();
	}
#else
	g_lastR = GetTickCount64();

	g_log<<"ack"<<header->ack<<" type"<<header->type<<endl;
#endif

	switch(header->type)
	{
#ifdef _SERVER
	case PACKET_ACKNOWLEDGMENT:
	case PACKET_REGISTRATION:
	case PACKET_LOGIN:
		{
			checkprev = false;
			break;
		}
#else        
	case PACKET_ACKNOWLEDGMENT:
	case PACKET_USERNAME_EXISTS:
	case PACKET_EMAIL_EXISTS:
	case PACKET_INCORRECT_LOGIN:
	case PACKET_INCORRECT_VERSION:
	case PACKET_LOGIN_CORRECT:
	case PACKET_TOO_MANY_CLIENTS:
	case PACKET_REG_DB_ERROR:
	case PACKET_REGISTRATION_DONE:
	case PACKET_CONNECTION_RESET:
		{
			checkprev = false;
			break;
		}
#endif
	default: break;
	}

#ifndef _SERVER
	//if(g_loadbytes > 0)
	{
		//char msg[128];
		//sprintf(msg, DateTime().c_str());
		//MessageBlock(msg, true);
	}
#endif

#ifdef _SERVER
	if(checkprev && c != NULL)
#else
	if(checkprev)
#endif
	{
#ifdef _SERVER
		if(PastAck(header->ack, c->m_recvack) || Recieved(header->ack, header->ack, from))
#else
		if(PastAck(header->ack, g_recvack) || Recieved(header->ack, header->ack))
#endif
		{
#ifdef _SERVER
			Acknowledge(header->ack, from);
#else
			Acknowledge(header->ack);
#endif
			//g_log<<"ack "<<header->ack<<endl;
			return;
		}
		
#ifdef _SERVER
		unsigned int next = NextAck(c->m_recvack);
#else
		unsigned int next = NextAck(g_recvack);
#endif

		if(header->ack == next) {}  // Translate packet
		else  // More than +1 after recvack?
		{
			unsigned int last = PrevAck(header->ack);
#ifdef _SERVER
			if(Recieved(next, last, from))
				ParseRecieved(next, last, from, c);  // Translate in order
#else
			if(Recieved(next, last))
				ParseRecieved(next, last);  // Translate in order
#endif
			else
			{
#ifdef _SERVER
				AddRecieved(buffer, bytes, from);
#else
				AddRecieved(buffer, bytes);
#endif
				return;
			}
		}
	}

#ifdef _SERVER
	PacketSwitch(header->type, buffer, bytes, from, c);
#else
	PacketSwitch(header->type, buffer, bytes);
#endif
	
	if(header->type != PACKET_ACKNOWLEDGMENT)
	{
#ifdef _SERVER
		if(c == NULL)
		{
			client = MatchClient(from);
			if(client >= 0)
				c = &g_client[client];
		}

		if(c != NULL)
			c->m_recvack = header->ack;
		Acknowledge(header->ack, from);
#else
		g_recvack = header->ack;
		Acknowledge(header->ack);
#endif
	}
}

#ifdef _SERVER
void PacketSwitch(int type, char* buffer, int bytes, struct sockaddr_in from, Client* c)
#else
void PacketSwitch(int type, char* buffer, int bytes)
#endif
{
	switch(type)
	{
#ifdef _SERVER
	case PACKET_LOGIN:				ReadLoginPacket((LoginPacket*)buffer, from, c);						break;
	case PACKET_REGISTRATION:		ReadRegistrationPacket((RegistrationPacket*)buffer, from, c);		break;
	case PACKET_ACKNOWLEDGMENT:	ReadAcknowledgmentPacket((AcknowledgmentPacket*)buffer, from, c);	break;
#else
	case PACKET_ACKNOWLEDGMENT:		ReadAcknowledgmentPacket((AcknowledgmentPacket*)buffer);			break;
	case PACKET_REGISTRATION_DONE:	ReadRegisteredPacket((RegisteredPacket*)buffer);          break;
	case PACKET_SPAWN_UNIT:			ReadSpawnUnitPacket((SpawnUnitPacket*)buffer);					break;
	case PACKET_HEIGHT_POINT:		ReadHeightPointPacket((HeightPointPacket*)buffer);			break;
	case PACKET_BUILDING:			ReadBuildingPacket((BuildingPacket*)buffer);				break;
	case PACKET_GARRISON:			ReadGarrisonPacket((GarrisonPacket*)buffer);				break;
	case PACKET_JOIN_INFO:			ReadJoinInfoPacket((JoinInfoPacket*)buffer);				break;
	case PACKET_DONE_LOADING:		ReadDoneLoadingPacket((DoneLoadingPacket*)buffer);			break;
		
	case PACKET_USERNAME_EXISTS:   ReadUsernameExistsPacket((UsernameExistsPacket*)buffer);    break;
	case PACKET_EMAIL_EXISTS:      ReadEmailExistsPacket((EmailExistsPacket*)buffer);          break;
	case PACKET_INCORRECT_LOGIN:   ReadIncorrectLoginPacket((IncorrectLoginPacket*)buffer);    break;
	case PACKET_INCORRECT_VERSION: ReadIncorrectVersionPacket((IncorrectVersionPacket*)buffer);    break;
	case PACKET_LOGIN_CORRECT:     ReadLoginCorrectPacket((LoginCorrectPacket*)buffer);        break;
	case PACKET_TOO_MANY_CLIENTS:  ReadTooManyClientsPacket((TooManyClientsPacket*)buffer);    break;
	case PACKET_REG_DB_ERROR:      ReadRegDBErrorPacket((RegDBErrorPacket*)buffer);            break;
	/*
	case PACKET_CONNECTION_RESET:  ReadConnectionResetPacket((ConnectionResetPacket*)buffer);  break;
	case PACKET_DISCONNECT:        ReadDisconnectPacket((DisconnectPacket*)buffer);            break;
	*/
#endif
	default: break;
	}
}

#ifdef _SERVER

void ReadLoginPacket(LoginPacket* p, struct sockaddr_in from, Client* c)
{
	g_log<<"rlp 1"<<endl;
	g_log.flush();

	if(c == NULL)
		c = NewClient(from);

	if(c == NULL)
	{
		TooManyClientsPacket tmcp;
		tmcp.header.type = PACKET_TOO_MANY_CLIENTS;
		tmcp.header.ack = 0;
		SendData((char*)&tmcp, sizeof(TooManyClientsPacket), &from, false, c);
		return;
	}
	
	g_log<<"rlp 2"<<endl;
	g_log.flush();

	if(p->version != VERSION)
	{
		IncorrectVersionPacket ivp;
		ivp.header.type = PACKET_INCORRECT_VERSION;
		ivp.version = VERSION;
		SendData((char*)&ivp, sizeof(IncorrectVersionPacket), &from, true, c);
		return;
	}

	g_log<<"rlp 3"<<endl;
	g_log.flush();

	//Prevent buffer overrun
	p->username[UN_LEN-1] = '\0';
	p->password[PW_LEN-1] = '\0';
	
	char username[UN_LEN*2];
	char crypt[CR_LEN];

	//EscapeString(p->username, username);
	strcpy(username, p->username);
	Hash(p->password, crypt);

	
	g_log<<"rlp 4"<<endl;
	g_log.flush();

	int loginpl = CorrectLogin(username, crypt);

	g_log<<"rlp 4.1"<<endl;
	g_log.flush();

	if(loginpl < 0)
	{
		g_log<<"rlp 4 i"<<endl;
		g_log.flush();

		IncorrectLoginPacket ilp;
		ilp.header.type = PACKET_INCORRECT_LOGIN;
		SendData((char*)&ilp, sizeof(IncorrectLoginPacket), &from, true, c);
		return;
	}

	
	g_log<<"rlp 5"<<endl;
	g_log.flush();

	if(c->m_player >= 0)	//Different player logged in previously from this address?
	{
		Player* pl = &g_player[c->m_player];
		if(stricmp(pl->m_username, p->username) != 0)
			DisconnectPlayer(c->m_player);
	}
	
	
	g_log<<"rlp 6"<<endl;
	g_log.flush();

	/*
	if(!LoadPlayer(username, c, p->username))
		g_error<<"Error loading player "<<username<<endl;
		*/

	g_player[loginpl].m_client = MatchClient(c);
	c->m_player = loginpl;

	
	g_log<<"rlp 7"<<endl;
	g_log.flush();
	
	Player* pl = &g_player[c->m_player];
	int prevclient = PlayerClient(c->m_player);	//Is user already logged in from somewhere else?
	if(prevclient >= 0)
		DisconnectClient(prevclient);

	
	g_log<<"rlp 8"<<endl;
	g_log.flush();
	
	LoginCorrectPacket clp;
	clp.header.type = PACKET_LOGIN_CORRECT;
	SendData((char*)&clp, sizeof(LoginCorrectPacket), &from, true, c);

	
	g_log<<"rlp 9"<<endl;
	g_log.flush();

	JoinInfo(c);

	// TO DO: inform other clients
}

void ReadRegistrationPacket(RegistrationPacket* p, struct sockaddr_in from, Client* c)
{
	if(c == NULL)
		c = NewClient(from);

	if(c == NULL)
	{
		TooManyClientsPacket tmcp;
		tmcp.header.type = PACKET_TOO_MANY_CLIENTS;
		tmcp.header.ack = 0;
		SendData((char*)&tmcp, sizeof(TooManyClientsPacket), &from, false, c);
		return;
	}

	if(PastAck(p->header.ack, c->m_recvack))
		return;	//This is a resent packet so return

	//Prevent buffer overrun
	p->username[UN_LEN-1] = '\0';
	p->email[EM_LEN-1] = '\0';
	p->password[PW_LEN-1] = '\0';

	if(strlen(p->password) <= 0 || strlen(p->email) <= 0 || strlen(p->username) <= 0)
	{
		RegDBErrorPacket rdbep;
		rdbep.header.type = PACKET_REG_DB_ERROR;
		c->m_sendack = NextAck(c->m_sendack);
		rdbep.header.ack = c->m_sendack;
		SendData((char*)&rdbep, sizeof(RegDBErrorPacket), &from, true, c);
		return;
	}
	
	//char username[UN_LEN*2];
	//char email[EM_LEN*2];
	char crypt[CR_LEN];

	//EscapeString(p->username, username);
	//EscapeString(p->email, email);
	Hash(p->password, crypt);

	if(UsernameExists(p->username) >= 0)
	{
		UsernameExistsPacket uep;
		uep.header.type = PACKET_USERNAME_EXISTS;
		c->m_sendack = NextAck(c->m_sendack);
		uep.header.ack = c->m_sendack;
		SendData((char*)&uep, sizeof(UsernameExistsPacket), &from, true, c);
		return;
	}

	if(EmailExists(p->email) >= 0)
	{
		EmailExistsPacket eep;
		eep.header.type = PACKET_EMAIL_EXISTS;
		c->m_sendack = NextAck(c->m_sendack);
		eep.header.ack = c->m_sendack;
		SendData((char*)&eep, sizeof(EmailExistsPacket), &from, true, c);
		return;
	}

	int plid = -1;

	if(!Register(p->username, p->email, crypt, &plid))
	{
		RegDBErrorPacket rdbep;
		rdbep.header.type = PACKET_REG_DB_ERROR;
		c->m_sendack = NextAck(c->m_sendack);
		rdbep.header.ack = c->m_sendack;
		SendData((char*)&rdbep, sizeof(RegDBErrorPacket), &from, true, c);
		return;
	}
	
	RegisteredPacket rp;
	rp.header.type = PACKET_REGISTRATION_DONE;
	c->m_sendack = NextAck(c->m_sendack);
	rp.header.ack = c->m_sendack;
	SendData((char*)&rp, sizeof(RegisteredPacket), &from, true, c);

	g_log<<"Registration id"<<plid<<" \""<<p->username<<"\" \""<<p->email<<"\" "<<crypt<<endl;

	//if(!LoadPlayer(p->username, c))	//So that we know to return on resent PACKET_REGISTRATION packets
	//	g_error<<"Error loading player "<<p->username<<endl;
}

#endif

#ifdef _SERVER
void ReadAcknowledgmentPacket(AcknowledgmentPacket* ap, struct sockaddr_in from, Client* c)
#else
void ReadAcknowledgmentPacket(AcknowledgmentPacket* ap)
#endif
{
	OldPacket* p;
	PacketHeader* header;
	
	for(auto i=g_sent.begin(); i!=g_sent.end(); i++)
	{
		p = &*i;
		header = (PacketHeader*)p->buffer;
#ifdef _SERVER
		if(header->ack == ap->header.ack && memcmp((void*)&p->addr, (void*)&from, sizeof(struct sockaddr_in)) == 0)
#else
		if(header->ack == ap->header.ack)
#endif
		{
			p->freemem();
			i = g_sent.erase(i);
#ifdef _SERVER
			g_log<<"left to ack "<<g_sent.size()<<endl;
			g_log.flush();
#endif
			return;
		}
	}
}

#ifndef _SERVER
void ReadRegisteredPacket(RegisteredPacket* rp)
{	
	//void MessageBlock(const char* msg, bool show);
	//void ShowMessageBlockContinue(bool show, void (*clickf)());

	MessageBlock("Registered successfully.", true);
	ShowMessageBlockContinue(true, Continue_BackToMain);
}

void ReadSpawnUnitPacket(SpawnUnitPacket* sup)
{
	Planet* planet = &g_planet[sup->planetid];
	Unit* u = &planet->m_unit[sup->uid];

		/*
struct SpawnUnitPacket
{
	PacketHeader header;
	int planetid;
	int uid;
	int owner;
	int type;
	Camera camera;
	int mode;
	Vec3f goal;
	Vec3f subgoal;
	int step;
	int target;
	int target2;
	int targtype;
	int driver;
	bool targetunit;
	bool underorder;
	int fuel;
	int labour;
	int transportres;
	int transportamt;
	int actpoints;
	int hitpoints;
	bool passive;
	int distaccum;
	int drivewage;
};
*/

	u->m_owner = sup->owner;
	u->m_type = sup->type;
	u->m_camera = sup->camera;
	u->m_mode = sup->mode;
	u->m_goal = sup->goal;
	u->m_subgoal = sup->subgoal;
	u->m_step = sup->step;
	u->m_target = sup->target;
	u->m_target2 = sup->target2;
	u->m_targtype = sup->targtype;
	u->m_driver = sup->driver;
	u->m_targetunit = sup->targetunit;
	u->m_underorder = sup->underorder;
	u->m_fuel = sup->fuel;
	u->m_labour = sup->labour;
	u->m_transportres = sup->transportres;
	u->m_transportamt = sup->transportamt;
	u->m_actpoints = sup->actpoints;
	u->m_hitpoints = sup->hitpoints;
	u->m_passive = sup->passive;
	u->m_distaccum = sup->distaccum;
	u->m_drivewage = sup->drivewage;

	if(u->m_owner == g_localplayer)
		AddVis(sup->planetid, sup->uid);
	
	if(g_mode == MENU)
	{
		g_loadbytes += sizeof(JoinInfoPacket);
		char msg[128];
		sprintf(msg, "Received %d bytes...", g_loadbytes);
		MessageBlock(msg, true);
	}
}

void ReadHeightPointPacket(HeightPointPacket* hpp)
{
	/*struct HeightPointPacket
{
	PacketHeader header;
	int planetid;
	int tx;
	int tz;
	float height;
};*/
	
	Planet* p = &g_planet[hpp->planetid];
	Heightmap* hm = &p->m_hmap;
	hm->setheight(hpp->tx, hpp->tz, hpp->height);

	g_log<<"hp "<<hpp->tx<<","<<hpp->tz<<endl;
	
	if(g_mode == MENU)
	{
		g_loadbytes += sizeof(JoinInfoPacket);
		char msg[128];
		sprintf(msg, "Received %d bytes...", g_loadbytes);
		MessageBlock(msg, true);
	}
}

void ReadBuildingPacket(BuildingPacket* bp)
{
	/*struct BuildingPacket
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
};*/

	Planet* p = &g_planet[bp->planetid];
	Building* b = &p->m_building[bp->bid];
	b->m_on = bp->on;
	b->m_type = bp->type;
	b->m_state = bp->state;
	b->m_pos = bp->pos;
	b->m_yaw = bp->yaw;
	for(int i=0; i<RESOURCES; i++)
	{
		b->m_conmat[i] = bp->conmat[i];
		b->m_stock[i] = bp->stock[i];
	}
	b->m_conwage = bp->conwage;
	
	if(g_mode == MENU)
	{
		g_loadbytes += sizeof(JoinInfoPacket);
		char msg[128];
		sprintf(msg, "Received %d bytes...", g_loadbytes);
		MessageBlock(msg, true);
	}
}

void ReadGarrisonPacket(GarrisonPacket* gp)
{
	/*
struct GarrisonPacket
{
	PacketHeader header;
	int planetid;
	int bid;
	int uid;
};*/

	Planet* p = &g_planet[gp->planetid];
	Building* b = &p->m_building[gp->bid];
	b->m_garrison.push_back(gp->uid);

	if(g_mode == MENU)
	{
		g_loadbytes += sizeof(JoinInfoPacket);
		char msg[128];
		sprintf(msg, "Received %d bytes...", g_loadbytes);
		MessageBlock(msg, true);
	}
}


void ReadJoinInfoPacket(JoinInfoPacket* jip)
{
	/*
struct JoinInfoPacket
{
	PacketHeader header;
	int playerid;
	Vec2i hmapwidth[PLANETS];
};*/

	g_localplayer = jip->playerid;

	for(int i=0; i<PLANETS; i++)
	{
		Planet* p = &g_planet[i];
		p->allocate(jip->hmapwidth[i].x, jip->hmapwidth[i].y);
	}
	
	if(g_mode == MENU)
	{
		g_loadbytes += sizeof(JoinInfoPacket);
		char msg[128];
		sprintf(msg, "Received %d bytes...", g_loadbytes);
		MessageBlock(msg, true);
	}
}

void ReadDoneLoadingPacket(DoneLoadingPacket* dlp)
{
	g_loadbytes = 0;

	for(int i=0; i<PLANETS; i++)
	{
		Planet* p = &g_planet[i];
		p->m_hmap.remesh();
	}

	g_spaceview = true;
	g_spacecam.position(0,0,0, 100,100,100, 0,1,0);
	g_camera = g_spacecam;
	g_mode = PLAY;
	OpenSoleView("spaceview");
}


void ReadUsernameExistsPacket(UsernameExistsPacket* uep)
{
	MessageBlock("Error: username already registered", true);
	ShowMessageBlockContinue(true, Continue_ToRegister);
}

void ReadEmailExistsPacket(EmailExistsPacket* eep)
{
	MessageBlock("Error: email already registered", true);
	ShowMessageBlockContinue(true, Continue_ToRegister);
}

void ReadIncorrectLoginPacket(IncorrectLoginPacket* ilp)
{
	//g_log<<"oinc li"<<endl;
	//g_log.flush();

	MessageBlock("Error: incorrect username or password", true);
	ShowMessageBlockContinue(true, Continue_ToLogin);
}

void ReadIncorrectVersionPacket(IncorrectVersionPacket* ivp)
{
	char msg[128];
	sprintf(msg, "Error: your client version (%0.2f) doesn't match server (%0.2f)", VERSION, ivp->version);
	MessageBlock(msg, true);
	ShowMessageBlockContinue(true, Continue_ToLogin);
}

void ReadLoginCorrectPacket(LoginCorrectPacket* lcp)
{
	MessageBlock("Login correct", true);
	ShowMessageBlockContinue(false, Continue_ToLogin);
}

void ReadTooManyClientsPacket(TooManyClientsPacket* tmcp)
{
	MessageBlock("Error: too many clients connected right now", true);
	ShowMessageBlockContinue(true, Continue_ToLogin);
}

void ReadRegDBErrorPacket(RegDBErrorPacket* rdbep)
{
	MessageBlock("Error: database registration error", true);
	ShowMessageBlockContinue(true, Continue_ToRegister);
}

void ReadConnectionResetPacket(ConnectionResetPacket* crp)
{
	MessageBlock("Connection reset", true);
	ShowMessageBlockContinue(true, Continue_BackToMain);
}

void ReadDisconnectPacket(DisconnectPacket* dp)
{
	//player dp->id disconnected
}

#endif