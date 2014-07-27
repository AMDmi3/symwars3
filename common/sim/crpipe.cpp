#include "crpipe.h"
#include "../render/heightmap.h"
#include "../render/shader.h"
#include "../phys/collision.h"
#include "../render/model.h"
#include "../render/vertexarray.h"
#include "player.h"
#include "building.h"
#include "road.h"
#include "../math/hmapmath.h"
#include "buildingtype.h"
#include "building.h"
#include "../../game/gmain.h"
#include "../../game/gui/ggui.h"

char CrPipeTile::condtype()
{
	return CONDUIT_CRPIPE;
}