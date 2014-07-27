#ifndef SELECTION_H
#define SELECTION_H

#include "../platform.h"
#include "../math/vec2i.h"
#include "../math/vec3f.h"

class Selection
{
public:
	std::list<int> units;
	std::list<int> buildings;
	std::list<Vec2i> roads;
	std::list<Vec2i> powls;
	std::list<Vec2i> crpipes;

	void clear();
};

extern unsigned int g_circle;

Selection DoSel(Vec3f campos, Vec3f camside, Vec3f camup2, Vec3f viewdir);
void DrawSel(Matrix* projection, Matrix* modelmat, Matrix* viewmat);
void DrawMarquee();
void ClearSel(Selection* s);
void AfterSel(Selection* s);

#endif
