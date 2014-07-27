#ifndef PATHNODE_H
#define PATHNODE_H

#include "../platform.h"
#include "../math/vec2i.h"
#include "../render/heightmap.h"

#define PATHNODE_SIZE	(TILE_SIZE/8)
#define PATHNODE_DIAG	(sqrt(PATHNODE_SIZE*PATHNODE_SIZE*2))

// Offsets for straights moves
const Vec2i straightoffsets[4] =
{
	Vec2i(1, 0), //E
	Vec2i(-1, 0), //W
	Vec2i(0, 1), //S
	Vec2i(0, -1) //N
};

// Offsets for diagonal moves
const Vec2i diagonaloffsets[4] =
{
	Vec2i(-1, -1), //NW
	Vec2i(1, -1), //NE
	Vec2i(-1, 1), //SW
	Vec2i(1, 1) //SE
};

#define DIR_NW      0
#define DIR_N       1
#define DIR_NE      2
#define DIR_E       3
#define DIR_SE      4
#define DIR_S       5
#define DIR_SW      6
#define DIR_W       7
#define DIRS        8

const Vec2i offsets[DIRS] =
{
	Vec2i(-1, -1), //NW
	Vec2i(0, -1), //N
	Vec2i(1, -1), //NE
	Vec2i(1, 0), //E
	Vec2i(1, 1), //SE
	Vec2i(0, 1), //S
	Vec2i(-1, 1), //SW
	Vec2i(-1, 0) //W
};

const int stepdist[DIRS] =
{
	(int)PATHNODE_DIAG, //NW
	(int)PATHNODE_SIZE, //N
	(int)PATHNODE_DIAG, //NE
	(int)PATHNODE_SIZE, //E
	(int)PATHNODE_DIAG, //SE
	(int)PATHNODE_SIZE, //S
	(int)PATHNODE_DIAG, //SW
	(int)PATHNODE_SIZE //W
};

// byte-align structures
#pragma pack(push, 1)

class PathNode
{
public:
	int F;
	short nx;
	short nz;
	int totalD;
	//unsigned char expansion;
	PathNode* previous;
	//bool tried;
	bool opened;
	bool closed;
	PathNode()
	{
		//tried = false;
		previous = NULL;
		opened = false;
		closed = false;
	};
	PathNode(int startx, int startz, int endx, int endz, int nx, int nz, PathNode* prev, int totalD, int stepD);
	//PathNode(int startx, int startz, int endx, int endz, int nx, int nz, PathNode* prev, int totalD, int stepD, unsigned char expan);
};

#pragma pack(pop)

class Heap;
class PathJob;

extern Vec2i g_pathdim;
extern PathNode* g_pathnode;
extern Heap g_openlist;

Vec2i PathNodePos(PathNode* node);
PathNode* PathNodeAt(int nx, int nz);
int PathNodeIndex(int nx, int nz);
bool AtGoal(PathJob* pj, PathNode* node);
void SnapToNode(PathJob* pj);
void ResetPathNodes();

#endif
