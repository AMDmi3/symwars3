#ifndef PATHJOB_H
#define PATHJOB_H

#include "../platform.h"
#include "../math/vec2i.h"

class Unit;
class Building;
class PathNode;

#define PATH_DELAY		200
//#define PATH_DELAY		0

#define PATHJOB_JPS					0
#define PATHJOB_QUICKPARTIAL		1
#define PATHJOB_ASTAR				2
#define PATHJOB_JPSPART				3

// byte-align structures
#pragma pack(push, 1)

class PathJob
{
public:
	unsigned char utype;
	unsigned char umode;
	int cmstartx;
	int cmstartz;
	int target;
	int target2;
	int targtype;
	std::list<Vec2i> *path;
	Vec2i *subgoal;
	short thisu;
	short ignoreu;
	short ignoreb;
	int cmgoalx;
	int cmgoalz;
	short ngoalx;
	short ngoalz;
	int cmgoalminx;
	int cmgoalminz;
	int cmgoalmaxx;
	int cmgoalmaxz;
	bool roaded;
	bool landborne;
	bool seaborne;
	bool airborne;
	int maxsearch;
	unsigned char pjtype;
	PathNode* closestnode;
	int closest;
	int searchdepth;
	int maxsubsearch;
	int maxsubstraight;
	int maxsubdiag;
	int maxsubdiagstraight;
	int subsearchdepth;
	void (*callback)(bool result, PathJob* pj);

	virtual bool process();
};
#pragma pack(pop)

extern std::list<PathNode*> g_toclear;

void Callback_UnitPath(bool result, PathJob* pj);

#endif
