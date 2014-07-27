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
#include "pathjob.h"

// Calculates a path.
void JPSPartPath(int utype, int umode, int cmstartx, int cmstartz, int target, int target2, int targtype,
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
	pj->pjtype = PATHJOB_JPSPART;
	pj->maxsearch = maxsearch;

	int sqmax = sqrt(maxsearch);
	int diagw = (sqmax - 1) / 2;

	pj->maxsubdiag = diagw;
	pj->maxsubdiagstraight = diagw;
	pj->maxsubstraight = diagw;

	// Returns the path from location `<startX, startY>` to location `<endX, endY>`.
	//return function(finder, startNode, endNode, clearance, toClear)

	pj->process();
	delete pj;
}
