#include "powl.h"
#include "../render/heightmap.h"
#include "../math/hmapmath.h"
#include "player.h"
#include "road.h"
#include "../render/shader.h"
#include "../../game/gui/ggui.h"
#include "buildingtype.h"
#include "../phys/collision.h"
#include "../../game/gmain.h"
#include "building.h"

char PowlTile::condtype()
{
	return CONDUIT_POWL;
}