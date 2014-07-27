#include "collision.h"
#include "../sim/building.h"
#include "../sim/buildingtype.h"
#include "../sim/unit.h"
#include "../sim/unittype.h"
#include "../render/heightmap.h"
#include "../sim/crpipe.h"
#include "../sim/powl.h"

int g_lastcollider = -1;
int g_collidertype;
bool g_ignored;

bool BlAdj(int i, int j)
{
	Building* bi = &g_building[i];
	Building* bj = &g_building[j];

	BuildingT* ti = &g_bltype[bi->type];
	BuildingT* tj = &g_bltype[bj->type];

	Vec2i tpi = bi->tilepos;
	Vec2i tpj = bj->tilepos;

	Vec2i mini;
	Vec2i minj;
	Vec2i maxi;
	Vec2i maxj;

	mini.x = tpi.x - ti->widthx/2;
	mini.y = tpi.y - ti->widthz/2;
	minj.x = tpj.x - tj->widthx/2;
	minj.y = tpj.y - tj->widthz/2;
	maxi.x = mini.x + ti->widthx;
	maxi.y = mini.y + ti->widthz;
	maxj.x = minj.x + tj->widthx;
	maxj.y = minj.y + tj->widthz;

	if(maxi.x >= minj.x && maxi.y >= minj.y && mini.x <= maxj.x && mini.y <= maxj.y)
		return true;

	return false;
}

// Is conduit x,z adjacent to building i?
bool CoAdj(char ctype, int i, int x, int z)
{
	Building* b = &g_building[i];
	BuildingT* t = &g_bltype[b->type];

	Vec2i tp = b->tilepos;

	Vec2i tmin;
	Vec2i tmax;

	tmin.x = tp.x - t->widthx/2;
	tmin.y = tp.y - t->widthz/2;
	tmax.x = tmin.x + t->widthx;
	tmax.y = tmin.y + t->widthz;

	Vec2i cmmin = Vec2i(tmin.x*TILE_SIZE, tmin.y*TILE_SIZE);
	Vec2i cmmax = Vec2i(tmax.x*TILE_SIZE, tmax.y*TILE_SIZE);

	ConduitType* ct = &g_cotype[ctype];
	Vec2i ccmp2 = ct->physoff + Vec2i(x, z)*TILE_SIZE;

	Vec2i cmmin2 = Vec2i(ccmp2.x-TILE_SIZE/2, ccmp2.y-TILE_SIZE/2);
	Vec2i cmmax2 = Vec2i(ccmp2.x+TILE_SIZE/2, ccmp2.y+TILE_SIZE/2);

	if(cmmax.x >= cmmin2.x && cmmax.y >= cmmin2.y && cmmin.x <= cmmax2.x && cmmin.y <= cmmax2.y)
		return true;

	return false;
}

bool CollidesWithBuildings(int minx, int minz, int maxx, int maxz, int ignore)
{
	g_ignored = false;

	for(int i=0; i<BUILDINGS; i++)
	{
		Building* b = &g_building[i];

		if(!b->on)
			continue;

		BuildingT* t = &g_bltype[ b->type ];

		int minx2 = (b->tilepos.x - t->widthx/2)*TILE_SIZE;
		int minz2 = (b->tilepos.y - t->widthz/2)*TILE_SIZE;
		int maxx2 = minx2 + t->widthx*TILE_SIZE - 1;
		int maxz2 = minz2 + t->widthz*TILE_SIZE - 1;

		if(maxx >= minx2 && maxz >= minz2 && minx <= maxx2 && minz <= maxz2)
		{
			if(i == ignore)
			{
				g_ignored = true;
				continue;
			}
			/*
			 if(g_debug1)
			 {
			 g_log<<"fabs((p.x)"<<p.x<<"-(x)"<<x<<") < (hwx)"<<hwx<<"+(hwx2)"<<hwx2<<" && fabs((p.z)"<<p.z<<"- (z)"<<z<<") < (hwz)"<<hwz<<"+(hwz2)"<<hwz2<<std::endl;
			 g_log<<fabs(p.x-x)<<" < "<<(hwx+hwx2)<<" && "<<fabs(p.z-z)<<" < "<<(hwz+hwz2)<<std::endl;
			 }*/

			g_lastcollider = i;
			g_collidertype = COLLIDER_BUILDING;
			return true;
		}
	}

	return false;
}

bool CollidesWithUnits(int minx, int minz, int maxx, int maxz, bool isunit, Unit* thisu, Unit* ignore)
{
	for(int i=0; i<UNITS; i++)
	{
		Unit* u = &g_unit[i];

		if(!u->on)
			continue;

		if(u == ignore)
			continue;

		if(u == thisu)
			continue;

		if(u->hidden())
			continue;

		UnitT* t = &g_utype[u->type];

		int minx2 = u->cmpos.x - t->size.x/2;
		int minz2 = u->cmpos.y - t->size.z/2;
		int maxx2 = minx2 + t->size.x - 1;
		int maxz2 = minz2 + t->size.z - 1;

		if(maxx >= minx2 && maxz >= minz2 && minx <= maxx2 && minz <= maxz2)
		{
			g_lastcollider = i;
			g_collidertype = COLLIDER_UNIT;
			return true;
		}
	}

	return false;
}

bool OffMap(int minx, int minz, int maxx, int maxz)
{
	if(minx < 0)
		return true;

	if(maxx >= (g_hmap.m_widthx-1)*TILE_SIZE)
		return true;

	if(minz < 0)
		return true;

	if(maxz >= (g_hmap.m_widthz-1)*TILE_SIZE)
		return true;

	return false;
}

#if 0

// TO DO: REWRITE
bool BuildingCollides(int type, Vec3f pos)
{
	BuildingT* t = &g_bltype[type];

	if(Offmap(pos.x, pos.z, t->widthX*TILE_SIZE/2, t->widthZ*TILE_SIZE/2))
		return true;

	if(CollidesWithBuildings(pos.x, pos.z, t->widthX*TILE_SIZE/2, t->widthZ*TILE_SIZE/2))
		return true;

	if(CollidesWithUnits(pos.x, pos.z, t->widthX*TILE_SIZE/2, t->widthZ*TILE_SIZE/2))
		return true;

	int startx = pos.x/TILE_SIZE-t->widthX/2;
	int endx = startx + t->widthX - 1;
	int startz = pos.z/TILE_SIZE-t->widthZ/2;
	int endz = startz + t->widthZ - 1;

	for(int x=startx; x<=endx; x++)
		for(int z=startz; z<=endz; z++)
			if(RoadAt(x, z)->on)
				return true;

	return false;
}

// used to check for collisions with passable units
// edit: used for initial spawn of unit
bool Unit::Collides2(bool checkroad)
{
	UnitT* t = &g_unitType[type];
	float r = t->radius;
	Vec3f p = camera.Position();

	if(checkroad)
	{
		bool roadVeh = t->roaded;

		int tx = p.x / TILE_SIZE;
		int tz = p.z / TILE_SIZE;

		CRoad* road = RoadAt(tx, tz);

		if(roadVeh && !road->on)
			return true;
		else if(!roadVeh && road->on)
			return true;
	}

	if(AtWater(p.x, p.z))
		return true;

	if(TileUnclimable(p.x, p.z))
		return true;

	if(CollidesWithBuildings(p.x, p.z, r, r))
		return true;

	if(CollidesWithUnits(p.x, p.z, r, r, true, this, NULL, false))
		return true;

	if(Offmap(p.x, p.z, r, r))
		return true;

	return false;
}

bool Unit::confirmcollision(int ctype, int ID, float eps)
{
	UnitT* ut = &g_unitType[type];
	//Vec3f p = camera.Position();
	float r = ut->radius;
	const Vec3f p = camera.Position();

	if(ctype == COLLIDER_BUILDING)
	{
		Building* b = &g_building[ID];
		BuildingT* bt = &g_bltype[b->type];
		Vec3f p2 = b->pos;
		float hwx = bt->widthX*TILE_SIZE/2.0;
		float hwz = bt->widthZ*TILE_SIZE/2.0;

		if(fabs(p2.x-p.x) < r+hwx-eps && fabs(p2.z-p.z) < r+hwz-eps)
			return true;
	}
	else if(ctype == COLLIDER_UNIT)
	{
		Unit* u2 = &g_unit[ID];
		ut = &g_unitType[u2->type];
		float r2 = ut->radius;
		const Vec3f p2 = u2->camera.Position();

		if(fabs(p2.x-p.x) < r+r2-eps && fabs(p2.z-p.z) < r+r2-eps)
			return true;
	}
	else if(ctype == COLLIDER_TERRAIN)
	{
		if(AtWater(p.x, p.z))
			return true;

		if(TileUnclimable(p.x, p.z))
			return true;
	}
	else if(ctype == COLLIDER_NOROAD)
	{
		CRoad* road = RoadAt(p.x/TILE_SIZE, p.z/TILE_SIZE);

		if(!road->on || !road->finished)
			return true;
	}

	return false;
}

bool Unit::collidesfast(Unit* ignoreUnit, int ignoreBuilding)
{
	UnitT* ut = &g_unitType[type];
	//Vec3f p = camera.Position();
	float r = ut->radius;
	const Vec3f p = camera.Position();

	const bool roadVeh = ut->roaded;

	float fstartx = p.x-r;
	float fstartz = p.z-r;
	float fendx = p.x+r;
	float fendz = p.z+r;

	//int extentx = ceil(r/cellwx);
	//int extentz = ceil(r/cellwz);
	const float cellwx = MIN_RADIUS*2.0f;
	const float cellwz = MIN_RADIUS*2.0f;

	//int startx = std::max(0, fstartx/cellwx);
	//int startz = std::max(0, fstartz/cellwz);
	//int endx = std::min(ceil(fendx/cellwx), g_hmap.m_widthX*TILE_SIZE/cellwx-1);
	//int endz = std::min(ceil(fendz/cellwz), g_hmap.m_widthZ*TILE_SIZE/cellwz-1);
	int startx = fstartx/cellwx;
	int startz = fstartz/cellwz;
	int endx = ceil(fendx/cellwx);
	int endz = ceil(fendz/cellwz);
	//int endx = startx+extentx;
	//int endz = startz+extentz;

	if(startx < 0 || startz < 0 || endx >= g_hmap.m_widthX*TILE_SIZE/cellwx || endz >= g_hmap.m_widthZ*TILE_SIZE/cellwz)
	{
		g_lastcollider = -1;
		g_collidertype = COLLIDER_TERRAIN;
		return true;
	}

	g_ignored = false;

	if(roadVeh)
	{
		for(int x=startx; x<endx; x++)
			for(int z=startz; z<endz; z++)
			{
				ColliderTile* cell = ColliderTileAt(x, z);
				//cell->colliders.push_back(Collider(COLLIDER_UNIT, uID));
				for(int i=0; i<cell->colliders.size(); i++)
				{
					Collider* c = &cell->colliders[i];
					if(c->type == COLLIDER_UNIT && &g_unit[c->ID] == this)
						continue;
					if(c->type == COLLIDER_BUILDING && c->ID == ignoreBuilding)
					{
						if(confirmcollision(c->type, c->ID))
							g_ignored = true;
						continue;
					}
					if(c->type == COLLIDER_UNIT)
					{
						if(&g_unit[c->ID] == ignoreUnit)
						{
							if(confirmcollision(c->type, c->ID))
								g_ignored = true;
							continue;
						}
						if(g_unit[c->ID].hidden())
							continue;
					}

					if(confirmcollision(c->type, c->ID))
					{
						g_collidertype = c->type;
						g_lastcollider = c->ID;

						return true;
					}
				}
			}
	}
	else
	{
		for(int x=startx; x<endx; x++)
			for(int z=startz; z<endz; z++)
			{
				ColliderTile* cell = ColliderTileAt(x, z);
				//cell->colliders.push_back(Collider(COLLIDER_UNIT, uID));
				for(int i=0; i<cell->colliders.size(); i++)
				{
					Collider* c = &cell->colliders[i];
					if(c->type == COLLIDER_UNIT && &g_unit[c->ID] == this)
						continue;
					if(c->type == COLLIDER_NOROAD)
						continue;
					if(c->type == COLLIDER_BUILDING && c->ID == ignoreBuilding)
					{
						if(confirmcollision(c->type, c->ID))
							g_ignored = true;

						continue;
					}
					if(c->type == COLLIDER_UNIT)
					{
						if(&g_unit[c->ID] == ignoreUnit)
						{
							if(confirmcollision(c->type, c->ID))
								g_ignored = true;
							continue;
						}
						if(g_unit[c->ID].hidden())
							continue;
					}

					if(confirmcollision(c->type, c->ID))
					{
						g_collidertype = c->type;
						g_lastcollider = c->ID;

						return true;
					}
				}
			}
	}

	return false;
}

bool Unit::Collides(Unit* ignoreUnit, int ignoreBuilding)
{
	UnitT* t = &g_unitType[type];
	float r = t->radius;
	Vec3f p = camera.Position();
	/*
	 g_debug1 = false;
	 if(UnitID(this) == 0)
	 {
	 g_debug1=  true;
	 }*/

	if(AtWater(p.x, p.z))
		return true;

	if(TileUnclimable(p.x, p.z))
		return true;

	if(CollidesWithBuildings(p.x, p.z, r, r, ignoreBuilding, MIN_RADIUS/2.0f))
	{
		//g_debug1 = false;
		return true;
	}

	//g_debug1 = false;

	if(!t->passable && CollidesWithUnits(p.x, p.z, r, r, true, this, ignoreUnit, MIN_RADIUS/2.0f))
	{
		//if(type == TRUCK)
		//	Chat("truck col");

		return true;
	}

	if(Offmap(p.x, p.z, r, r))
		return true;

	return false;
}

#endif
