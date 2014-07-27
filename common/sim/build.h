#ifndef BUILD_H
#define BUILD_H

#include "../math/vec3f.h"
#include "../math/vec2i.h"

class Building;
class BuildingT;
class Matrix;

void DrawSBuild();
void UpdateSBuild();
bool CheckCanPlace(int type, Vec2i pos);
void DrawBReason(Matrix* mvp, float width, float height, bool persp);
bool PlaceBuilding(int type, Vec2i pos, bool finished, int owner, int* bid);
bool PlaceBAb(int btype, Vec2i tabout, Vec2i* tplace);
bool PlaceUAb(int utype, Vec2i cmabout, Vec2i* cmplace);

#endif
