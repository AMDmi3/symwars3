#ifndef DMDMODEL_H
#define DMDMODEL_H

#include "vertexarray.h"

#define BUILDINGM_VERSION                1.0f

#define TAG_BUILDINGM                {'D', 'M', 'D', 'M', 'L'}

bool LoadDMDModel(VertexArray** va, unsigned int& diffusem, unsigned int& specularm, unsigned int& normalm, unsigned int& ownerm, Vec3f scale, Vec3f translate, const char* relative, bool dontqueue);

#endif
