

#ifndef PACKETS_H
#define PACKETS_H

#include "../platform.h"
#include "../utils.h"
#include "../debug.h"
#include "../sim/player.h"
#include "../math/vec2i.h"
#include "../math/camera.h"
#include "../sim/planet.h"

class OldPacket
{
public:
	char* buffer;
	int len;
	long long last;
	long long first;
#ifdef _SERVER
	struct sockaddr_in addr;
#endif
	
	void freemem()
	{
		if(buffer != NULL)
			delete [] buffer;
		buffer = NULL;
	}
	
	OldPacket()
	{
		buffer = NULL;
	}
	~OldPacket()
	{
		freemem();
	}

	OldPacket(const OldPacket& original)
	{
		*this = original;
	}

	OldPacket& operator=(const OldPacket &original)
	{
		if(original.buffer && original.len > 0)
		{
			len = original.len;
			buffer = new char[len];
			memcpy((void*)buffer, (void*)original.buffer, len);
			last = original.last;
			first = original.first;
#ifdef _SERVER
			memcpy((void*)&addr, (void*)&original.addr, sizeof(struct sockaddr_in));
#endif
		}
		else
		{
			buffer = NULL;
			len = 0;
		}

		return *this;
	}
};

#define     PACKET_LOGIN					1
#define     PACKET_INCORRECT_LOGIN			2
#define     PACKET_INCORRECT_VERSION		3
#define     PACKET_REGISTRATION				4
#define     PACKET_USERNAME_EXISTS			5
#define     PACKET_EMAIL_EXISTS				6
#define		PACKET_ACKNOWLEDGMENT			7
#define		PACKET_REGISTRATION_DONE		8
#define		PACKET_LOGIN_CORRECT			9
#define		PACKET_TOO_MANY_CLIENTS			10
#define		PACKET_REG_DB_ERROR				11
//#define		REG_SUCCESS					12
#define		PACKET_DISCONNECT				13
#define		PACKET_CONNECTION_RESET			14
#define		PACKET_SPAWN_UNIT				15
#define		PACKET_NEW_PLAYER				16
#define		PACKET_HEIGHT_POINT				17
#define		PACKET_DONE_LOADING				18
//#define		PACKET_EXPLORED_TILE			19
#define		PACKET_PLANET_CAM				20
#define		PACKET_BUILDING					21
#define		PACKET_JOIN_INFO				22
#define		PACKET_GARRISON					23
#define		PACKET_PLAYER					24

struct PacketHeader
{
	int type;
	unsigned int ack;
};

struct LoginPacket
{
	PacketHeader header;
	float version;
	char username[UN_LEN];
	char password[PW_LEN];
};

struct IncorrectLoginPacket
{
	PacketHeader header;
};

struct IncorrectVersionPacket
{
	PacketHeader header;
	float version;
};

struct RegistrationPacket
{
	PacketHeader header;
	char username[UN_LEN];
	char email[EM_LEN];
	char password[PW_LEN];
};

struct UsernameExistsPacket
{
	PacketHeader header;
};

struct EmailExistsPacket
{
	PacketHeader header;
};

struct AcknowledgmentPacket
{
	PacketHeader header;
};

struct RegisteredPacket
{
	PacketHeader header;
};

struct LoginCorrectPacket
{
	PacketHeader header;
};

struct TooManyClientsPacket
{
	PacketHeader header;
};

struct RegDBErrorPacket
{
	PacketHeader header;
};

/*
struct RegSuccessPacket
{
	PacketHeader header;
};*/

struct DisconnectPacket
{
	PacketHeader header;
	int id;
};

struct ConnectionResetPacket
{
	PacketHeader header;
};

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

struct NewPlayerPacket
{
	PacketHeader header;
	int playerid;
	char username[UN_LEN];
	char email[EM_LEN];
	int local[RESOURCES];
	int universal[RESOURCES];
};

struct HeightPointPacket
{
	PacketHeader header;
	int planetid;
	int tx;
	int tz;
	float height;
};

struct DoneLoadingPacket
{
	PacketHeader header;
};

/*
struct ExploredTilePacket
{
	PacketHeader header;
	int planetid;
	int tx;
	int tz;
};*/

struct PlanetCamPacket
{
	PacketHeader header;
	int planetid;
	Camera cam;
};

struct JoinInfoPacket
{
	PacketHeader header;
	int playerid;
	Vec2i hmapwidth[PLANETS];
};

/*
	bool m_on;
	int m_type;
	int m_state;
	Vec3f m_pos;
	//EdBuilding m_model;
	float m_yaw;
	list<int> m_garrison;
	TileVis m_vis;
	int m_conmat[RESOURCES];
	int m_stock[RESOURCES];
	int m_conwage;	// construction wage
	*/

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
	int conwage;	// construction wage
};

struct GarrisonPacket
{
	PacketHeader header;
	int planetid;
	int bid;
	int uid;
};

/*
	bool m_on;
	int m_local[RESOURCES];	// used just for counting; cannot be used
	int m_universal[RESOURCES];
	int m_reschange[RESOURCES];
	int m_conwage;	// construction wage
	int m_truckwage;
	int m_unitprice[UNIT_TYPES];
	char m_username[UN_LEN];*/

struct PlayerPacket
{
	PacketHeader header;
	int playerid;
	bool on;
	int local[RESOURCES];
	int universal[RESOURCES];
	int reschange[RESOURCES];
	//int conwage;
	//int truckwage;

};

#endif



