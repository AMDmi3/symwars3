#include "build.h"
#include "buildingtype.h"
#include "road.h"
#include "powl.h"
#include "crpipe.h"
#include "../render/shader.h"
#include "../platform.h"
#include "../window.h"
#include "../math/camera.h"
#include "../render/heightmap.h"
#include "../math/hmapmath.h"
#include "building.h"
#include "../utils.h"
#include "unittype.h"
#include "unit.h"
#include "../render/water.h"
#include "../phys/collision.h"
#include "../gui/richtext.h"
#include "../gui/font.h"
#include "../math/vec4f.h"
#include "../gui/icon.h"
#include "player.h"
#include "../../game/gmain.h"
#include "../gui/gui.h"
#include "../gui/widget.h"
#include "../gui/widgets/spez/constructionview.h"
#include "../render/foliage.h"
#include "unitmove.h"

void UpdateSBuild()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	if(py->build == BUILDING_NONE)
		return;

	//py->vdrag[0] = Vec3f(-1,-1,-1);
	//py->vdrag[1] = Vec3f(-1,-1,-1);

	Vec3f ray = ScreenPerspRay(py->mouse.x, py->mouse.y, py->width, py->height, c->zoompos(), c->m_strafe, c->up2(), c->m_view - c->m_pos, FIELD_OF_VIEW);

	Vec3f intersection;

	Vec3f line[2];
	line[0] = c->zoompos();
	line[1] = line[0] + ray * MAX_DISTANCE / 3 / py->zoom;

#if 1
	if(!FastMapIntersect(&g_hmap, line, &intersection))
#else
	if(!GetMapIntersection(&g_hmap, line, &intersection))
#endif
		return;

	if(!py->mousekeys[0])
		py->vdrag[0] = intersection;
	else
		py->vdrag[1] = intersection;

	py->canplace = true;

	if(py->build < BUILDING_TYPES)
	{
		Vec2i tilepos (intersection.x/TILE_SIZE, intersection.z/TILE_SIZE);

		py->vdrag[0].x = tilepos.x * TILE_SIZE;
		py->vdrag[0].z = tilepos.y * TILE_SIZE;

		BuildingT* t = &g_bltype[py->build];

		if(t->widthx%2 == 1)
			py->vdrag[0].x += TILE_SIZE/2;
		if(t->widthz%2 == 1)
			py->vdrag[0].z += TILE_SIZE/2;

		py->vdrag[0].y = Lowest(tilepos.x, tilepos.y, tilepos.x, tilepos.y);

		if(!CheckCanPlace(py->build, tilepos))
		{
			py->canplace = false;
			py->bpcol = g_collidertype;
		}
		//PlaceBuilding(type, tilepos, true, -1, -1, -1);
	}
	else if(py->build == BUILDING_ROAD)
	{
		if(py->mousekeys[0])
			UpdCoPlans(CONDUIT_ROAD, g_localP, py->vdrag[0], py->vdrag[1]);
		else
		{
			ClearCoPlans(CONDUIT_ROAD);
			UpdCoPlans(CONDUIT_ROAD, g_localP, py->vdrag[0], py->vdrag[0]);
		}
	}
	else if(py->build == BUILDING_POWL)
	{
		if(py->mousekeys[0])
			UpdCoPlans(CONDUIT_POWL, g_localP, py->vdrag[0], py->vdrag[1]);
		else
		{
			ClearCoPlans(CONDUIT_POWL);
			UpdCoPlans(CONDUIT_POWL, g_localP, py->vdrag[0], py->vdrag[0]);
		}
	}
	else if(py->build == BUILDING_CRPIPE)
	{
		if(py->mousekeys[0])
			UpdCoPlans(CONDUIT_CRPIPE, g_localP, py->vdrag[0], py->vdrag[1]);
		else
		{
			ClearCoPlans(CONDUIT_CRPIPE);
			UpdCoPlans(CONDUIT_CRPIPE, g_localP, py->vdrag[0], py->vdrag[0]);
		}
	}
}

void DrawSBuild()
{
	Player* py = &g_player[g_curP];

	if(py->build == BUILDING_NONE)
		return;

	Shader* s = &g_shader[g_curS];

	if(py->build < BUILDING_TYPES)
	{
		//g_log<<"dr"<<std::endl;

		if(py->canplace)
			glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 0.5f);
		else
			glUniform4f(s->m_slot[SSLOT_COLOR], 1, 0, 0, 0.5f);

		const BuildingT* t = &g_bltype[py->build];
		Model* m = &g_model[ t->model ];
		m->draw(0, py->vdrag[0], 0);
	}
	else if(py->build == BUILDING_ROAD)
	{
	}
	else if(py->build == BUILDING_POWL)
	{
	}
	else if(py->build == BUILDING_CRPIPE)
	{
	}
}

void DrawBReason(Matrix* mvp, float width, float height, bool persp)
{
	Player* py = &g_player[g_curP];

	if(py->canplace || py->build == BUILDING_NONE)
		return;

	Vec3f pos3 = py->vdrag[0];

	if(py->build >= BUILDING_TYPES)
		pos3 = py->vdrag[1];

	RichText reason;

	Vec4f pos4 = ScreenPos(mvp, pos3, width, height, persp);

	switch(py->bpcol)
	{
	case COLLIDER_NONE:
		reason.m_part.push_back(RichTextP(UString("")));
		break;
	case COLLIDER_UNIT:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" A unit is in the way.")));
		break;
	case COLLIDER_BUILDING:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" Another building is in the way.")));
		break;
	case COLLIDER_TERRAIN:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" Buildings must be placed on even terrain.")));
		break;
	case COLLIDER_NOROAD:
		reason.m_part.push_back(RichTextP(UString("")));
		break;
	case COLLIDER_OTHER:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" Can't place here.")));
		break;
	case COLLIDER_NOLAND:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" This building must be placed on land.")));
		break;
	case COLLIDER_NOSEA:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" This structure must be placed in the sea.")));
		break;
	case COLLIDER_NOCOAST:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" This building must be placed along the coast.")));
		break;
	case COLLIDER_ROAD:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" A road is in the way.")));
		break;
	case COLLIDER_OFFMAP:
		reason.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_EXCLAMATION));
		reason.m_part.push_back(RichTextP(UString(" Building is out of bounds.")));
		break;
	}

	float color[] = {0.9,0.7,0.2,0.8};
	//DrawCenterShadText(MAINFONT32, pos4.x, pos4.y-64, &reason, color, -1);
	DrawBoxShadText(MAINFONT32, pos4.x-200, pos4.y-64-100, 400, 200, &reason, color, 0, -1);
}

bool BuildingLevel(int type, Vec2i tpos)
{
#if 1
	BuildingT* t = &g_bltype[type];

	Vec2i tmin;
	Vec2i tmax;

	tmin.x = tpos.x - t->widthx/2;
	tmin.y = tpos.y - t->widthz/2;
	tmax.x = tmin.x + t->widthx;
	tmax.y = tmin.y + t->widthz;

	float miny = g_hmap.getheight(tmin.x, tmin.y);
	float maxy = g_hmap.getheight(tmin.x, tmin.y);

	bool haswater = false;
	bool hasland = false;

	for(int x=tmin.x; x<=tmax.x; x++)
		for(int z=tmin.y; z<=tmax.y; z++)
		{
			float thisy = g_hmap.getheight(x, z);

			if(thisy < miny)
				miny = thisy;

			if(thisy > maxy)
				maxy = thisy;

			// Must have two adject water tiles to be water-vessel-accessible
			if(thisy < WATER_LEVEL)
			{
				// If z is along building edge and x and x+1 are water tiles
				if((z==tmin.y || z==tmax.y) && x+1 <= g_hmap.m_widthx && x+1 <= tmax.x && g_hmap.getheight(x+1, z) < WATER_LEVEL)
					haswater = true;
				// If x is along building edge and z and z+1 are water tiles
				if((x==tmin.x || x==tmax.x) && z+1 <= g_hmap.m_widthz && z+1 <= tmax.y && g_hmap.getheight(x, z+1) < WATER_LEVEL)
					haswater = true;
			}
			// Must have two adjacent land tiles to be road-accessible
			else if(thisy > WATER_LEVEL)
			{
				// If z is along building edge and x and x+1 are land tiles
				if((z==tmin.y || z==tmax.y) && x+1 <= g_hmap.m_widthx && x+1 <= tmax.x && g_hmap.getheight(x+1, z) > WATER_LEVEL)
					hasland = true;
				// If x is along building edge and z and z+1 are land tiles
				if((x==tmin.x || x==tmax.x) && z+1 <= g_hmap.m_widthz && z+1 <= tmax.y && g_hmap.getheight(x, z+1) > WATER_LEVEL)
					hasland = true;
			}
		}

	if(miny < WATER_LEVEL)
	{
#if 0
		haswater = true;
#endif
		miny = WATER_LEVEL;
	}

#if 0
	if(maxy > WATER_LEVEL)
		hasland = true;
#endif

	if(maxy - miny > MAX_CLIMB_INCLINE)
	{
		g_collidertype = COLLIDER_TERRAIN;
		return false;
	}

	if(t->foundation == FOUNDATION_LAND)
	{
		if(haswater)
		{
			g_collidertype = COLLIDER_NOLAND;
			return false;
		}
		if(!hasland)
		{
			g_collidertype = COLLIDER_NOLAND;
			return false;
		}
	}
	else if(t->foundation == FOUNDATION_SEA)
	{
		if(!haswater)
		{
			g_collidertype = COLLIDER_NOSEA;
			return false;
		}
		if(hasland)
		{
			g_collidertype = COLLIDER_NOSEA;
			return false;
		}
	}
	else if(t->foundation == FOUNDATION_COASTAL)
	{
		if(!haswater || !hasland)
		{
			g_collidertype = COLLIDER_NOCOAST;
			return false;
		}
	}

#if 0
	for(int x=tmin.x; x<=tmax.x; x++)
		for(int z=tmin.y; z<=tmax.y; z++)
		{
			if(g_hmap.getheight(x, z) != compare)
				return false;

			if(g_hmap.getheight(x, z) <= WATER_LEVEL)
				return false;
		}
#endif
#endif

	return true;
}

bool Offmap(int minx, int minz, int maxx, int maxz)
{
	if(minx < 0 || minz < 0
			|| maxx >= g_hmap.m_widthx*TILE_SIZE
			|| maxz >= g_hmap.m_widthz*TILE_SIZE)
	{
		g_collidertype = COLLIDER_OFFMAP;
		return true;
	}

	return false;
}

bool BuildingCollides(int type, Vec2i pos)
{
	BuildingT* t = &g_bltype[type];

	Vec2i tmin;
	Vec2i tmax;

	tmin.x = pos.x - t->widthx/2;
	tmin.y = pos.y - t->widthz/2;
	tmax.x = tmin.x + t->widthx - 1;
	tmax.y = tmin.y + t->widthz - 1;

	Vec2i cmmin;
	Vec2i cmmax;

	cmmin.x = tmin.x * TILE_SIZE;
	cmmin.y = tmin.y * TILE_SIZE;
	cmmax.x = cmmin.x + t->widthx*TILE_SIZE - 1;
	cmmax.y = cmmin.y + t->widthz*TILE_SIZE - 1;

	if(Offmap(cmmin.x, cmmin.y, cmmax.x, cmmax.y))
		return true;

	for(int x=tmin.x; x<=tmax.x; x++)
		for(int z=tmin.y; z<=tmax.y; z++)
			if(GetCo(CONDUIT_ROAD, x, z, false)->on)
			{
				g_collidertype = COLLIDER_ROAD;
				return true;
			}

	if(CollidesWithBuildings(cmmin.x, cmmin.y, cmmax.x, cmmax.y, -1))
		return true;

	if(CollidesWithUnits(cmmin.x, cmmin.y, cmmax.x, cmmax.y, false, NULL, NULL))
		return true;

	return false;
}

bool CheckCanPlace(int type, Vec2i pos)
{
	if(!BuildingLevel(type, pos))
		return false;

	if(BuildingCollides(type, pos))
		return false;

	return true;
}

bool PlaceBuilding(int type, Vec2i pos, bool finished, int owner, int* bid)
{
	int i = NewBuilding();

	if(bid)
		*bid = i;

	if(i < 0)
		return false;

	Building* b = &g_building[i];
	b->on = true;
	b->type = type;
	b->tilepos = pos;

	BuildingT* t = &g_bltype[type];

	Vec2i tmin;
	Vec2i tmax;

	tmin.x = pos.x - t->widthx/2;
	tmin.y = pos.y - t->widthz/2;
	tmax.x = tmin.x + t->widthx;
	tmax.y = tmin.y + t->widthz;

	b->drawpos = Vec3f(pos.x*TILE_SIZE, Lowest(tmin.x, tmin.y, tmax.x, tmax.y), pos.y*TILE_SIZE);

	if(t->foundation == FOUNDATION_SEA)
		b->drawpos.y = WATER_LEVEL;

#if 1
	if(t->widthx % 2 == 1)
		b->drawpos.x += TILE_SIZE/2;

	if(t->widthz % 2 == 1)
		b->drawpos.z += TILE_SIZE/2;

	b->owner = owner;

	b->finished = finished;

	Zero(b->conmat);
	Zero(b->stocked);
	Zero(b->maxcost);
	Zero(b->prodprice);
	b->propprice = 0;

	int cmminx = tmin.x*TILE_SIZE;
	int cmminz = tmin.y*TILE_SIZE;
	int cmmaxx = cmminx + t->widthx*TILE_SIZE;
	int cmmaxz = cmminz + t->widthz*TILE_SIZE;

	ClearFol(cmminx, cmminz, cmmaxx, cmmaxz);
#if 0
	ClearPowerlines(cmminx, cmminz, cmmaxx, cmmaxz);
	ClearPipelines(cmminx, cmminz, cmmaxx, cmmaxz);
	RePow();
	RePipe();
	ReRoadNetw();
#endif

#endif

	b->remesh();

	if(g_mode == APPMODE_PLAY)
	{
		//b->allocres();
		b->inoperation = false;
		
		if(!b->finished && owner == g_curP)
		{
			Player* py = &g_player[g_curP];

			ClearSel(&py->sel);
			py->sel.buildings.push_back(i);

			GUI* gui = &py->gui;
			ConstructionView* cv = (ConstructionView*)gui->get("construction view")->get("construction view");
			cv->regen(&py->sel);
			gui->open("construction view");
		}
	}
	else
	{
		b->inoperation = true;
	}

	b->fillcollider();

	return true;
}

//find to place building about certain tile
bool PlaceBAb(int btype, Vec2i tabout, Vec2i* tplace)
{
	//g_log<<"PlaceBAround "<<player<<std::endl;
	//g_log.flush();
    
	BuildingT* t = &g_bltype[btype];
	int shell = 1;
    
	//char msg[128];
	//sprintf(msg, "place b a %f,%f,%f", vAround.x/16, vAround.y/16, vAround.z/16);
	//Chat(msg);
    
	do
	{
		std::vector<Vec2i> canplace;
		Vec2i ttry;
		int tilex, tilez;
		int left, right, top, bottom;
		left = tabout.x - shell;
		top = tabout.y - shell;
		right = tabout.x + shell;
		bottom = tabout.y + shell;

		canplace.reserve( (right-left)*2/TILE_SIZE + (bottom-top)*2/TILE_SIZE );
        
		tilez = top;
		for(tilex=left; tilex<right; tilex++)
		{
			ttry = Vec2i(tilex, tilez);

			int cmstartx = ttry.x*TILE_SIZE - t->widthx/2;
			int cmendx = cmstartx + t->widthx - 1;
			int cmstartz = ttry.y*TILE_SIZE - t->widthz/2;
			int cmendz = cmstartz + t->widthz - 1;
			
			if(t->widthx%2 == 1)
			{
				cmstartx += TILE_SIZE/2;
				cmendx += TILE_SIZE/2;
			}
			if(t->widthz%2 == 1)
			{
				cmstartz += TILE_SIZE/2;
				cmendz += TILE_SIZE/2;
			}
            
			if(cmstartx < 0)
				continue;
			else if(cmendx >= g_hmap.m_widthx * TILE_SIZE)
				continue;
			if(cmstartz < 0)
				continue;
			else if(cmendz >= g_hmap.m_widthz * TILE_SIZE)
				continue;
            
			//char msg[128];
			//sprintf(msg, "check %d,%d,%d,%d", startx, startz, endx, endz);
			//Chat(msg);
            
			if(!CheckCanPlace(btype, ttry))
				continue;
			canplace.push_back(ttry);
		}
        
		tilex = right;
		for(tilez=top; tilez<bottom; tilez++)
		{
			ttry = Vec2i(tilex, tilez);

			int cmstartx = ttry.x*TILE_SIZE - t->widthx/2;
			int cmendx = cmstartx + t->widthx - 1;
			int cmstartz = ttry.y*TILE_SIZE - t->widthz/2;
			int cmendz = cmstartz + t->widthz - 1;
			
			if(t->widthx%2 == 1)
			{
				cmstartx += TILE_SIZE/2;
				cmendx += TILE_SIZE/2;
			}
			if(t->widthz%2 == 1)
			{
				cmstartz += TILE_SIZE/2;
				cmendz += TILE_SIZE/2;
			}
            
			if(cmstartx < 0)
				continue;
			else if(cmendx >= g_hmap.m_widthx * TILE_SIZE)
				continue;
			if(cmstartz < 0)
				continue;
			else if(cmendz >= g_hmap.m_widthz * TILE_SIZE)
				continue;
            
			//char msg[128];
			//sprintf(msg, "check %d,%d,%d,%d", startx, startz, endx, endz);
			//Chat(msg);
            
			if(!CheckCanPlace(btype, ttry))
				continue;
			canplace.push_back(ttry);
		}
        
		tilez = bottom;
		for(tilex=right; tilex>left; tilex--)
		{
			ttry = Vec2i(tilex, tilez);

			int cmstartx = ttry.x*TILE_SIZE - t->widthx/2;
			int cmendx = cmstartx + t->widthx - 1;
			int cmstartz = ttry.y*TILE_SIZE - t->widthz/2;
			int cmendz = cmstartz + t->widthz - 1;
			
			if(t->widthx%2 == 1)
			{
				cmstartx += TILE_SIZE/2;
				cmendx += TILE_SIZE/2;
			}
			if(t->widthz%2 == 1)
			{
				cmstartz += TILE_SIZE/2;
				cmendz += TILE_SIZE/2;
			}
            
			if(cmstartx < 0)
				continue;
			else if(cmendx >= g_hmap.m_widthx * TILE_SIZE)
				continue;
			if(cmstartz < 0)
				continue;
			else if(cmendz >= g_hmap.m_widthz * TILE_SIZE)
				continue;
            
			//char msg[128];
			//sprintf(msg, "check %d,%d,%d,%d", startx, startz, endx, endz);
			//Chat(msg);
            
			if(!CheckCanPlace(btype, ttry))
				continue;
			canplace.push_back(ttry);
		}
        
		tilex = left;
		for(tilez=bottom; tilez>top; tilez--)
		{
			ttry = Vec2i(tilex, tilez);

			int cmstartx = ttry.x*TILE_SIZE - t->widthx/2;
			int cmendx = cmstartx + t->widthx - 1;
			int cmstartz = ttry.y*TILE_SIZE - t->widthz/2;
			int cmendz = cmstartz + t->widthz - 1;
			
			if(t->widthx%2 == 1)
			{
				cmstartx += TILE_SIZE/2;
				cmendx += TILE_SIZE/2;
			}
			if(t->widthz%2 == 1)
			{
				cmstartz += TILE_SIZE/2;
				cmendz += TILE_SIZE/2;
			}
            
			if(cmstartx < 0)
				continue;
			else if(cmendx >= g_hmap.m_widthx * TILE_SIZE)
				continue;
			if(cmstartz < 0)
				continue;
			else if(cmendz >= g_hmap.m_widthz * TILE_SIZE)
				continue;
            
			//char msg[128];
			//sprintf(msg, "check %d,%d,%d,%d", startx, startz, endx, endz);
			//Chat(msg);
            
			if(!CheckCanPlace(btype, ttry))
				continue;
			canplace.push_back(ttry);
		}
        
		if(canplace.size() > 0)
		{
			//Chat("placing");
			//g_log<<"placeb t="<<btype<<" "<<vTile.x<<","<<vTile.y<<","<<vTile.z<<"("<<(vTile.x/16)<<","<<(vTile.y/16)<<","<<(vTile.z/16)<<")"<<std::endl;
			//g_log.flush();
			//*tpos = canplace[ rand()%canplace.size() ];
            *tplace = canplace[ 0 ];

			return true;
		}
        
		//char msg[128];
		//sprintf(msg, "shell %d", shell);
		//Chat(msg);
        
		shell++;
	}while(shell < g_hmap.m_widthx || shell < g_hmap.m_widthz);
    
	return false;
}

//find to place unit about certain position
bool PlaceUAb(int utype, Vec2i cmabout, Vec2i* cmplace)
{
	//g_log<<"PlaceBAround "<<player<<std::endl;
	//g_log.flush();
    
	UnitT* t = &g_utype[utype];
	int shell = 1;
    
	//char msg[128];
	//sprintf(msg, "place b a %f,%f,%f", vAround.x/16, vAround.y/16, vAround.z/16);
	//Chat(msg);
    
	do
	{
		std::vector<Vec2i> canplace;
		Vec2i cmtry;
		int tilex, tilez;
		int left, right, top, bottom;
		left = cmabout.x - shell;
		top = cmabout.y - shell;
		right = cmabout.x + shell;
		bottom = cmabout.y + shell;

		canplace.reserve( (right-left)*2/t->size.x + (bottom-top)*2/t->size.z );
        
		tilez = top;
		for(tilex=left; tilex<right; tilex++)
		{
			cmtry = Vec2i(tilex, tilez);

			int cmstartx = cmtry.x - t->size.x/2;
			int cmendx = cmstartx + t->size.x - 1;
			int cmstartz = cmtry.y - t->size.z/2;
			int cmendz = cmstartz + t->size.z - 1;
            
			if(cmstartx < 0)
				continue;
			else if(cmendx >= g_hmap.m_widthx * TILE_SIZE)
				continue;
			if(cmstartz < 0)
				continue;
			else if(cmendz >= g_hmap.m_widthz * TILE_SIZE)
				continue;
            
			//char msg[128];
			//sprintf(msg, "check %d,%d,%d,%d", startx, startz, endx, endz);
			//Chat(msg);
            
			//if(!CheckCanPlace(btype, cmtry))
			if(UnitCollides(NULL, cmtry, utype))
				continue;
			canplace.push_back(cmtry);
		}
        
		tilex = right;
		for(tilez=top; tilez<bottom; tilez++)
		{
			cmtry = Vec2i(tilex, tilez);

			int cmstartx = cmtry.x - t->size.x/2;
			int cmendx = cmstartx + t->size.x - 1;
			int cmstartz = cmtry.y - t->size.z/2;
			int cmendz = cmstartz + t->size.z - 1;
			
			if(cmstartx < 0)
				continue;
			else if(cmendx >= g_hmap.m_widthx * TILE_SIZE)
				continue;
			if(cmstartz < 0)
				continue;
			else if(cmendz >= g_hmap.m_widthz * TILE_SIZE)
				continue;
            
			//char msg[128];
			//sprintf(msg, "check %d,%d,%d,%d", startx, startz, endx, endz);
			//Chat(msg);
            
			if(UnitCollides(NULL, cmtry, utype))
				continue;
			canplace.push_back(cmtry);
		}
        
		tilez = bottom;
		for(tilex=right; tilex>left; tilex--)
		{
			cmtry = Vec2i(tilex, tilez);

			int cmstartx = cmtry.x - t->size.x/2;
			int cmendx = cmstartx + t->size.x - 1;
			int cmstartz = cmtry.y - t->size.z/2;
			int cmendz = cmstartz + t->size.z - 1;
			
			if(cmstartx < 0)
				continue;
			else if(cmendx >= g_hmap.m_widthx * TILE_SIZE)
				continue;
			if(cmstartz < 0)
				continue;
			else if(cmendz >= g_hmap.m_widthz * TILE_SIZE)
				continue;
            
			//char msg[128];
			//sprintf(msg, "check %d,%d,%d,%d", startx, startz, endx, endz);
			//Chat(msg);
            
			if(UnitCollides(NULL, cmtry, utype))
				continue;
			canplace.push_back(cmtry);
		}
        
		tilex = left;
		for(tilez=bottom; tilez>top; tilez--)
		{
			cmtry = Vec2i(tilex, tilez);

			int cmstartx = cmtry.x - t->size.x/2;
			int cmendx = cmstartx + t->size.x - 1;
			int cmstartz = cmtry.y - t->size.z/2;
			int cmendz = cmstartz + t->size.z - 1;
			
			if(cmstartx < 0)
				continue;
			else if(cmendx >= g_hmap.m_widthx * TILE_SIZE)
				continue;
			if(cmstartz < 0)
				continue;
			else if(cmendz >= g_hmap.m_widthz * TILE_SIZE)
				continue;
            
			//char msg[128];
			//sprintf(msg, "check %d,%d,%d,%d", startx, startz, endx, endz);
			//Chat(msg);
            
			if(UnitCollides(NULL, cmtry, utype))
				continue;
			canplace.push_back(cmtry);
		}
        
		if(canplace.size() > 0)
		{
			//Chat("placing");
			//g_log<<"placeb t="<<btype<<" "<<vTile.x<<","<<vTile.y<<","<<vTile.z<<"("<<(vTile.x/16)<<","<<(vTile.y/16)<<","<<(vTile.z/16)<<")"<<std::endl;
			//g_log.flush();
			//*tpos = canplace[ rand()%canplace.size() ];
            *cmplace = canplace[ 0 ];

			return true;
		}
        
		//char msg[128];
		//sprintf(msg, "shell %d", shell);
		//Chat(msg);
        
		shell++;
	}while(shell < g_hmap.m_widthx || shell < g_hmap.m_widthz);
    
	return false;
}