#include "pathjob.h"
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
#include "jpspath.h"
#include "reconstructpath.h"
#include "pathdebug.h"
#include "jpsexpansion.h"
#include "../window.h"
#include "partialpath.h"

long long g_lastpath = 0;
std::list<PathNode*> g_toclear;

void ClearNodes(std::list<PathNode*> &toclear)
{
	for(auto niter = toclear.begin(); niter != toclear.end(); niter++)
	{
		PathNode* n = *niter;
		n->opened = false;
		n->closed = false;
	}

	toclear.clear();
}

bool PathJob::process()
{
#if 0
	long long frames = g_simframe - g_lastpath;

	int delay = frames * 1000 / DRAW_FRAME_RATE;

	if(delay < PATH_DELAY)
		return false;

	g_lastpath = g_simframe;
#endif

	ResetPathNodes();

	SnapToNode(this);

	PathNode* node;
	searchdepth = 0;
	closest = 0;
	closestnode = NULL;

	while( g_openlist.hasmore() )
	{
		searchdepth ++;

		if((pjtype == PATHJOB_QUICKPARTIAL || pjtype == PATHJOB_JPSPART) && searchdepth > maxsearch)
			break;

		// Pops the lowest F-cost node, moves it in the closed std::list
		node = g_openlist.deletemin();
		int i = node - g_pathnode;

		Vec2i npos = PathNodePos(node);

		node->closed = true;

		// If the popped node is the endNode, return it
		if( AtGoal(this, node) )
		{
			ReconstructPath(this, node);
			ClearNodes(g_toclear);

			if(callback)
				callback(true, this);

			return true;
		}

		// otherwise, identify successors of the popped node
		if(pjtype == PATHJOB_JPS || pjtype == PATHJOB_JPSPART)
			IdentifySuccessors_JPS(this, node);
		else if(pjtype == PATHJOB_QUICKPARTIAL)
			IdentifySuccessors_QP(this, node);
	}

	bool pathfound = false;

	if((pjtype == PATHJOB_QUICKPARTIAL || pjtype == PATHJOB_JPSPART) && closestnode)
	{
		pathfound = true;
		ReconstructPath(this, closestnode);
	}

	ClearNodes(g_toclear);

	if(callback)
		callback(pathfound, this);

	return true;
}

void Callback_UnitPath(bool result, PathJob* pj)
{
	short ui = pj->thisu;

	if(ui < 0)
		return;

	Unit* u = &g_unit[ui];

	u->threadwait = false;
	u->pathblocked = !result;
}
