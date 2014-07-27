#ifndef COLLISION_H
#define COLLISION_H

#include "../platform.h"

#define MAX_CLIMB_INCLINE		(TILE_SIZE)

#define COLLIDER_NONE		-1
#define COLLIDER_UNIT		0
#define COLLIDER_BUILDING	1
#define COLLIDER_TERRAIN	2
#define COLLIDER_NOROAD		3
#define COLLIDER_OTHER		4
#define COLLIDER_NOLAND		5
#define COLLIDER_NOSEA		6
#define COLLIDER_NOCOAST	7
#define COLLIDER_ROAD		8
#define COLLIDER_OFFMAP		9

extern int g_lastcollider;
extern int g_collidertype;
extern bool g_ignored;

class Unit;

bool BlAdj(int i, int j);
bool CoAdj(char ctype, int i, int x, int z);
bool CollidesWithBuildings(int minx, int minz, int maxx, int maxz, int ignore=-1);
bool CollidesWithUnits(int minx, int minz, int maxx, int maxz, bool isunit=false, Unit* thisu=NULL, Unit* ignore=NULL);

#endif
