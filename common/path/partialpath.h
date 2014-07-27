#ifndef PARTIALPATH_H
#define PARTIALPATH_H

#include "../math/vec3i.h"
#include "../math/vec2i.h"
#include "../render/heightmap.h"
#include "../math/vec3f.h"
#include "../math/vec2i.h"
#include "../platform.h"

class Unit;
class Building;
class PathJob;
class PathNode;

void PartialPath(int utype, int umode, int cmstartx, int cmstartz, int target, int target2, int targtype,
				 std::list<Vec2i> *path, Vec2i *subgoal, Unit* thisu, Unit* ignoreu, Building* ignoreb,
				 int cmgoalx, int cmgoalz, int cmgoalminx, int cmgoalminz, int cmgoalmaxx, int cmgoalmaxz,
				 int maxsearch);

void IdentifySuccessors_QP(PathJob* pj, PathNode* node);

#endif
