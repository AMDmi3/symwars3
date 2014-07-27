#ifndef CRPIPE_H
#define CRPIPE_H

#include "connectable.h"
#include "resources.h"
#include "../render/model.h"
#include "../render/vertexarray.h"
#include "../math/vec3i.h"
#include "infrastructure.h"

class CrPipeTile : public ConduitTile
{
public:
	char condtype();
};

#endif

