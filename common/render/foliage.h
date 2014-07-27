#ifndef FOLIAGE_H
#define FOLIAGE_H

#include "../math/3dmath.h"
#include "../math/vec3f.h"
#include "../math/vec3i.h"
#include "../math/matrix.h"
#include "vertexarray.h"

class FoliageT
{
public:
	char name[64];
	Vec3i size;
	unsigned int texindex;
	int model;
};


#define FOLIAGE_TREE1		0
#define FOLIAGE_TREE2		1
#define FOLIAGE_TREE3		2
#if 0
#define FOLIAGE_BUSH1		3
#define FOLIAGE_TYPES		4
#else
#define FOLIAGE_TYPES		3
#endif

extern FoliageT g_foliageT[FOLIAGE_TYPES];

// byte-align structures
#pragma pack(push, 1)
class Foliage
{
public:
	bool on;
	unsigned char type;
	Vec3f pos;
	float yaw;

	Foliage();
	void reinstance();
};
#pragma pack(pop)

//#define FOLIAGES	128
#define FOLIAGES	2048
//#define FOLIAGES	6000
//#define FOLIAGES	30000
//#define FOLIAGES	240000

extern Foliage g_foliage[FOLIAGES];

void DefF(int type, const char* modelrelative, Vec3f scale, Vec3f translate, Vec3i size);
bool PlaceFoliage(int type, Vec3i ipos);
void DrawFoliage(Vec3f zoompos, Vec3f vertical, Vec3f horizontal);
void ClearFol(int minx, int minz, int maxx, int maxz);
void FreeFoliage();
void FillForest();
#endif
