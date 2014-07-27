#ifndef RECONSTRUCTPATH_H
#define RECONSTRUCTPATH_H

#include "../platform.h"
#include "../math/vec2i.h"
#include "pathjob.h"

class PathNode;

void ReconstructPath(std::list<Vec2i> &path, PathNode* bestS, Vec2i &subgoal, int cmgoalx, int cmgoalz);
void ReconstructPathJPS(std::list<Vec2i> &path, PathNode* bestS, Vec2i &subgoal, int cmgoalx, int cmgoalz);
void ReconstructPath(PathJob* pj, PathNode* endnode);

#endif
