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
#include "pathjob.h"

void ReconstructPath(PathJob* pj, PathNode* endnode)
{
#if 1
	// Reconstruct the path, following the path steps
	for(PathNode* n = endnode; n; n = n->previous)
	{
		Vec2i npos = PathNodePos(n);
		Vec2i cmpos( npos.x * PATHNODE_SIZE + PATHNODE_SIZE/2, npos.y * PATHNODE_SIZE + PATHNODE_SIZE/2 );
		pj->path->push_front(cmpos);
	}
#endif

#if 1
	pj->path->push_back(Vec2i(pj->cmgoalx, pj->cmgoalz));
#endif

	if(pj->path->size() > 0)
		*pj->subgoal = *pj->path->begin();
}
