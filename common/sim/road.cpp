#include "road.h"
#include "../render/heightmap.h"
#include "player.h"
#include "../render/shader.h"
#include "../math/hmapmath.h"
#include "../render/foliage.h"
#include "../phys/collision.h"
#include "../math/3dmath.h"
#include "building.h"
#include "buildingtype.h"
#include "../../game/gmain.h"
#include "../render/water.h"
#include "../math/polygon.h"
#include "../../game/gui/ggui.h"
#include "../utils.h"
#include "../path/pathnode.h"
#include "../path/collidertile.h"

char RoadTile::condtype()
{
	return CONDUIT_ROAD;
}

void RoadTile::freecollider()
{
	// This can only be called by an actual (non-plan) road tile,
	// otherwise it will go out of array bounds!

	Vec2i t;
	CoXZ(CONDUIT_ROAD, this, false, t.x, t.y);

	int cmminx = t.x * TILE_SIZE;
	int cmminz = t.y * TILE_SIZE;
	int cmmaxx = (t.x+1) * TILE_SIZE - 1;
	int cmmaxz = (t.y+1) * TILE_SIZE - 1;

	int nminx = cmminx / PATHNODE_SIZE;
	int nminz = cmminz / PATHNODE_SIZE;
	int nmaxx = cmmaxx / PATHNODE_SIZE;
	int nmaxz = cmmaxz / PATHNODE_SIZE;

	for(int x=nminx; x<=nmaxx; x++)
		for(int z=nminz; z<=nmaxz; z++)
		{
			ColliderTile* c = ColliderTileAt(x, z);
			//c->hasroad = false;
			c->flags &= ~FLAG_HASROAD;
		}

#if 0
	int roadx, roadz;

	RoadXZ(this, roadx, roadz);
	Vec3f pos = RoadPosition(roadx, roadz);

	//BuildingT* bt = &g_bltype[type];
	float hwx = TILE_SIZE/2.0f;
	float hwz = TILE_SIZE/2.0f;

	float fstartx = pos.x-hwx;
	float fstartz = pos.z-hwz;
	float fendx = pos.x+hwx;
	float fendz = pos.z+hwz;

	//int extentx = ceil(r/cellwx);
	//int extentz = ceil(r/cellwz);
	const float cellwx = MIN_RADIUS*2.0f;
	const float cellwz = MIN_RADIUS*2.0f;

	int startx = fstartx/cellwx;
	int startz = fstartz/cellwz;
	int endx = ceil(fendx/cellwx);
	int endz = ceil(fendz/cellwz);
	//int endx = startx+extentx;
	//int endz = startz+extentz;

	for(int x=startx; x<endx; x++)
		for(int z=startz; z<endz; z++)
		{
			bool found = false;
			ColliderTile* cell = ColliderTileAt(x, z);
			for(int i=0; i<cell->colliders.size(); i++)
			{
				Collider* c = &cell->colliders[i];
				if(c->type == COLLIDER_NOROAD)
				{
					found = true;
					break;
				}
			}

			if(!found)
				cell->colliders.push_back(Collider(COLLIDER_NOROAD, -1));
		}
#endif
}

void RoadTile::fillcollider()
{
	// This can only be called by an actual (non-plan) road tile,
	// otherwise it will go out of array bounds!
	
	Vec2i t;
	CoXZ(CONDUIT_ROAD, this, false, t.x, t.y);

	int cmminx = t.x * TILE_SIZE;
	int cmminz = t.y * TILE_SIZE;
	int cmmaxx = (t.x+1) * TILE_SIZE - 1;
	int cmmaxz = (t.y+1) * TILE_SIZE - 1;

	int nminx = cmminx / PATHNODE_SIZE;
	int nminz = cmminz / PATHNODE_SIZE;
	int nmaxx = cmmaxx / PATHNODE_SIZE;
	int nmaxz = cmmaxz / PATHNODE_SIZE;

	for(int x=nminx; x<=nmaxx; x++)
		for(int z=nminz; z<=nmaxz; z++)
		{
			ColliderTile* c = ColliderTileAt(x, z);
			//c->hasroad = true;
			c->flags ^= FLAG_HASROAD;
		}

#if 0
	int roadx, roadz;

	RoadXZ(this, roadx, roadz);
	Vec3f pos = RoadPosition(roadx, roadz);

	//BuildingT* bt = &g_bltype[type];
	float hwx = TILE_SIZE/2.0f;
	float hwz = TILE_SIZE/2.0f;

	float fstartx = pos.x-hwx;
	float fstartz = pos.z-hwz;
	float fendx = pos.x+hwx;
	float fendz = pos.z+hwz;

	//int extentx = ceil(r/cellwx);
	//int extentz = ceil(r/cellwz);
	const float cellwx = MIN_RADIUS*2.0f;
	const float cellwz = MIN_RADIUS*2.0f;

	int startx = fstartx/cellwx;
	int startz = fstartz/cellwz;
	int endx = ceil(fendx/cellwx);
	int endz = ceil(fendz/cellwz);
	//int endx = startx+extentx;
	//int endz = startz+extentz;

	for(int x=startx; x<endx; x++)
		for(int z=startz; z<endz; z++)
		{
			ColliderTile* cell = ColliderTileAt(x, z);
			//cell->colliders.push_back(Collider(COLLIDER_UNIT, uID));
			for(int i=0; i<cell->colliders.size(); i++)
			{
				Collider* c = &cell->colliders[i];
				if(c->type == COLLIDER_NOROAD)
				{
					cell->colliders.erase( cell->colliders.begin() + i );
					//break;
					i--;
				}
			}
		}
#endif
}

void CheckRoadAccess()
{
	std::list<int> sharednetw;

	for(int i=0; i<BUILDINGS; i++)
	{
		Building* b = &g_building[i];

		if(!b->on)
			continue;

		for(auto netwiter = b->roadnetw.begin(); netwiter != b->roadnetw.end(); netwiter++)
		{
			bool found = false;
			for(auto sharednetwiter = sharednetw.begin(); sharednetwiter != sharednetw.end(); sharednetwiter++)
			{
				if(*sharednetwiter == *netwiter)
				{
					found = true;
					break;
				}
			}

			if(!found)
				sharednetw.push_back(*netwiter);
		}
	}

	for(int i=0; i<BUILDINGS; i++)
	{
		Building* b = &g_building[i];

		if(!b->on)
			continue;

		auto sharednetwiter = sharednetw.begin();
		while( sharednetwiter != sharednetw.end() )
		{
			bool found = false;

			for(auto netwiter = b->roadnetw.begin(); netwiter != b->roadnetw.end(); netwiter++)
			{
				if(*sharednetwiter == *netwiter)
				{
					found = true;
					break;
				}
			}

			if(!found)
			{
				sharednetwiter = sharednetw.erase( sharednetwiter );
				continue;
			}

			sharednetwiter++;
		}

#if 0
		if(sharednetw.size() <= 0)
			OnRoadInacc();
#endif
	}

#if 0
	if(sharednetw.size() > 0)
		OnAllRoadAc();
#endif
}
