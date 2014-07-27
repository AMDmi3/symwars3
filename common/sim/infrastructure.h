#ifndef INFRASTRUCTURE_H
#define INFRASTRUCTURE_H

#include "connectable.h"
#include "resources.h"
#include "../render/model.h"
#include "../render/vertexarray.h"
#include "../math/vec3i.h"
#include "../render/heightmap.h"

#define CONDUIT_ROAD	0
#define CONDUIT_POWL	1
#define CONDUIT_CRPIPE	2
#define CONDUIT_TYPES	3

class ConduitTile
{
public:
	bool on;
	char conntype;
	bool finished;
	char owner;
	int conmat[RESOURCES];
	short netw;	//network
	VertexArray drawva;
	//bool inaccessible;
	short transporter[RESOURCES];
	Vec3f drawpos;
	//int maxcost[RESOURCES];

	ConduitTile();
	~ConduitTile();

	virtual char condtype();
	int netreq(int res);
	void destroy();
	void allocate();
	bool checkconstruction();
	virtual void fillcollider();
	virtual void freecollider();
};

class ConduitType
{
public:
	int conmat[RESOURCES];
	unsigned short netwoff;	//offset to network list in Building class
	unsigned short seloff;	//offset to selection list in Selection class
	int model[CONNECTION_TYPES][2];	//0 = not finished, 1 = finished/constructed
	unsigned short maxforwincl;
	unsigned short maxsideincl;
	bool blconduct;	//do buildings conduct this resource (also act as conduit in a network?)
	Vec2i physoff;	//offset in cm
	Vec3f drawoff;	//offset in cm
	ConduitTile* cotiles[2];	//0 = actual placed, 1 = plan proposed
	bool cornerpl;	//is the conduit centered on corners or tile centers?

	ConduitType()
	{
		Zero(conmat);
		blconduct = false;
		cotiles[0] = NULL;
		cotiles[1] = NULL;
		cornerpl = false;
	}

	~ConduitType()
	{
		for(int i=0; i<2; i++)
			if(cotiles[i])
			{
				delete [] cotiles[i];
				cotiles[i] = NULL;
			}
	}
};

extern ConduitType g_cotype[CONDUIT_TYPES];

inline ConduitTile* GetCo(char ctype, int tx, int tz, bool plan)
{
	ConduitType* ct = &g_cotype[ctype];
	ConduitTile* tilesarr = ct->cotiles[(int)plan];
	return &tilesarr[ tx + tz*g_hmap.m_widthx ];
}

class Building;

void DefCo(char ctype, 
		   unsigned short netwoff, 
		   unsigned short seloff, 
		   unsigned short maxforwincl, 
		   unsigned short maxsideincl, 
		   bool blconduct, 
		   bool cornerpl, 
		   Vec2i physoff, 
		   Vec3f drawoff);
void CoConMat(char ctype, char rtype, short ramt);
void UpdCoPlans(char ctype, char owner, Vec3f start, Vec3f end);
void ClearCoPlans(char ctype);
void ReNetw(char ctype);
void ResetNetw(char ctype);
bool ReNetwB(char ctype);
void MergeNetw(char ctype, int A, int B);
bool ReNetwTiles(char ctype);
bool CompareCo(char ctype, ConduitTile* ctile, int tx, int tz);
bool BAdj(char ctype, int i, int tx, int tz);
bool CompareB(char ctype, Building* b, ConduitTile* ctile);
bool CoLevel(char ctype, float iterx, float iterz, float testx, float testz, float dx, float dz, int i, float d, bool plantoo);
void RemeshCo(char ctype, int tx, int tz, bool plan);
void PlaceCo(char ctype);
void PlaceCo(char ctype, int tx, int tz, int owner, bool plan);
void Repossess(char ctype, int tx, int tz, int owner);
void DrawCo(char ctype);
void CoXZ(char ctype, ConduitTile* ctile, bool plan, int& tx, int& tz);
void DefConn(char conduittype, char connectiontype, bool finished, const char* modelfile, const Vec3f scale, Vec3f transl);
void PruneCo(char ctype);

#endif
