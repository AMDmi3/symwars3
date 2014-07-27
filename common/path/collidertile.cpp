#include "collidertile.h"
#include "../math/vec2i.h"
#include "../math/3dmath.h"
#include "../sim/unit.h"
#include "../sim/unittype.h"
#include "../sim/building.h"
#include "../sim/buildingtype.h"
#include "../render/heightmap.h"
#include "../math/hmapmath.h"
#include "../phys/collision.h"
#include "../sim/road.h"
#include "../render/water.h"
#include "../utils.h"
#include "../render/shader.h"
#include "../sim/selection.h"
#include "../sim/sim.h"
#include "../phys/trace.h"
#include "binheap.h"
#include "pathnode.h"
#include "../math/vec2i.h"
#include "pathdebug.h"
#include "pathjob.h"
#include "../debug.h"
#include "../sim/infrastructure.h"

ColliderTile *g_collidertile = NULL;

ColliderTile::ColliderTile()
{
#if 0
	bool hasroad;
	bool hasland;
	bool haswater;
	bool abrupt;	//abrupt incline?
	std::list<int> units;
	std::list<int> foliage;
	int building;
#endif

	//hasroad = false;
	//hasland = false;
	//haswater = false;
	//abrupt = false;
	flags = 0;
	building = -1;
	for(int i=0; i<MAX_COLLIDER_UNITS; i++)
		units[i] = -1;
}

inline Vec2i PathNodePos(int cmposx, int cmposz)
{
	return Vec2i(cmposx/PATHNODE_SIZE, cmposz/PATHNODE_SIZE);
}

void FreePathGrid()
{
	g_log<<"free path gr"<<std::endl;

	if(g_collidertile)
	{
		delete [] g_collidertile;
		g_collidertile = NULL;
	}

	g_pathdim = Vec2i(0,0);

	if(g_pathnode)
	{
		delete [] g_pathnode;
		g_pathnode = NULL;
	}

	g_openlist.freemem();
}

void AllocPathGrid(int cmwx, int cmwz)
{
	FreePathGrid();
	g_pathdim.x = cmwx / PATHNODE_SIZE;
	g_pathdim.y = cmwz / PATHNODE_SIZE;
	g_collidertile = new ColliderTile [ g_pathdim.x * g_pathdim.y ];

	g_log<<"path gr allc "<<g_pathdim.x<<","<<g_pathdim.y<<std::endl;

	int cwx = g_pathdim.x;
	int cwz = g_pathdim.y;

	g_pathnode = new PathNode [ cwx * cwz ];
	g_openlist.alloc( cwx * cwz );

	for(int x=0; x<cwx; x++)
		for(int z=0; z<cwz; z++)
		{
			PathNode* n = PathNodeAt(x, z);
			n->nx = x;
			n->nz = z;
			n->opened = false;
			n->closed = false;
		}

	//g_lastpath = g_simframe;
}

ColliderTile* ColliderTileAt(int nx, int nz)
{
	return &g_collidertile[ PathNodeIndex(nx, nz) ];
}

void FillColliderGrid()
{
	const int cwx = g_pathdim.x;
	const int cwz = g_pathdim.y;

	//g_log<<"path gr "<<cwx<<","<<cwz<<std::endl;

	for(int x=0; x<cwx; x++)
		for(int z=0; z<cwz; z++)
		{
			int cmx = x*PATHNODE_SIZE + PATHNODE_SIZE/2;
			int cmz = z*PATHNODE_SIZE + PATHNODE_SIZE/2;
			ColliderTile* cell = ColliderTileAt(x, z);

			//g_log<<"cell "<<x<<","<<z<<" cmpos="<<cmx<<","<<cmz<<" y="<<g_hmap.accheight(cmx, cmz)<<std::endl;

			if(AtLand(cmx, cmz))
			{
				//cell->hasland = true;
				cell->flags |= FLAG_HASLAND;
				//g_log<<"land "<<(cmx/TILE_SIZE)<<","<<(cmz/TILE_SIZE)<<" flag="<<(cell->flags & FLAG_HASLAND)<<"="<<(unsigned int)cell->flags<<std::endl;
				//g_log<<"land"<<std::endl;
			}
			else
			{
				//cell->hasland = false;
				cell->flags &= ~FLAG_HASLAND;
			}

#if 0
			if(AtWater(cmx, cmz))
				cell->haswater = true;
			else
				cell->haswater = false;
#endif

			if(TileUnclimable(cmx, cmz) && (cell->flags & FLAG_HASLAND))
			{
				//cell->abrupt = true;
				cell->flags |= FLAG_ABRUPT;
			}
			else
			{
				//cell->abrupt = false;
				cell->flags &= ~FLAG_ABRUPT;
			}

			int tx = cmx/TILE_SIZE;
			int tz = cmz/TILE_SIZE;

			ConduitTile* r = GetCo(CONDUIT_ROAD, tx, tz, false);

			//if(r->on /* && r->finished */ )
			if(r->on && r->finished)
			{
				//cell->hasroad = true;
				cell->flags |= FLAG_HASROAD;
			}
			else
			{
				//cell->hasroad = false;
				cell->flags &= ~FLAG_HASROAD;
			}
		}


	for(int x=0; x<LARGEST_UNIT_NODES; x++)
		for(int z=0; z<cwz; z++)
		{
			ColliderTile* cell = ColliderTileAt(x, z);
			cell->flags |= FLAG_ABRUPT;
		}

	for(int x=cwx-LARGEST_UNIT_NODES-1; x<cwx; x++)
		for(int z=0; z<cwz; z++)
		{
			ColliderTile* cell = ColliderTileAt(x, z);
			cell->flags |= FLAG_ABRUPT;
		}

	for(int x=0; x<cwx; x++)
		for(int z=0; z<LARGEST_UNIT_NODES; z++)
		{
			ColliderTile* cell = ColliderTileAt(x, z);
			cell->flags |= FLAG_ABRUPT;
		}

	for(int x=0; x<cwx; x++)
		for(int z=cwz-LARGEST_UNIT_NODES-1; z<cwz; z++)
		{
			ColliderTile* cell = ColliderTileAt(x, z);
			cell->flags |= FLAG_ABRUPT;
		}

	for(int i=0; i<UNITS; i++)
	{
		Unit* u = &g_unit[i];

		if(!u->on)
			continue;

		u->fillcollider();
	}

	for(int i=0; i<BUILDINGS; i++)
	{
		Building* b = &g_building[i];

		if(!b->on)
			continue;

		b->fillcollider();
	}

	ResetPathNodes();
}

void Unit::fillcollider()
{
	UnitT* t = &g_utype[type];
	int ui = this - g_unit;

	//cm = centimeter position
	int cmminx = cmpos.x - t->size.x/2;
	int cmminz = cmpos.y - t->size.z/2;
	int cmmaxx = cmminx + t->size.x - 1;
	int cmmaxz = cmminz + t->size.z - 1;

	//c = cell position
	int cminx = cmminx / PATHNODE_SIZE;
	int cminz = cmminz / PATHNODE_SIZE;
	int cmaxx = cmmaxx / PATHNODE_SIZE;
	int cmaxz = cmmaxz / PATHNODE_SIZE;

	for(int nz = cminz; nz <= cmaxz; nz++)
		for(int nx = cminx; nx <= cmaxx; nx++)
		{
			ColliderTile* c = ColliderTileAt(nx, nz);

			for(short uiter = 0; uiter < MAX_COLLIDER_UNITS; uiter++)
			{
				if(c->units[uiter] < 0)
				{
					c->units[uiter] = ui;
					break;
				}
			}
		}
}

void Building::fillcollider()
{
	BuildingT* t = &g_bltype[type];
	int bi = this - g_building;

	//t = tile position
	int tminx = tilepos.x - t->widthx/2;
	int tminz = tilepos.y - t->widthz/2;
	int tmaxx = tminx + t->widthx;
	int tmaxz = tminz + t->widthz;

	//cm = centimeter position
	int cmminx = tminx*TILE_SIZE;
	int cmminz = tminz*TILE_SIZE;
	int cmmaxx = tmaxx*TILE_SIZE - 1;
	int cmmaxz = tmaxz*TILE_SIZE - 1;

	//c = cell position
	int cminx = cmminx / PATHNODE_SIZE;
	int cminz = cmminz / PATHNODE_SIZE;
	int cmaxx = cmmaxx / PATHNODE_SIZE;
	int cmaxz = cmmaxz / PATHNODE_SIZE;

	for(int nz = cminz; nz <= cmaxz; nz++)
		for(int nx = cminx; nx <= cmaxx; nx++)
		{
			ColliderTile* c = ColliderTileAt(nx, nz);
			c->building = bi;
		}
}

void Unit::freecollider()
{
	UnitT* t = &g_utype[type];
	int ui = this - g_unit;

	//cm = centimeter position
	int cmminx = cmpos.x - t->size.x/2;
	int cmminz = cmpos.y - t->size.z/2;
	int cmmaxx = cmminx + t->size.x - 1;
	int cmmaxz = cmminz + t->size.z - 1;

	//c = cell position
	int cminx = cmminx / PATHNODE_SIZE;
	int cminz = cmminz / PATHNODE_SIZE;
	int cmaxx = cmmaxx / PATHNODE_SIZE;
	int cmaxz = cmmaxz / PATHNODE_SIZE;

	for(int nz = cminz; nz <= cmaxz; nz++)
		for(int nx = cminx; nx <= cmaxx; nx++)
		{
			ColliderTile* c = ColliderTileAt(nx, nz);

			for(short uiter = 0; uiter < MAX_COLLIDER_UNITS; uiter++)
			{
				if(c->units[uiter] == ui)
					c->units[uiter] = -1;
			}
		}
}

void Building::freecollider()
{
	BuildingT* t = &g_bltype[type];
	int bi = this - g_building;

	//t = tile position
	int tminx = tilepos.x - t->widthx/2;
	int tminz = tilepos.y - t->widthz/2;
	int tmaxx = tminx + t->widthx;
	int tmaxz = tminz + t->widthz;

	//cm = centimeter position
	int cmminx = tminx*TILE_SIZE;
	int cmminz = tminz*TILE_SIZE;
	int cmmaxx = tmaxx*TILE_SIZE - 1;
	int cmmaxz = tmaxz*TILE_SIZE - 1;

	//c = cell position
	int cminx = cmminx / PATHNODE_SIZE;
	int cminz = cmminz / PATHNODE_SIZE;
	int cmaxx = cmmaxx / PATHNODE_SIZE;
	int cmaxz = cmmaxz / PATHNODE_SIZE;

	for(int nz = cminz; nz <= cmaxz; nz++)
		for(int nx = cminx; nx <= cmaxx; nx++)
		{
			ColliderTile* c = ColliderTileAt(nx, nz);

			if(c->building == bi)
				c->building = -1;
		}
}

// Uses cm pos instead of pathnode pos
// Uses cm-accurate intersection checks
bool Walkable2(PathJob* pj, int cmposx, int cmposz)
{
	const int nx = cmposx / PATHNODE_SIZE;
	const int nz = cmposz / PATHNODE_SIZE;

#if 0
	if(nx < 0 || nz < 0 || nx >= g_pathdim.x || nz >= g_pathdim.y)
		return false;
#endif

	ColliderTile* cell = ColliderTileAt( nx, nz );

#if 1
	if(cell->flags & FLAG_ABRUPT)
	{
		//g_log<<"abrupt"<<std::endl;
		return false;
	}

	if(pj->roaded && !(cell->flags & FLAG_HASROAD))
	{
		//g_log<<"!road"<<std::endl;
		return false;
	}

	if(pj->landborne && !(cell->flags & FLAG_HASLAND))
	{
		//g_log<<"!land flag="<<(cell->flags & FLAG_HASLAND)<<"="<<(unsigned int)cell->flags<<std::endl;
		return false;
	}

	if(pj->seaborne && (cell->flags & FLAG_HASLAND))
	{
		//g_log<<"!sea"<<std::endl;
		return false;
	}
#endif

	UnitT* ut = &g_utype[pj->utype];

	int cmminx = cmposx - ut->size.x/2;
	int cmminz = cmposz - ut->size.z/2;
	int cmmaxx = cmminx + ut->size.x - 1;
	int cmmaxz = cmminz + ut->size.z - 1;

	int cminx = cmminx/PATHNODE_SIZE;
	int cminz = cmminz/PATHNODE_SIZE;
	int cmaxx = cmmaxx/PATHNODE_SIZE;
	int cmaxz = cmmaxz/PATHNODE_SIZE;

#if 0
	if(cminx < 0 || cminz < 0 || cmaxx >= g_pathdim.x || cmaxz >= g_pathdim.y)
	{
		return false;
	}
#endif

	for(int z=cminz; z<=cmaxz; z++)
		for(int x=cminx; x<=cmaxx; x++)
		{
			cell = ColliderTileAt(x, z);

			if(cell->building >= 0 && cell->building != pj->ignoreb)
			{
				Building* b = &g_building[cell->building];
				BuildingT* t2 = &g_bltype[b->type];

				int tminx = b->tilepos.x - t2->widthx/2;
				int tminz = b->tilepos.y - t2->widthz/2;
				int tmaxx = tminx + t2->widthx;
				int tmaxz = tminz + t2->widthz;

				int minx2 = tminx*TILE_SIZE;
				int minz2 = tminz*TILE_SIZE;
				int maxx2 = tmaxx*TILE_SIZE - 1;
				int maxz2 = tmaxz*TILE_SIZE - 1;

				if(cmminx <= maxx2 && cmminz <= maxz2 && cmmaxx >= minx2 && cmmaxz >= minz2)
				{
					//g_log<<"bld"<<std::endl;
					return false;
				}
			}

			for(short uiter = 0; uiter < MAX_COLLIDER_UNITS; uiter++)
			{
				short uindex = cell->units[uiter];

				if( uindex < 0 )
					continue;

				Unit* u = &g_unit[uindex];

				if(uindex != pj->thisu && uindex != pj->ignoreu && !u->hidden())
				{
#if 1
					UnitT* t = &g_utype[u->type];

					int cmminx2 = u->cmpos.x - t->size.x/2;
					int cmminz2 = u->cmpos.y - t->size.z/2;
					int cmmaxx2 = cmminx2 + t->size.x - 1;
					int cmmaxz2 = cmminz2 + t->size.z - 1;

					if(cmmaxx >= cmminx2 && cmmaxz >= cmminz2 && cmminx <= cmmaxx2 && cmminz <= cmmaxz2)
					{
						//g_log<<"u"<<std::endl;
						return false;
					}
#else
					return false;
#endif
				}
			}
		}

	return true;
}

bool Standable(const PathJob* pj, const int nx, const int nz)
{
#if 0
	if(nx < 0 || nz < 0 || nx >= g_pathdim.x || nz >= g_pathdim.y)
		return false;
#endif

	ColliderTile* cell = ColliderTileAt( nx, nz );

#if 1
	if(cell->flags & FLAG_ABRUPT)
		return false;

	if(pj->roaded && !(cell->flags & FLAG_HASROAD))
	{
		return false;
	}

	if(pj->landborne && !(cell->flags & FLAG_HASLAND))
	{
		return false;
	}

	if(pj->seaborne && (cell->flags & FLAG_HASLAND))
	{
		return false;
	}
#endif

	const UnitT* ut = &g_utype[pj->utype];

	const int cmposx = nx * PATHNODE_SIZE + PATHNODE_SIZE/2;
	const int cmposz = nz * PATHNODE_SIZE + PATHNODE_SIZE/2;

	const int cmminx = cmposx - ut->size.x/2;
	const int cmminz = cmposz - ut->size.z/2;
	const int cmmaxx = cmminx + ut->size.x - 1;
	const int cmmaxz = cmminz + ut->size.z - 1;

	const int cminx = cmminx/PATHNODE_SIZE;
	const int cminz = cmminz/PATHNODE_SIZE;
	const int cmaxx = cmmaxx/PATHNODE_SIZE;
	const int cmaxz = cmmaxz/PATHNODE_SIZE;

#if 0
	if(cminx < 0 || cminz < 0 || cmaxx >= g_pathdim.x || cmaxz >= g_pathdim.y)
	{
		return false;
	}
#endif

	for(int z=cminz; z<=cmaxz; z++)
		for(int x=cminx; x<=cmaxx; x++)
		{
			cell = ColliderTileAt(x, z);

			if(cell->building >= 0 && cell->building != pj->ignoreb)
			{
#if 0
				Building* b = &g_building[cell->building];
				BuildingT* t2 = &g_bltype[b->type];

				int tminx = b->tilepos.x - t2->widthx/2;
				int tminz = b->tilepos.y - t2->widthz/2;
				int tmaxx = tminx + t2->widthx;
				int tmaxz = tminz + t2->widthz;

				int minx2 = tminx*TILE_SIZE;
				int minz2 = tminz*TILE_SIZE;
				int maxx2 = tmaxx*TILE_SIZE - 1;
				int maxz2 = tmaxz*TILE_SIZE - 1;

				if(cmminx <= maxx2 && cmminz <= maxz2 && cmmaxx >= minx2 && cmmaxz >= minz2)
					return false;
#else
				return false;
#endif
			}

			for(short uiter = 0; uiter < MAX_COLLIDER_UNITS; uiter++)
			{
				short uindex = cell->units[uiter];

				if( uindex < 0 )
					continue;

				Unit* u = &g_unit[uindex];

				if(uindex != pj->thisu && uindex != pj->ignoreu && !u->hidden())
				{
#if 0
					UnitT* t = &g_utype[u->type];

					int cmminx2 = u->cmpos.x - t->size.x/2;
					int cmminz2 = u->cmpos.y - t->size.z/2;
					int cmmaxx2 = cmminx2 + t->size.x - 1;
					int cmmaxz2 = cmminz2 + t->size.z - 1;

					if(cmmaxx >= cmminx2 && cmmaxz >= cmminz2 && cmminx <= cmmaxx2 && cmminz <= cmmaxz2)
					{

						return false;
					}
#else
					return false;
#endif
				}
			}
		}

	return true;
}
