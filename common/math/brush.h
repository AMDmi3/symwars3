#ifndef BRUSH_H
#define BRUSH_H

#include "vec3f.h"
#include "plane3f.h"
#include "polygon.h"
#include "triangle.h"

void MakeHull(Vec3f* norms, float* ds, const Vec3f pos, const float radius, const float height);
void MakeHull(Vec3f* norms, float* ds, const Vec3f pos, const float hwx, const float hwz, const float height);
void MakeHull(Vec3f* norms, float* ds, const Vec3f pos, const Vec3f vmin, const Vec3f vmax);
bool LineInterHull(const Vec3f* line, const Vec3f* norms, const float* ds, const int numplanes);
bool LineInterHull(const Vec3f* line, Plane3f* planes, const int numplanes, Vec3f* intersection);
bool LineInterHull(const Vec3f* line, const Vec3f* norms, const float* ds, const int numplanes, Vec3f* intersection);

#endif
