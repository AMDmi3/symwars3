#ifndef RESOURCES_H
#define RESOURCES_H

#include "../platform.h"

class Resource
{
public:
	char icon;
	bool physical;
	bool capacity;
	bool global;
	std::string name;
	float rgba[4];
	std::string depositn;
	int conduit;
};

//conduit
#define CONDUIT_NONE		-1
#define CONDUIT_ROAD		0
#define CONDUIT_POWL		1
#define	CONDUIT_CRPIPE		2

#define RES_NONE			-1
#define RES_FUNDS			0
#define RES_LABOUR			1
#define RES_HOUSING			2
#define RES_FARMPRODUCTS	3
#define RES_RETFOOD			4
#define RES_PRODUCTION		5
#define RES_MINERALS		6
#define RES_CRUDEOIL		7
#define RES_WSFUEL			8
#define RES_RETFUEL			9
#define RES_ENERGY			10
#define RES_URANIUM			11
#define RESOURCES			12
extern Resource g_resource[RESOURCES];

class Basket
{
public:
	int r[RESOURCES];
};

class Bundle
{
public:
	unsigned char res;
	int amt;
};

void DefR(int resi, const char* n, const char* depn, int iconindex, bool phys, bool cap, bool glob, float r, float g, float b, float a, int conduit);
void Zero(int *b);
bool ResB(int building, int res);

#endif
