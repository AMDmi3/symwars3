#include "pathnode.h"
#include "collidertile.h"
#include "../math/vec2i.h"
#include "../math/3dmath.h"
#include "../sim/unit.h"
#include "../sim/unittype.h"
#include "../sim/building.h"
#include "../sim/buildingtype.h"
#include "../render/heightmap.h"
#include "../math/hmapmath.h"
#include "../phys/collision.h"
#include "../sim/road.h"
#include "../render/water.h"
#include "../utils.h"
#include "../render/shader.h"
#include "../sim/selection.h"
#include "../sim/sim.h"
#include "../phys/trace.h"
#include "binheap.h"
#include "partialpath.h"
#include "reconstructpath.h"
#include "pathdebug.h"

void PartialPath(int utype, int umode, int cmstartx, int cmstartz, int target, int target2, int targtype,
				 std::list<Vec2i> *path, Vec2i *subgoal, Unit* thisu, Unit* ignoreu, Building* ignoreb,
				 int cmgoalx, int cmgoalz, int cmgoalminx, int cmgoalminz, int cmgoalmaxx, int cmgoalmaxz,
				 int maxsearch)
{
	UnitT* ut = &g_utype[utype];

	PathJob* pj = new PathJob;
	pj->utype = utype;
	pj->umode = umode;
	pj->cmstartx = cmstartx;
	pj->cmstartz = cmstartz;
	pj->target = target;
	pj->target2 = target2;
	pj->targtype = targtype;
	pj->path = path;
	pj->subgoal = subgoal;
	pj->thisu = thisu ? thisu - g_unit : -1;
	pj->ignoreu = ignoreu ? ignoreu - g_unit : -1;
	pj->ignoreb = ignoreb ? ignoreb - g_building : -1;
	pj->cmgoalx = (cmgoalminx+cmgoalmaxx)/2;
	pj->cmgoalz = (cmgoalminz+cmgoalmaxz)/2;
	pj->ngoalx = pj->cmgoalx / PATHNODE_SIZE;
	pj->ngoalz = pj->cmgoalz / PATHNODE_SIZE;
	pj->cmgoalminx = cmgoalminx;
	pj->cmgoalminz = cmgoalminz;
	pj->cmgoalmaxx = cmgoalmaxx;
	pj->cmgoalmaxz = cmgoalmaxz;
	pj->roaded = ut->roaded;
	pj->landborne = ut->landborne;
	pj->seaborne = ut->seaborne;
	pj->airborne = ut->airborne;
	pj->callback = Callback_UnitPath;
	pj->pjtype = PATHJOB_QUICKPARTIAL;
	pj->maxsearch = maxsearch;

	// Returns the path from location `<startX, startY>` to location `<endX, endY>`.
	//return function(finder, startNode, endNode, clearance, toClear)

	pj->process();
	delete pj;
}

void IdentifySuccessors_QP(PathJob* pj, PathNode* node)
{
	Vec2i npos = PathNodePos(node);

	int thisdistance = Magnitude2(Vec2i(npos.x - pj->ngoalx, npos.y - pj->ngoalz));

	if( !pj->closestnode || thisdistance < pj->closest )
	{
		pj->closestnode = node;
		pj->closest = thisdistance;
	}

	int runningD = 0;

	if(node->previous)
		runningD = node->previous->totalD;

	bool standable[DIRS];

	for(int i=0; i<DIRS; i++)
		standable[i] = Standable(pj, npos.x + offsets[i].x, npos.y + offsets[i].y);

	bool passable[DIRS];

	passable[DIR_NW] = standable[DIR_NW] && standable[DIR_N] && standable[DIR_W];
	passable[DIR_N] = standable[DIR_N];
	passable[DIR_NE] = standable[DIR_NE] && standable[DIR_N] && standable[DIR_E];
	passable[DIR_E] = standable[DIR_E];
	passable[DIR_SE] = standable[DIR_SE] && standable[DIR_S] && standable[DIR_E];
	passable[DIR_S] = standable[DIR_S];
	passable[DIR_SW] = standable[DIR_SW] && standable[DIR_S] && standable[DIR_W];
	passable[DIR_W] = standable[DIR_W];

	for(int i=0; i<DIRS; i++)
	{
		if(!passable[i])
			continue;

		int newD = runningD + stepdist[i];

		Vec2i nextnpos(npos.x + offsets[i].x, npos.y + offsets[i].y);
		PathNode* nextn = PathNodeAt(nextnpos.x, nextnpos.y);

		if(!nextn->closed && (!nextn->opened || newD < nextn->totalD))
		{
			g_toclear.push_back(nextn); // Records this node to reset its properties later.
			nextn->totalD = newD;
			int H = Manhattan( nextnpos - Vec2i(pj->ngoalx, pj->ngoalz) );
			nextn->F = nextn->totalD + H;
			nextn->previous = node;

			if( !nextn->opened )
			{
				g_openlist.insert(nextn);
				nextn->opened = true;
			}
			else
			{
				g_openlist.heapify(nextn);
			}
		}
	}
}
