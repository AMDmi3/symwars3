#ifndef SIM_H
#define SIM_H

#include "../render/heightmap.h"
#include "../platform.h"
#include "../texture.h"
#include "../render/model.h"
#include "../sim/resources.h"
#include "../gui/icon.h"
#include "../sim/buildingtype.h"
#include "../sim/unittype.h"
#include "../render/foliage.h"
#include "../sim/road.h"
#include "../sim/powl.h"
#include "../sim/crpipe.h"
#include "../render/water.h"
#include "../sim/selection.h"
#include "../sound/sound.h"
#include "../render/particle.h"
#include "../render/skybox.h"
#include "../sim/player.h"
#include "../gui/sprite.h"
#include "../gui/cursor.h"

extern long long g_simframe;

#define SIM_FRAME_RATE		30

#define STARTING_RETFOOD		9000	//lasts for 5 minutes
#define STARTING_LABOUR			1000	//lasts for 33.333 seconds
#define LABOURER_FOODCONSUM		1
#define LABOURER_ENERGYCONSUM	3
#define DRIVE_WORK_DELAY		(SIM_FRAME_RATE*60)	//in sim frames
#define WORK_DELAY				(SIM_FRAME_RATE*1)	//in sim frames
#define LOOKJOB_DELAY_MAX		(SIM_FRAME_RATE*5)
#define FUEL_DISTANCE			(TILE_SIZE)		//(TILE_SIZE*2.0f)

void Queue();

#endif
