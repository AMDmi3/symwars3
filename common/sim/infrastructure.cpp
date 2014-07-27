#include "infrastructure.h"
#include "../render/heightmap.h"
#include "building.h"
#include "../phys/collision.h"
#include "../render/water.h"
#include "player.h"
#include "../sim/selection.h"
#include "../gui/gui.h"
#include "../gui/widgets/spez/constructionview.h"
#include "../../game/gui/gviewport.h"
#include "../../game/gmain.h"
#include "../../game/gui/ggui.h"
#include "../math/hmapmath.h"
#include "../render/foliage.h"

ConduitType g_cotype[CONDUIT_TYPES];

void ClearCoPlans(char ctype)
{
	//if(!get)
	//	return;

	for(int x=0; x<g_hmap.m_widthx; x++)
		for(int z=0; z<g_hmap.m_widthz; z++)
			GetCo(ctype, x, z, true)->on = false;
}

void ResetNetw(char ctype)
{
	ConduitType* ct = &g_cotype[ctype];

	for(int i=0; i<BUILDINGS; i++)
	{
		Building* b = &g_building[i];

		if(ct->blconduct)
		{
			short& netw = *(short*)(((char*)b)+ct->netwoff);
			netw = -1;
		}
		else
		{
			std::list<short>& netw = *(std::list<short>*)(((char*)b)+ct->netwoff);
			netw.clear();
		}
	}

	int lastnetw = 0;

	for(int x=0; x<g_hmap.m_widthx; x++)
		for(int z=0; z<g_hmap.m_widthz; z++)
		{
			ConduitTile* ctile = GetCo(ctype, x, z, false);

			if(!ctile->on)
				continue;

			if(!ctile->finished)
				continue;

			ctile->netw = lastnetw++;
		}
}

// If buildings conduct the resource,
// merge the networks of touching buildings.
// Return true if there was a change.
bool ReNetwB(char ctype)
{
	ConduitType* ct = &g_cotype[ctype];

	if(!ct->blconduct)
		return false;

	bool change = false;
	
	Building* b;
	Building* b2;

	for(int i=0; i<BUILDINGS; i++)
	{
		b = &g_building[i];
		short& netw = *(short*)(((char*)b)+ct->netwoff);

		if(!b->on)
			continue;

		for(int j=i+1; j<BUILDINGS; j++)
		{
			b2 = &g_building[j];

			if(!b2->on)
				continue;

			if(!BlAdj(i, j))
				continue;
			
			short& netw2 = *(short*)(((char*)b2)+ct->netwoff);

			if(netw < 0 && netw2 >= 0)
			{
				netw = netw2;
				change = true;
			}
			else if(netw2 < 0 && netw >= 0)
			{
				netw2 = netw;
				change = true;
			}
			else if(netw >= 0 && netw2 >= 0 && netw != netw2)
			{
				MergeNetw(ctype, netw, netw2);
				change = true;
			}
		}
	}

	return change;
}

// Merge two networks that have been found to be touching.
void MergeNetw(char ctype, int A, int B)
{
	int mini = imin(A, B);
	int maxi = imax(A, B);

	ConduitType* ct = &g_cotype[ctype];

	if(ct->blconduct)
		for(int i=0; i<BUILDINGS; i++)
		{
			Building* b = &g_building[i];

			if(!b->on)
				continue;
		
			short& netw = *(short*)(((char*)b)+ct->netwoff);

			if(netw == maxi)
				netw = mini;
		}
	else
		for(int i=0; i<BUILDINGS; i++)
		{
			Building* b = &g_building[i];

			bool found = false;
			std::list<short>& bnetw = *(std::list<short>*)(((char*)b)+ct->netwoff);
			auto netwiter = bnetw.begin();

			while(netwiter != bnetw.end())
			{
				if(*netwiter == maxi)
				{
					if(!found)
					{
						*netwiter = mini;
						found = true;
					}
					else
					{
						netwiter = bnetw.erase( netwiter );
						continue;
					}
				}

				netwiter++;
			}
		}

	for(int x=0; x<g_hmap.m_widthx; x++)
		for(int z=0; z<g_hmap.m_widthz; z++)
		{
			ConduitTile* ctile = GetCo(ctype, x, z, false);

			if(!ctile->on)
				continue;

			if(!ctile->finished)
				continue;

			if(ctile->netw == maxi)
				ctile->netw = mini;
		}
}

bool CompareCo(char ctype, ConduitTile* ctile, int tx, int tz)
{
	ConduitTile* ctile2 = GetCo(ctype, tx, tz, false);

	if(!ctile2->on)
		return false;

	if(!ctile2->finished)
		return false;

	if(ctile2->netw < 0 && ctile->netw >= 0)
	{
		ctile2->netw = ctile->netw;
		return true;
	}
	else if(ctile->netw < 0 && ctile2->netw >= 0)
	{
		ctile->netw = ctile2->netw;
		return true;
	}
	else if(ctile->netw >= 0 && ctile2->netw >= 0 && ctile->netw != ctile2->netw)
	{
		MergeNetw(ctype, ctile->netw, ctile2->netw);
		return true;
	}

	return false;
}

// Building adjacent?
bool BAdj(char ctype, int i, int tx, int tz)
{
	Building* b = &g_building[i];
	BuildingT* bt = &g_bltype[b->type];

	Vec2i btp = b->tilepos;

	//Building min/max positions
	int bcmminx = (btp.x - bt->widthx/2)*TILE_SIZE;
	int bcmminz = (btp.y - bt->widthz/2)*TILE_SIZE;
	int bcmmaxx = bcmminx + bt->widthx*TILE_SIZE;
	int bcmmaxz = bcmminz + bt->widthz*TILE_SIZE;

	ConduitType* ct = &g_cotype[ctype];
	//Vec3i p2 = RoadPhysPos(x, z);
	Vec2i ccmp = Vec2i(tx, tz)*TILE_SIZE + ct->physoff;
	
	//Conduit min/max positions
	const int hwx2 = TILE_SIZE/2;
	const int hwz2 = TILE_SIZE/2;
	int ccmminx = ccmp.x - hwx2;
	int ccmminz = ccmp.y - hwz2;
	int ccmmaxx = ccmminx + TILE_SIZE;
	int ccmmaxz = ccmminz + TILE_SIZE;

	if(bcmmaxx >= ccmminx && bcmmaxz >= ccmminz && bcmminx <= ccmmaxx && bcmminz <= ccmmaxz)
		return true;

	return false;
}

bool CompareB(char ctype, Building* b, ConduitTile* ctile)
{
	ConduitType* ct = &g_cotype[ctype];

	if(!ct->blconduct)
	{
		std::list<short>& bnetw = *(std::list<short>*)(((char*)b)+ct->netwoff);

		if(bnetw.size() <= 0 && ctile->netw >= 0)
		{
			bnetw.push_back(ctile->netw);
			return true;
		}/*
		 else if(r->netw < 0 && b->roadnetw >= 0)
		 {
		 pow->netw = b->pownetw;
		 return true;
		 }
		 else if(pow->netw >= 0 && b->pownetw >= 0 && pow->netw != b->pownetw)
		 {
		 MergePow(pow->netw, b->pownetw);
		 return true;
		 }*/
		else if(bnetw.size() > 0 && ctile->netw >= 0)
		{
			bool found = false;
			for(auto netwiter = bnetw.begin(); netwiter != bnetw.end(); netwiter++)
			{
				if(*netwiter == ctile->netw)
				{
					found = true;
					break;
				}
			}
			if(!found)
				bnetw.push_back(ctile->netw);
		}

		return false;
	}
	else
	{
		short& bnetw = *(short*)(((char*)b)+ct->netwoff);

		if(bnetw < 0 && ctile->netw >= 0)
		{
			bnetw = ctile->netw;
			return true;
		}
		else if(ctile->netw < 0 && bnetw >= 0)
		{
			ctile->netw = bnetw;
			return true;
		}
		else if(ctile->netw >= 0 && bnetw >= 0 && ctile->netw != bnetw)
		{
			MergeNetw(ctype, ctile->netw, bnetw);
			return true;
		}

		return false;
	}

	return false;
}

// Called by conduit network update function.
// Returns true if there was a change.
bool ReNetwTiles(char ctype)
{
	bool change = false;

	for(int x=0; x<g_hmap.m_widthx; x++)
		for(int z=0; z<g_hmap.m_widthz; z++)
		{
			ConduitTile* ctile = GetCo(ctype, x, z, false);

			if(!ctile->on)
				continue;

			if(!ctile->finished)
				continue;

			if(x > 0 && CompareCo(ctype, ctile, x-1, z))
				change = true;
			if(x < g_hmap.m_widthx-1 && CompareCo(ctype, ctile, x+1, z))
				change = true;
			if(z > 0 && CompareCo(ctype, ctile, x, z-1))
				change = true;
			if(z < g_hmap.m_widthz-1 && CompareCo(ctype, ctile, x, z+1))
				change = true;

			for(int i=0; i<BUILDINGS; i++)
			{
				Building* b = &g_building[i];

				if(!b->on)
					continue;

				if(!BAdj(ctype, i, x, z))
					continue;

				if(CompareB(ctype, b, ctile))
					change = true;
			}
		}

	return change;
}

void ReNetw(char ctype)
{
	ResetNetw(ctype);

	bool change;

	do
	{
		change = false;

		if(ReNetwB(ctype))
			change = true;

		if(ReNetwTiles(ctype))
			change = true;
	}
	while(change);

#if 0
	CheckRoadAccess();
#endif
}

// Is the tile level for a conduit? Take into account the direction which the conduit is leading and coming from 
// (e.g., forward incline may be greater than sideways incline).
bool CoLevel(char ctype, float iterx, float iterz, float testx, float testz, float dx, float dz, int i, float d, bool plantoo)
{
	//return true;

	bool n = false, e = false, s = false, w = false;
	int ix, iz;

	// Check which neighbours should have conduits too based on
	// the dragged starting and ending position (actually the loop
	// variables for the drag line).

	if(i > 0)
	{
		float x = iterx - dx;
		float z = iterz - dz;

		ix = x;
		iz = z;

		if(ix == testx)
		{
			if(iz == testz+1)	n = true;
			else if(iz == testz-1)	s = true;
		}
		else if(iz == testz)
		{
			if(ix == testx+1)	w = true;
			else if(ix == testx-1)	e = true;
		}

		float prevx = x - dx;
		float prevz = z - dz;

		if((int)x != prevx && (int)z != prevz)
		{
			ix = prevx;

			if(ix == testx)
			{
				if(iz == testz+1)	n = true;
				else if(iz == testz-1)	s = true;
			}
			else if(iz == testz)
			{
				if(ix == testx+1)	w = true;
				else if(ix == testx-1)	e = true;
			}
		}
	}

	if(i < d)
	{
		float x = iterx + dx;
		float z = iterz + dz;

		ix = x;
		iz = z;

		if(ix == testx)
		{
			if(iz == testz+1)	n = true;
			else if(iz == testz-1)	s = true;
		}
		else if(iz == testz)
		{
			if(ix == testx+1)	w = true;
			else if(ix == testx-1)	e = true;
		}

		if(i > 0)
		{
			float prevx = x - dx;
			float prevz = z - dz;

			if((int)x != prevx && (int)z != prevz)
			{
				ix = prevx;

				if(ix == testx)
				{
					if(iz == testz+1)	n = true;
					else if(iz == testz-1)	s = true;
				}
				else if(iz == testz)
				{
					if(ix == testx+1)	w = true;
					else if(ix == testx-1)	e = true;
				}
			}
		}
	}

	ConduitType* ct = &g_cotype[ctype];

	// Check for water

	if(!ct->cornerpl)
	{
		ix = testx;
		iz = testz;

		if(g_hmap.getheight(ix, iz) <= WATER_LEVEL)		return false;
		if(g_hmap.getheight(ix+1, iz) <= WATER_LEVEL)	return false;
		if(g_hmap.getheight(ix, iz+1) <= WATER_LEVEL)	return false;
		if(g_hmap.getheight(ix+1, iz+1) <= WATER_LEVEL)	return false;
	}
	else
	{
		//ix = (testx * TILE_SIZE + physoff.x)/TILE_SIZE;
		//iz = (testz * TILE_SIZE + physoff.y)/TILE_SIZE;
		ix = testx;
		iz = testz;

		if(g_hmap.getheight(ix, iz) <= WATER_LEVEL)		return false;
	}

	// Check which neighbours have conduits too

	if(ix > 0)
	{
		if(GetCo(ctype, ix-1, iz, false)->on)	w = true;
		//if(RoadPlanAt(ix-1, iz)->on)	w = true;
		if(plantoo)
			if(GetCo(ctype, ix-1, iz, true)->on) w = true;
	}

	if(ix < g_hmap.m_widthx-1)
	{
		if(GetCo(ctype, ix+1, iz, false)->on)	e = true;
		//if(RoadPlanAt(ix+1, iz)->on)	e = true;
		if(plantoo)
			if(GetCo(ctype, ix+1, iz, true)->on) e = true;
	}

	if(iz > 0)
	{
		if(GetCo(ctype, ix, iz-1, false)->on)	s = true;
		//if(RoadPlanAt(ix, iz-1)->on)	s = true;
		if(plantoo)
			if(GetCo(ctype, ix, iz-1, true)->on) s= true;
	}

	if(iz < g_hmap.m_widthz-1)
	{
		if(GetCo(ctype, ix, iz+1, false)->on)	n = true;
		//if(RoadPlanAt(ix, iz+1)->on)	n = true;
		if(plantoo)
			if(GetCo(ctype, ix, iz+1, true)->on) n = true;
	}
#if 0
	g_log<<"level? ix"<<ix<<","<<iz<<std::endl;
	g_log.flush();
#endif

	// Check forward and sideways incline depending on the connection type

	// 4- or 3-way connections
	if((n && e && s && w) || (n && e && s && !w) || (n && e && !s && w) || (n && e && !s && !w) || (n && !e && s && w)
			|| (n && !e && !s && w) || (!n && e && s && !w) || (!n && !e && s && w) || (!n && !e && !s && !w) || (!n && e && s && w))
	{
		float compare = g_hmap.getheight(ix, iz);
		if(fabs(g_hmap.getheight(ix+1, iz) - compare) > ct->maxsideincl)	return false;
		if(fabs(g_hmap.getheight(ix, iz+1) - compare) > ct->maxsideincl)	return false;
		if(fabs(g_hmap.getheight(ix+1, iz+1) - compare) > ct->maxsideincl)	return false;
	}

	// straight-through 2-way connection or 1-way connection
	else if((n && !e && s && !w) || (n && !e && !s && !w) || (!n && !e && s && !w))
	{
		if(fabs(g_hmap.getheight(ix, iz) - g_hmap.getheight(ix+1, iz)) > ct->maxsideincl)	return false;
		if(fabs(g_hmap.getheight(ix, iz+1) - g_hmap.getheight(ix+1, iz+1)) > ct->maxsideincl)	return false;
		if(fabs(g_hmap.getheight(ix, iz) - g_hmap.getheight(ix, iz+1)) > ct->maxforwincl)	return false;
		if(fabs(g_hmap.getheight(ix+1, iz) - g_hmap.getheight(ix+1, iz+1)) > ct->maxforwincl)	return false;
	}

	// straight-through 2-way connection or 1-way connection
	else if((!n && e && !s && w) || (!n && e && !s && !w) || (!n && !e && !s && w))
	{
		if(fabs(g_hmap.getheight(ix, iz) - g_hmap.getheight(ix+1, iz)) > ct->maxforwincl)	return false;
		if(fabs(g_hmap.getheight(ix, iz+1) - g_hmap.getheight(ix+1, iz+1)) > ct->maxforwincl)	return false;
		if(fabs(g_hmap.getheight(ix, iz) - g_hmap.getheight(ix, iz+1)) > ct->maxsideincl)	return false;
		if(fabs(g_hmap.getheight(ix+1, iz) - g_hmap.getheight(ix+1, iz+1)) > ct->maxsideincl)	return false;
	}

#if 0
	g_log<<"level yes! ix"<<ix<<","<<iz<<std::endl;
	g_log.flush();
#endif

	return true;
}

void RemeshCo(char ctype, int tx, int tz, bool plan)
{
	ConduitTile* ctile = GetCo(ctype, tx, tz, plan);

	if(!ctile->on)
		return;
#if 0
	g_log<<"mesh on "<<x<<","<<z<<std::endl;
	g_log.flush();
#endif

	ConduitType* ct = &g_cotype[ctype];
	int mi = ct->model[ctile->conntype][plan];
	Model* m = &g_model[mi];

	VertexArray* cva = &ctile->drawva;
	VertexArray* mva = &m->m_va[0];

	cva->free();

	//g_log<<"meshroad allocating "<<mva->numverts<<"...";
	//g_log.flush();

	cva->alloc( mva->numverts );

	if(!cva->vertices)
		OutOfMem(__FILE__, __LINE__);

	if(!cva->texcoords)
		OutOfMem(__FILE__, __LINE__);

	if(!cva->normals)
		OutOfMem(__FILE__, __LINE__);

	float realx, realz;
	ctile->drawpos = Vec3f(tx*TILE_SIZE, 0, tz*TILE_SIZE) + ct->drawoff;

	for(int i=0; i<cva->numverts; i++)
	{
		cva->vertices[i] = mva->vertices[i];
		cva->texcoords[i] = mva->texcoords[i];
		cva->normals[i] = mva->normals[i];

		realx = ctile->drawpos.x + cva->vertices[i].x;
		realz = ctile->drawpos.z + cva->vertices[i].z;

#if 0
		cva->vertices[i].y += Bilerp(&g_hmap, realx, realz);
#else
		cva->vertices[i].y += g_hmap.accheight2(realx, realz);
#endif
	}

	for(int i=0; i<cva->numverts; i+=3)
	{
		cva->normals[i+0] = Normal(&cva->vertices[i]);
		cva->normals[i+1] = Normal(&cva->vertices[i]);
		cva->normals[i+2] = Normal(&cva->vertices[i]);
	}

	//g_log<<"done meshroad"<<std::endl;
	//g_log.flush();

	cva->genvbo();
}

void UpdCoPlans(char ctype, char owner, Vec3f start, Vec3f end)
{
	ClearCoPlans(ctype);

	int x1 = Clipi(start.x/TILE_SIZE, 0, g_hmap.m_widthx-1);
	int z1 = Clipi(start.z/TILE_SIZE, 0, g_hmap.m_widthz-1);

	int x2 = Clipi(end.x/TILE_SIZE, 0, g_hmap.m_widthx-1);
	int z2 = Clipi(end.z/TILE_SIZE, 0, g_hmap.m_widthz-1);

#if 0
	g_log<<"road plan "<<x1<<","<<z1<<"->"<<x2<<","<<z2<<std::endl;
	g_log.flush();
#endif
	
	//PlacePowl(x1, z1, stateowner, true);
	//PlacePowl(x2, z2, stateowner, true);

	float dx = x2-x1;
	float dz = z2-z1;

	float d = sqrtf(dx*dx + dz*dz);

	dx /= d;
	dz /= d;

	int prevx = x1;
	int prevz = z1;

	int i = 0;

	//PlaceRoad(x1, z1, g_localPlayer, true);
	//if(RoadLevel(x2, z2, dx, dz, i, d))
	//	PlaceRoad(x2, z2, g_localPlayer, true);

	for(float x=x1, z=z1; i<=d; x+=dx, z+=dz, i++)
	{
		if(CoLevel(ctype, x, z, x, z, dx, dz, i, d, true))
		{
#if 0
			g_log<<"place road urp "<<x<<","<<z<<std::endl;
			g_log.flush();
#endif
			PlaceCo(ctype, x, z, owner, true);
		}

		if((int)x != prevx && (int)z != prevz)
		{
			if(CoLevel(ctype, x, z, prevx, z, dx, dz, i, d, true))
				PlaceCo(ctype, prevx, z, owner, true);
		}

		prevx = x;
		prevz = z;

#if 0
		g_log<<"place road "<<x<<","<<z<<std::endl;
		g_log.flush();
#endif
	}

	if((int)x2 != prevx && (int)z2 != prevz)
	{
		if(CoLevel(ctype, x2, z1, prevx, z2, dx, dz, i, d, true))
			PlaceCo(ctype, prevx, z2, owner, true);
	}

	for(int x=0; x<g_hmap.m_widthx; x++)
		for(int z=0; z<g_hmap.m_widthz; z++)
			RemeshCo(ctype, x, z, true);
}

void Repossess(char ctype, int tx, int tz, int owner)
{
	ConduitTile* ctile = GetCo(ctype, tx, tz, false);
	ctile->owner = owner;
	ctile->allocate();
}

bool CoPlaceable(int ctype, int x, int z)
{
	ConduitType* ct = &g_cotype[ctype];
	Vec2i cmpos = Vec2i(x, z) * TILE_SIZE + ct->physoff;
	
	if(TileUnclimable(cmpos.x, cmpos.y))
		return false;

	// Make sure construction resources can be transported
	// to this conduit tile/corner. E.g., powerlines and 
	// above-ground pipelines need to be road-accessible.

#if 1	//Doesn't work yet?
	for(int ri=0; ri<RESOURCES; ri++)
	{
		if(ct->conmat[ri] <= 0)
			continue;

		Resource* r = &g_resource[ri];

		if(r->conduit == CONDUIT_NONE)
			continue;

		if(r->conduit == ctype)
			continue;

		char reqctype = r->conduit;
		ConduitType* reqct = &g_cotype[reqctype];

		Vec2i tpos = cmpos / TILE_SIZE;

		int cmminx;
		int cmminz;
		int cmmaxx;
		int cmmaxz;
		int cmminx2;
		int cmminz2;
		int cmmaxx2;
		int cmmaxz2;

		cmminx = tpos.x * TILE_SIZE + reqct->physoff.x - TILE_SIZE/2;
		cmminz = tpos.y * TILE_SIZE + reqct->physoff.y - TILE_SIZE/2;
		cmmaxx = cmminx + TILE_SIZE;
		cmmaxz = cmminz + TILE_SIZE;
		cmminx2 = cmmaxx;
		cmminz2 = cmmaxz;
		cmmaxx2 = cmminx2 + TILE_SIZE;
		cmmaxz2 = cmminz2 + TILE_SIZE;
		
		cmminx -= 1;
		cmminz -= 1;
		cmminx2 -= 1;
		cmminz2 -= 1;

		if(GetCo(reqctype, tpos.x, tpos.y, false)->on)
			continue;

		if(tpos.x-1 >= 0 && tpos.y-1 >= 0 && GetCo(reqctype, tpos.x-1, tpos.y-1, false)->on)
			if(cmmaxx >= cmpos.x && cmmaxz >= cmpos.y && cmminx <= cmpos.x && cmminz <= cmpos.y)
				continue;
		
		if(tpos.x+1 < g_hmap.m_widthx && tpos.y-1 >= 0 && GetCo(reqctype, tpos.x+1, tpos.y-1, false)->on)
			if(cmmaxx2 >= cmpos.x && cmmaxz >= cmpos.y && cmminx2 <= cmpos.x && cmminz <= cmpos.y)
				continue;
		
		if(tpos.x+1 < g_hmap.m_widthx && tpos.y+1 < g_hmap.m_widthz && GetCo(reqctype, tpos.x+1, tpos.y+1, false)->on)
			if(cmmaxx2 >= cmpos.x && cmmaxz2 >= cmpos.y && cmminx2 <= cmpos.x && cmminz2 <= cmpos.y)
				continue;
		
		if(tpos.x-1 >= 0 && tpos.y+1 < g_hmap.m_widthz && GetCo(reqctype, tpos.x-1, tpos.y+1, false)->on)
			if(cmmaxx >= cmpos.x && cmmaxz2 >= cmpos.y && cmminx <= cmpos.x && cmminz2 <= cmpos.y)
				continue;

		return false;
	}
#endif

	if(!ct->cornerpl)
	{
		if(CollidesWithBuildings(cmpos.x, cmpos.y, cmpos.x, cmpos.y))
			return false;
	}
	else
	{
		if(CollidesWithBuildings(cmpos.x+1, cmpos.y+1, cmpos.x-1, cmpos.y-1))
			return false;

		bool nw_occupied = false;
		bool se_occupied = false;
		bool sw_occupied = false;
		bool ne_occupied = false;

		Vec2i nw_tile_center = cmpos + Vec2i(-TILE_SIZE/2, -TILE_SIZE/2);
		Vec2i se_tile_center = cmpos + Vec2i(TILE_SIZE/2, TILE_SIZE/2);
		Vec2i sw_tile_center = cmpos + Vec2i(-TILE_SIZE/2, TILE_SIZE/2);
		Vec2i ne_tile_center = cmpos + Vec2i(TILE_SIZE/2, -TILE_SIZE/2);

		//Make sure the conduit corner isn't surrounded by buildings or map edges

		if(x<=0 || z<=0)
			nw_occupied = true;
		else if(CollidesWithBuildings(nw_tile_center.x, nw_tile_center.y, nw_tile_center.x, nw_tile_center.y))
			nw_occupied = true;

		if(x<=0 || z>=g_hmap.m_widthz-1)
			sw_occupied = true;
		else if(CollidesWithBuildings(sw_tile_center.x, sw_tile_center.y, sw_tile_center.x, sw_tile_center.y))
			sw_occupied = true;

		if(x>=g_hmap.m_widthx-1 || z>=g_hmap.m_widthz-1)
			se_occupied = true;
		else if(CollidesWithBuildings(se_tile_center.x, se_tile_center.y, se_tile_center.x, se_tile_center.y))
			se_occupied = true;

		if(x>=g_hmap.m_widthx-1 || z<=0)
			ne_occupied = true;
		else if(CollidesWithBuildings(ne_tile_center.x, ne_tile_center.y, ne_tile_center.x, ne_tile_center.y))
			ne_occupied = true;

		if( nw_occupied && sw_occupied && se_occupied && ne_occupied )
			return false;
	}

	return true;
}

int GetConn(char ctype, int x, int z, bool plan=false)
{
	bool n = false, e = false, s = false, w = false;

	if(x+1 < g_hmap.m_widthx && GetCo(ctype, x+1, z, false)->on)
		e = true;
	if(x-1 >= 0 && GetCo(ctype, x-1, z, false)->on)
		w = true;

	if(z+1 < g_hmap.m_widthz && GetCo(ctype, x, z+1, false)->on)
		s = true;
	if(z-1 >= 0 && GetCo(ctype, x, z-1, false)->on)
		n = true;

	if(plan)
	{
		if(x+1 < g_hmap.m_widthx && GetCo(ctype, x+1, z, true)->on)
			e = true;
		if(x-1 >= 0 && GetCo(ctype, x-1, z, true)->on)
			w = true;

		if(z+1 < g_hmap.m_widthz && GetCo(ctype, x, z+1, true)->on)
			s = true;
		if(z-1 >= 0 && GetCo(ctype, x, z-1, true)->on)
			n = true;
	}

	return ConnType(n, e, s, w);
}

void ConnectCo(char ctype, int tx, int tz, bool plan)
{
	ConduitTile* ctile = GetCo(ctype, tx, tz, plan);

	if(!ctile->on)
		return;

	ctile->conntype = GetConn(ctype, tx, tz, plan);
	RemeshCo(ctype, tx, tz, plan);
}

void ConnectCoAround(char ctype, int x, int z, bool plan)
{
	if(x+1 < g_hmap.m_widthx)
		ConnectCo(ctype, x+1, z, plan);
	if(x-1 >= 0)
		ConnectCo(ctype, x-1, z, plan);

	if(z+1 < g_hmap.m_widthz)
		ConnectCo(ctype, x, z+1, plan);
	if(z-1 >= 0)
		ConnectCo(ctype, x, z-1, plan);
}

void PlaceCo(char ctype, int tx, int tz, int owner, bool plan)
{
	if(!plan && GetCo(ctype, tx, tz, false)->on)
	{
		Repossess(ctype, tx, tz, owner);
		return;
	}

	if(!CoPlaceable(ctype, tx, tz))
		return;

	ConduitTile* ctile = GetCo(ctype, tx, tz, plan);

	ctile->on = true;
	ctile->owner = owner;
	//Zero(ctile->maxcost);

	ConduitType* ct = &g_cotype[ctype];

	ctile->drawpos = Vec3f(tx*TILE_SIZE, 0, tz*TILE_SIZE) + ct->drawoff;

	if(g_mode == APPMODE_PLAY)
		ctile->finished = false;
	//if(plan || g_mode == APPMODE_EDITOR)
	if(plan)
		ctile->finished = true;

	ConnectCo(ctype, tx, tz, plan);
	ConnectCoAround(ctype, tx, tz, plan);

	for(int i=0; i<RESOURCES; i++)
		ctile->transporter[i] = -1;

	//if(!plan)
	//	ReRoadNetw();

	if(!plan && !ct->cornerpl)
	{
		ClearFol(tx*TILE_SIZE, tz*TILE_SIZE, tx*TILE_SIZE + TILE_SIZE, tz*TILE_SIZE + TILE_SIZE);

		g_hmap.hidetile(tx, tz);
	}
}

void PlaceCo(char ctype)
{
	Player* py = &g_player[g_curP];

	if(g_mode == APPMODE_PLAY)
		ClearSel(&py->sel);

	ConduitType* ct = &g_cotype[ctype];
	std::list<Vec2i>& csel = *(std::list<Vec2i>*)(((char*)&py->sel)+ct->seloff);

	for(int x=0; x<g_hmap.m_widthx; x++)
	{
		for(int z=0; z<g_hmap.m_widthz; z++)
		{
			ConduitTile* plan = GetCo(ctype, x, z, true);

			if(plan->on)
			{
				ConduitTile* actual = GetCo(ctype, x, z, false);
				bool willchange = !actual->on;
				PlaceCo(ctype, x, z, plan->owner, false);

				if(g_mode == APPMODE_PLAY && !actual->finished)
					csel.push_back(Vec2i(x,z));

				if(g_mode == APPMODE_PLAY && willchange)
				{
#if 0
					NewJob(GOINGTOROADJOB, x, z);
#endif
				}
			}
		}
	}

	ClearCoPlans(ctype);
	ReNetw(ctype);

	if(!ct->cornerpl)
		g_hmap.genvbo();

	if(g_mode == APPMODE_PLAY)
	{
		Player* py = &g_player[g_curP];
		GUI* gui = &py->gui;

		if(py->sel.roads.size() > 0)
		{
			ConstructionView* cv = (ConstructionView*)gui->get("construction view")->get("construction view");
			cv->regen(&py->sel);
			gui->open("construction view");
		}
	}
}

void DrawCo(char ctype)
{
	//StartTimer(TIMER_DRAWROADS);

	Player* py = &g_player[g_curP];
	ConduitType* ct = &g_cotype[ctype];
	Shader* s = &g_shader[g_curS];

	for(int x=0; x<g_hmap.m_widthx; x++)
		for(int z=0; z<g_hmap.m_widthz; z++)
		{
			ConduitTile* ctile = GetCo(ctype, x, z, false);

			if(!ctile->on)
				continue;

			const float* owncol = g_player[ctile->owner].colorcode;
			glUniform4f(s->m_slot[SSLOT_OWNCOLOR], owncol[0], owncol[1], owncol[2], owncol[3]);
			
			const int mi = ct->model[ctile->conntype][(int)ctile->finished];
			const Model* m = &g_model[mi];
			m->usetex();

			DrawVA(&ctile->drawva, ctile->drawpos);
		}

	if(py->build != BUILDING_TYPES + ctype)
		return;

	glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 0.5f);

	for(int x=0; x<g_hmap.m_widthx; x++)
		for(int z=0; z<g_hmap.m_widthz; z++)
		{
			ConduitTile* ctile = GetCo(ctype, x, z, true);

			if(!ctile->on)
				continue;

			const float* owncol = g_player[ctile->owner].colorcode;
			glUniform4f(s->m_slot[SSLOT_OWNCOLOR], owncol[0], owncol[1], owncol[2], owncol[3]);
			
			const int mi = ct->model[ctile->conntype][(int)ctile->finished];
			const Model* m = &g_model[mi];
			m->usetex();

			DrawVA(&ctile->drawva, ctile->drawpos);
		}

	glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	//StopTimer(TIMER_DRAWROADS);
}

ConduitTile::ConduitTile()
{
	on = false;
	finished = false;
	Zero(conmat);
	netw = -1;
}

ConduitTile::~ConduitTile()
{
}

char ConduitTile::condtype()
{
	return CONDUIT_NONE;
}

int ConduitTile::netreq(int res)
{
	int netrq = 0;

	if(!finished)
	{
		ConduitType* ct = &g_cotype[condtype()];
		netrq = ct->conmat[res] - conmat[res];
	}

	return netrq;
}

void ConduitTile::destroy()
{
	Zero(conmat);
	on = false;
	finished = false;
	netw = -1;
	freecollider();
}

void ConduitTile::allocate()
{
#if 0
	CPlayer* p = &g_player[owner];

	float alloc;

	char transx[32];
	transx[0] = '\0';

	for(int i=0; i<RESOURCES; i++)
	{
		if(g_roadcost[i] <= 0)
			continue;

		if(i == LABOUR)
			continue;

		alloc = g_roadcost[i] - conmat[i];

		if(p->global[i] < alloc)
			alloc = p->global[i];

		conmat[i] += alloc;
		p->global[i] -= alloc;

		if(alloc > 0.0f)
			TransxAppend(transx, i, -alloc);
	}

	if(transx[0] != '\0'
#ifdef LOCAL_TRANSX
			&& owner == g_localP
#endif
	  )
	{
		int x, z;
		RoadXZ(this, x, z);
		NewTransx(RoadPosition(x, z), transx);
	}

	checkconstruction();
#endif
}

bool ConduitTile::checkconstruction()
{
	ConduitType* ct = &g_cotype[condtype()];

	for(int i=0; i<RESOURCES; i++)
		if(conmat[i] < ct->conmat[i])
			return false;

#if 0
	if(owner == g_localP)
	{
		Chat("Road construction complete.");
		ConCom();
	}

	finished = true;
	fillcollider();

	int x, z;
	RoadXZ(this, x, z);
	MeshRoad(x, z);
	ReRoadNetw();

	if(owner == g_localP)
		OnFinishedB(ROAD);
#endif

	fillcollider();

	return true;
}

void ConduitTile::fillcollider()
{
}

void ConduitTile::freecollider()
{
}

void DefConn(char conduittype, char connectiontype, bool finished, const char* modelfile, const Vec3f scale, Vec3f transl)
{
	int* tm = &g_cotype[conduittype].model[connectiontype][(int)finished];
	QueueModel(tm, modelfile, scale, transl);
}

void DefCo(char ctype, 
		   unsigned short netwoff, 
		   unsigned short seloff, 
		   unsigned short maxforwincl, 
		   unsigned short maxsideincl, 
		   bool blconduct, 
		   bool cornerpl, 
		   Vec2i physoff, 
		   Vec3f drawoff)
{
	ConduitType* ct = &g_cotype[ctype];
	ct->netwoff = netwoff;
	ct->seloff = seloff;
	ct->maxforwincl = maxforwincl;
	ct->maxsideincl = maxsideincl;
	ct->physoff = physoff;
	ct->drawoff = drawoff;
	ct->cornerpl = cornerpl;
	ct->blconduct = blconduct;
}

void CoConMat(char ctype, char rtype, short ramt)
{
	ConduitType* ct = &g_cotype[ctype];
	ct->conmat[rtype] = ramt;
}

void CoXZ(char ctype, ConduitTile* ctile, bool plan, int& tx, int& tz)
{
	ConduitType* ct = &g_cotype[ctype];
	ConduitTile* tilearr = ct->cotiles[(int)plan];
	int off = ctile - tilearr;
	tz = off / g_hmap.m_widthx;
	tx = off % g_hmap.m_widthx;
}

void PruneCo(char ctype)
{
	ConduitType* ct = &g_cotype[ctype];

	for(int x=0; x<g_hmap.m_widthx; x++)
		for(int z=0; z<g_hmap.m_widthz; z++)
			if(GetCo(ctype, x, z, false)->on && !CoPlaceable(ctype, x, z))
			{
				GetCo(ctype, x, z, false)->on = false;
				ConnectCoAround(ctype, x, z, false);
			}
}