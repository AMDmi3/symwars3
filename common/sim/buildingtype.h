#ifndef BUILDINGTYPE_H
#define BUILDINGTYPE_H

#include "../math/vec3f.h"
#include "../math/vec2i.h"
#include "resources.h"
#include "../render/billboard.h"
#include "../render/particle.h"
#include "infrastructure.h"

#define MAX_B_EMITTERS	10

class BuildingT
{
public:
	// width in tiles
	int widthx;
	int widthz;

	int model;
	int cmodel;

	char name[64];

	int foundation;

	int input[RESOURCES];
	int output[RESOURCES];

	int conmat[RESOURCES];

	int reqdeposit;

	EmitterPlace emitterpl[MAX_B_EMITTERS];

	bool hugterr;

	BuildingT();
};

#define FOUNDATION_LAND			0
#define FOUNDATION_COASTAL		1
#define FOUNDATION_SEA			2

#define BUILDING_NONE			-1
#define BUILDING_APARTMENT		0
#define BUILDING_FACTORY		1
#define BUILDING_REFINERY		2
#define BUILDING_NUCPOW			3
#define BUILDING_FARM			4
#define BUILDING_STORE			5
#define BUILDING_HARBOUR		6
#define BUILDING_OILWELL		7
#define BUILDING_MINE			8
#define BUILDING_GASSTATION		9
#define BUILDING_TYPES			10

#define BUILDING_ROAD			(BUILDING_TYPES+CONDUIT_ROAD)
#define BUILDING_POWL			(BUILDING_TYPES+CONDUIT_POWL)
#define BUILDING_CRPIPE			(BUILDING_TYPES+CONDUIT_CRPIPE)
//#define BUILDING_WATERPIPE		(BUILDING_TYPES+4)

#define TOTAL_BUILDABLES		(BUILDING_TYPES+CONDUIT_TYPES)

extern BuildingT g_bltype[BUILDING_TYPES];

void DefB(int type, const char* name, Vec2i size, bool hugterr, const char* modelrelative, Vec3f scale, Vec3f translate, const char* cmodelrelative,  Vec3f cscale, Vec3f ctranslate, int foundation, int reqdeposit);
void BConMat(int type, int res, int amt);
void BInput(int type, int res, int amt);
void BOutput(int type, int res, int amt);
void BEmitter(int type, int emitterindex, int ptype, Vec3f offset);

#endif
