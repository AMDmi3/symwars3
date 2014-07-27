#include "unit.h"
#include "../render/shader.h"
#include "unittype.h"
#include "../texture.h"
#include "../utils.h"
#include "player.h"
#include "../render/model.h"
#include "../math/hmapmath.h"
#include "unitmove.h"
#include "../render/transaction.h"
#include "sim.h"
#include "../phys/trace.h"
#include "../phys/collision.h"
#include "../path/collidertile.h"
#include "../path/pathdebug.h"
#include "../sim/buildingtype.h"
#include "../sim/building.h"
#include "../path/jpspath.h"
#include "../path/jpspartpath.h"
#include "../path/pathnode.h"
#include "../path/partialpath.h"

bool UnitCollides(Unit* u, Vec2i cmpos, int utype)
{
	UnitT* t = &g_utype[utype];
	int minx = cmpos.x - t->size.x/2;
	int minz = cmpos.y - t->size.z/2;
	int maxx = minx + t->size.x - 1;
	int maxz = minz + t->size.z - 1;

	int cx = cmpos.x / PATHNODE_SIZE;
	int cz = cmpos.y / PATHNODE_SIZE;

	ColliderTile* cell = ColliderTileAt(cx, cz);

	if(!t->seaborne && !(cell->flags & FLAG_HASLAND))
		return true;

	if(cell->flags & FLAG_ABRUPT)
		return true;

	if(u == g_pathunit)
	{
		g_log<<"================================"<<std::endl;
		g_log<<"unit "<<(int)(u - g_unit)<<" move simframe "<<g_simframe<<std::endl;
		g_log<<"cm pos = "<<u->cmpos.x<<","<<u->cmpos.y<<std::endl;
		g_log<<"cm rect = ("<<minx<<","<<minz<<")->("<<maxx<<","<<maxz<<")"<<std::endl;

		int cposx = cmpos.x / PATHNODE_SIZE;
		int cposz = cmpos.y / PATHNODE_SIZE;
		int cminx = minx / PATHNODE_SIZE;
		int cminz = minz / PATHNODE_SIZE;
		int cmaxx = maxx / PATHNODE_SIZE;
		int cmaxz = maxz / PATHNODE_SIZE;

		g_log<<"pathcell rect: ("<<cminx<<","<<cminz<<")->("<<cmaxx<<","<<cmaxz<<")"<<std::endl;
		g_log<<"pathcell pos: ("<<cposx<<","<<cposz<<")"<<std::endl;

		g_log.flush();
	}

#if 0
	for(int i=0; i<UNITS; i++)
	{
		Unit* u2 = &g_unit[i];

		if(!u2->on)
			continue;

		if(u2 == u)
			continue;

		t = &g_utype[u2->type];
		int minx2 = u2->cmpos.x - t->size.x/2;
		int minz2 = u2->cmpos.y - t->size.z/2;
		int maxx2 = minx2 + t->size.x;
		int maxz2 = minz2 + t->size.z;

		/*
		It's important to test for
		equality as being passable
		because the units might be
		right beside each other.
		*/
		if(minx >= maxx2)
			continue;

		if(minz >= maxz2)
			continue;

		if(maxx <= minx2)
			continue;

		if(maxz <= minz2)
			continue;

		if(u == g_pathunit)
		{
			g_log<<"collision with unit "<<i<<std::endl;

			g_log<<"\tcm pos = "<<u2->cmpos.x<<","<<u2->cmpos.y<<std::endl;
			g_log<<"\tcm rect = ("<<minx2<<","<<minz2<<")->("<<maxx2<<","<<maxz2<<")"<<std::endl;

			int cposx = u->cmpos.x / PATHNODE_SIZE;
			int cposz = u->cmpos.y / PATHNODE_SIZE;
			int cminx = minx / PATHNODE_SIZE;
			int cminz = minz / PATHNODE_SIZE;
			int cmaxx = maxx / PATHNODE_SIZE;
			int cmaxz = maxz / PATHNODE_SIZE;

			g_log<<"\tpathcell rect: ("<<cminx<<","<<cminz<<")->("<<cmaxx<<","<<cmaxz<<")"<<std::endl;
			g_log<<"\tpathcell pos: ("<<cposx<<","<<cposz<<")"<<std::endl;

			g_log<<"-----------------------------------------"<<std::endl;

			g_log.flush();
		}

		return true;
	}
#else

	int cminx = minx / PATHNODE_SIZE;
	int cminz = minz / PATHNODE_SIZE;
	int cmaxx = maxx / PATHNODE_SIZE;
	int cmaxz = maxz / PATHNODE_SIZE;

	for(int x=cminx; x<=cmaxx; x++)
		for(int z=cminz; z<=cmaxz; z++)
		{
			cell = ColliderTileAt(x, z);

			if(cell->building >= 0)
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

				if(minx <= maxx2 && minz <= maxz2 && maxx >= minx2 && maxz >= minz2)
				{
#if 0
					g_log<<"collides at cell ("<<(minx/PATHNODE_SIZE)<<","<<(minz/PATHNODE_SIZE)<<")->("<<(maxx/PATHNODE_SIZE)<<","<<(maxz/PATHNODE_SIZE)<<")"<<std::endl;
					g_log<<"subgoal = "<<(u->subgoal.x/PATHNODE_SIZE)<<","<<(u->subgoal.y/PATHNODE_SIZE)<<std::endl;
					g_log.flush();
#endif

					return true;
				}
			}

			for(short uiter = 0; uiter < MAX_COLLIDER_UNITS; uiter ++)
			{
				short uindex = cell->units[uiter];

				if(uindex < 0)
					continue;

				Unit* u2 = &g_unit[uindex];

				if(u2 == u)
					continue;

				UnitT* t2 = &g_utype[u2->type];
				int minx2 = u2->cmpos.x - t2->size.x/2;
				int minz2 = u2->cmpos.y - t2->size.z/2;
				int maxx2 = minx2 + t2->size.x - 1;
				int maxz2 = minz2 + t2->size.z - 1;

				if(minx <= maxx2 && minz <= maxz2 && maxx >= minx2 && maxz >= minz2)
					return true;
			}
		}
#endif


	if(u == g_pathunit)
	{
		g_log<<"no collision"<<std::endl;
		g_log<<"-----------------------------------------"<<std::endl;
		g_log.flush();
	}

	return false;
}

void MoveUnit(Unit* u)
{
	UnitT* t = &g_utype[u->type];
	u->prevpos = u->cmpos;
	u->collided = false;

	if(u->threadwait)
		return;

	if(Magnitude(u->goal - u->cmpos) <= t->cmspeed)
		return;

	if(u->underorder && u->target < 0 && Magnitude(u->goal - u->cmpos) <= PATHNODE_SIZE)
		return;

	if(u->path.size() <= 0 || *u->path.rbegin() != u->goal)
	{
#if 1
		if(t->military)
		{
			if(g_simframe - u->lastpath < u->pathdelay)
			{
				return;
			}

			//u->pathdelay += 1;
			//u->pathdelay *= 2;
			u->pathdelay += 10;
			u->lastpath = g_simframe;

			int nodesdist = Magnitude( u->goal - u->cmpos ) / PATHNODE_SIZE;
#if 1
			PartialPath(u->type, u->mode,
						u->cmpos.x, u->cmpos.y, u->target, u->target2, u->targtype, &u->path, &u->subgoal,
						u, NULL, NULL,
						u->goal.x, u->goal.y,
						u->goal.x, u->goal.y, u->goal.x, u->goal.y,
						nodesdist*10);
			//TILE_SIZE*4/PATHNODE_SIZE);
#else
			JPSPartPath(u->type, u->mode,
						u->cmpos.x, u->cmpos.y, u->target, u->target2, u->targtype, &u->path, &u->subgoal,
						u, NULL, NULL,
						u->goal.x, u->goal.y,
						u->goal.x, u->goal.y, u->goal.x, u->goal.y,
						nodesdist*4);
#endif

#if 0
			RichText rtext("ppathf");
			NewTransx(u->drawpos + Vec3f(0,t->size.y,0), &rtext);
#endif
		}
		else if(!u->pathblocked)
#endif
		{
#if 0
			if(!FullPath(0,
						 u->type, u->mode,
						 u->cmpos.x, u->cmpos.y, u->target, u->target, u->target2, u->path, u->subgoal,
						 u, NULL, NULL,
						 u->goal.x, u->goal.y,
						 u->goal.x, u->goal.y, u->goal.x, u->goal.y))
#endif

				JPSPath(
					u->type, u->mode,
					u->cmpos.x, u->cmpos.y, u->target, u->target2, u->targtype, &u->path, &u->subgoal,
					u, NULL, NULL,
					u->goal.x, u->goal.y,
					u->goal.x, u->goal.y, u->goal.x, u->goal.y);
		}

		return;
	}

	u->freecollider();

	Vec2i dir = u->subgoal - u->cmpos;

	if(Magnitude2(u->subgoal - u->cmpos) <= t->cmspeed * t->cmspeed)
	{
		u->cmpos = u->subgoal;

		if(u->path.size() >= 2)
		{
			u->path.erase( u->path.begin() );
			u->subgoal = *u->path.begin();
			dir = u->subgoal - u->cmpos;
		}
#if 0
		else
		{
			u->fillcollider();
			u->drawpos.x = u->cmpos.x;
			u->drawpos.z = u->cmpos.y;
			u->drawpos.y = g_hmap.accheight(u->drawpos.x, u->drawpos.z);
			u->rotation.y = GetYaw(dir.x, dir.y);
			return;
		}
#endif
	}

	if(dir.x != 0 || dir.y != 0)
	{
		u->rotation.y = GetYaw(dir.x, dir.y);

		int mag = Magnitude(dir);
#if 0
		if(mag <= 0)
			mag = 1;
#endif

		Vec2i scaleddir = dir * t->cmspeed / mag;
		u->cmpos = u->cmpos + scaleddir;

#if 1
		if(UnitCollides(u, u->cmpos, u->type))
#else
		if(Trace(u->type, u->mode, u->prevpos, u->cmpos, u, NULL, NULL) != COLLIDER_NONE)
#endif
		{
			u->collided = true;
			u->cmpos = u->prevpos;
			u->path.clear();
			u->subgoal = u->cmpos;
			u->fillcollider();
			return;
		}
#if 0
		u->collided = false;
#endif
	}

	if(UnitCollides(u, u->cmpos, u->type))
		u->collided = true;

	u->fillcollider();
	u->drawpos.x = u->cmpos.x;
	u->drawpos.z = u->cmpos.y;
	u->drawpos.y = g_hmap.accheight(u->cmpos.x, u->cmpos.y);
}
