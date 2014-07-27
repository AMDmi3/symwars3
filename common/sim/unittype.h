#ifndef UNITTYPE_H
#define UNITTYPE_H

#include "../math/vec3i.h"
#include "../math/vec2i.h"
#include "../render/heightmap.h"
#include "resources.h"
#include "../math/vec3f.h"

class UnitT
{
public:
#if 0
	unsigned int texindex;
	Vec2i bilbsize;
#endif
	int model;
	Vec3i size;
	char name[64];
	int starthp;
	int cmspeed;
	int cost[RESOURCES];
	bool walker;
	bool landborne;
	bool roaded;
	bool seaborne;
	bool airborne;
	bool military;
};


#define UNIT_ROBOSOLDIER	0
#define UNIT_LABOURER		1
#define UNIT_TRUCK			2
#define UNIT_TYPES			3

extern UnitT g_utype[UNIT_TYPES];

void DefU(int type, const char* modelrelative, Vec3f scale, Vec3f translate, Vec3i size, const char* name, int starthp, bool landborne, bool walker, bool roaded, bool seaborne, bool airborne, int cmspeed, bool military);

#endif
