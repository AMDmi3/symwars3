#include "../render/heightmap.h"
#include "../texture.h"
#include "../utils.h"
#include "../math/vec3f.h"
#include "../window.h"
#include "../math/camera.h"
#include "../sim/unit.h"
#include "../sim/unittype.h"
#include "../math/hmapmath.h"
#include "../render/foliage.h"
#include "../sim/unit.h"
#include "../render/foliage.h"
#include "savemap.h"
#include "../sim/building.h"
#include "../sim/road.h"
#include "../sim/powl.h"
#include "../sim/crpipe.h"
#include "../sim/deposit.h"
#include "../render/water.h"
#include "../sim/player.h"
#include "../sim/selection.h"
#include "../path/collidertile.h"
#include "../path/pathjob.h"
#include "../path/pathnode.h"
#include "../debug.h"

float ConvertHeight(unsigned char brightness)
{
	// Apply exponential scale to height data.
	float y = (float)brightness*TILE_Y_SCALE/255.0f - TILE_Y_SCALE/2.0f;
	y = y / fabs(y) * pow(fabs(y), TILE_Y_POWER) * TILE_Y_AFTERPOW;

	if(y <= WATER_LEVEL)
		y -= TILE_SIZE;
	else if(y > WATER_LEVEL && y < WATER_LEVEL + TILE_SIZE/100)
		y += TILE_SIZE/100;

	return y;
}

void PlaceUnits()
{
	UnitT* ut = &g_utype[UNIT_LABOURER];
	PathJob pj;
	pj.airborne = ut->airborne;
	pj.landborne = ut->landborne;
	pj.seaborne = ut->seaborne;
	pj.roaded = ut->roaded;
	pj.ignoreb = 0;
	pj.ignoreu = 0;
	pj.thisu = 0;
	pj.utype = UNIT_LABOURER;

	for(int li=0; li<6*PLAYERS; li++)
	{
		int ntries = 0;

		for(; ntries < 100; ntries++)
		{
			Vec2i cmpos(rand()%g_hmap.m_widthx*TILE_SIZE, rand()%g_hmap.m_widthz*TILE_SIZE);
			Vec2i tpos = cmpos / TILE_SIZE;
			Vec2i npos = cmpos / PATHNODE_SIZE;

			pj.cmstartx = cmpos.x;
			pj.cmstartz = cmpos.y;

			if(Walkable2(&pj, cmpos.x, cmpos.y))
			{
				PlaceUnit(UNIT_LABOURER, cmpos, -1, NULL);
				//g_log<<"placed "<<li<<" at"<<tpos.x<<","<<tpos.y<<std::endl;
				break;
			}
			//else
			//g_log<<"not placed at"<<tpos.x<<","<<tpos.y<<std::endl;
		}
	}
}

void LoadJPGMap(const char* relative)
{
	g_hmap.destroy();
	g_hmap2.destroy();
	g_hmap4.destroy();
	g_hmap8.destroy();

	CheckGLError(__FILE__, __LINE__);

	LoadedTex *pImage = NULL;

	char full[1024];
	FullPath(relative, full);

	pImage = LoadJPG(full);

	if(!pImage)
		return;

	CheckGLError(__FILE__, __LINE__);

	g_hmap.allocate((pImage->sizeX-1), (pImage->sizeY-1));
#if 0
	g_hmap2.allocate((pImage->sizeX-1)/2, (pImage->sizeY-1)/2);
	g_hmap4.allocate((pImage->sizeX-1)/4, (pImage->sizeY-1)/4);
	g_hmap8.allocate((pImage->sizeX-1)/8, (pImage->sizeY-1)/8);
#endif

	CheckGLError(__FILE__, __LINE__);
	for(int x=0; x<pImage->sizeX; x++)
	{
		for(int z=0; z<pImage->sizeY; z++)
		{
			float r = (unsigned int)pImage->data[ z*pImage->sizeX*3 + x*3 + 0 ];
			float g = (unsigned int)pImage->data[ z*pImage->sizeX*3 + x*3 + 1 ];
			float b = (unsigned int)pImage->data[ z*pImage->sizeX*3 + x*3 + 2 ];

			// Apply exponential scale to height data.
			float y = ConvertHeight((r+g+b)/3.0f);

			g_hmap.setheight(x, z, y);

#if 0
			if(x%2 == 0 && z%2 == 0)
				g_hmap2.setheight(x/2, z/2, y);

			if(x%4 == 0 && z%4 == 0)
				g_hmap4.setheight(x/4, z/4, y);

			if(x%8 == 0 && z%8 == 0)
				g_hmap8.setheight(x/8, z/8, y);
#endif
		}
	}

	CheckGLError(__FILE__, __LINE__);
	AllocWater(g_hmap.m_widthx, g_hmap.m_widthz);
	CheckGLError(__FILE__, __LINE__);

	g_hmap.remesh(1);
#if 0
	g_hmap2.remesh(2);
	g_hmap4.remesh(4);
	g_hmap8.remesh(8);
#endif
	CheckGLError(__FILE__, __LINE__);

#if 1
	AllocPathGrid(g_hmap.m_widthx*TILE_SIZE, g_hmap.m_widthz*TILE_SIZE);
	AllocGrid((pImage->sizeX-1), (pImage->sizeY-1));
	FillColliderGrid();
#endif

	CheckGLError(__FILE__, __LINE__);
	FillForest();
	//PlaceUnits();

	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	Vec3f center = Vec3f( g_hmap.m_widthx * TILE_SIZE/2.0f, g_hmap.getheight(g_hmap.m_widthx/2, g_hmap.m_widthz/2), g_hmap.m_widthz * TILE_SIZE/2.0f );
	Vec3f delta = center - c->m_view;
	c->move(delta);
	Vec3f viewvec = c->m_view - c->m_pos;
	viewvec = Normalize(viewvec) * imax(g_hmap.m_widthx, g_hmap.m_widthz) * TILE_SIZE;
	c->m_pos = c->m_view - viewvec;
	py->zoom = INI_ZOOM;

	if(pImage)
	{
		//free(pImage);								// Free the image structure
		delete pImage;

		g_log<<relative<<"\n\r";
		g_log.flush();
	}

	c->position(
		-1000.0f/3, 1000.0f/3 + 5000, -1000.0f/3,
		0, 5000, 0,
		0, 1, 0);

	c->position(1000.0f/3, 1000.0f/3, 1000.0f/3, 0, 0, 0, 0, 1, 0);

	c->move( Vec3f(g_hmap.m_widthx*TILE_SIZE/2, 1000, g_hmap.m_widthz*TILE_SIZE/2) );

	//CalcMapView();
}

void FreeMap()
{
	g_hmap.destroy();
	FreeGrid();
	FreeUnits();
	FreeFoliage();
	FreeBuildings();
	FreeDeposits();
	Player* py = &g_player[g_curP];
	py->sel.clear();
}

void SaveHeightmap(FILE *fp)
{
	fwrite(&g_hmap.m_widthx, sizeof(int), 1, fp);
	fwrite(&g_hmap.m_widthz, sizeof(int), 1, fp);

	fwrite(g_hmap.m_heightpoints, sizeof(float), (g_hmap.m_widthx+1)*(g_hmap.m_widthz+1), fp);
	//fwrite(g_hmap.m_countryowner, sizeof(int), g_hmap.m_widthx*g_hmap.m_widthz, fp);
}

void ReadHeightmap(FILE *fp)
{
	int widthx=0, widthz=0;

	fread(&widthx, sizeof(int), 1, fp);
	fread(&widthz, sizeof(int), 1, fp);

	g_hmap.allocate(widthx, widthz);
#if 0
	g_hmap2.allocate(widthx/2, widthz/2);
	g_hmap4.allocate(widthx/4, widthz/4);
	g_hmap8.allocate(widthx/8, widthz/8);
#endif

	fread(g_hmap.m_heightpoints, sizeof(float), (g_hmap.m_widthx+1)*(g_hmap.m_widthz+1), fp);
	//fread(g_hmap.m_countryowner, sizeof(int), g_hmap.m_widthx*g_hmap.m_widthz, fp);

#if 0
	for(int x=0; x<widthx+1; x++)
		for(int z=0; z<widthz+1; z++)
		{
			if(x%2 == 0 && z%2 == 0)
				g_hmap2.setheight(x/2, z/2, g_hmap.getheight(x, z));
			if(x%4 == 0 && z%4 == 0)
				g_hmap4.setheight(x/4, z/4, g_hmap.getheight(x, z));
			if(x%8 == 0 && z%8 == 0)
				g_hmap8.setheight(x/8, z/8, g_hmap.getheight(x, z));
		}
#endif

	g_hmap.remesh(1);
#if 0
	g_hmap2.remesh(2);
	g_hmap4.remesh(4);
	g_hmap8.remesh(8);
#endif

	AllocGrid(widthx, widthz);

	AllocPathGrid(widthx*TILE_SIZE, widthz*TILE_SIZE);
	//CalcMapView();
}

void SaveFoliage(FILE *fp)
{
	for(int i=0; i<FOLIAGES; i++)
	{
#if 0
		bool on;
		unsigned char type;
		Vec3f pos;
		float yaw;
#endif

		Foliage *f = &g_foliage[i];

		fwrite(&f->on, sizeof(bool), 1, fp);

		if(!f->on)
			continue;

		fwrite(&f->type, sizeof(unsigned char), 1, fp);
		fwrite(&f->pos, sizeof(Vec3f), 1, fp);
	}
}

void ReadFoliage(FILE *fp)
{
	for(int i=0; i<FOLIAGES; i++)
	{
		Foliage *f = &g_foliage[i];

		fread(&f->on, sizeof(bool), 1, fp);

		if(!f->on)
		{
			f->reinstance();
			continue;
		}

		fread(&f->type, sizeof(unsigned char), 1, fp);
		fread(&f->pos, sizeof(Vec3f), 1, fp);

		f->reinstance();
	}
}

void SaveDeposits(FILE *fp)
{
	for(int i=0; i<DEPOSITS; i++)
	{
#if 0
		bool on;
		bool occupied;
		int restype;
		int amount;
		Vec2i tilepos;
		Vec3f drawpos;
#endif

		Deposit *d = &g_deposit[i];

		fwrite(&d->on, sizeof(bool), 1, fp);

		if(!d->on)
			continue;

		fwrite(&d->occupied, sizeof(bool), 1, fp);
		fwrite(&d->restype, sizeof(int), 1, fp);
		fwrite(&d->amount, sizeof(int), 1, fp);
		fwrite(&d->tilepos, sizeof(Vec2i), 1, fp);
		fwrite(&d->drawpos, sizeof(Vec3f), 1, fp);
	}
}

void ReadDeposits(FILE *fp)
{
	for(int i=0; i<DEPOSITS; i++)
	{
		Deposit *d = &g_deposit[i];

		fread(&d->on, sizeof(bool), 1, fp);

		if(!d->on)
			continue;

		fread(&d->occupied, sizeof(bool), 1, fp);
		fread(&d->restype, sizeof(int), 1, fp);
		fread(&d->amount, sizeof(int), 1, fp);
		fread(&d->tilepos, sizeof(Vec2i), 1, fp);
		fread(&d->drawpos, sizeof(Vec3f), 1, fp);
	}
}

void SaveUnits(FILE *fp)
{
	for(int i=0; i<UNITS; i++)
	{
#if 0
		bool on;
		int type;
		int stateowner;
		int corpowner;
		int unitowner;

		/*
		The f (floating-point) position vectory is used for drawing.
		*/
		Vec3f fpos;

		/*
		The real position is stored in integers.
		*/
		Vec2i cmpos;
		Vec3f facing;
		Vec2f rotation;

		std::list<Vec2i> path;
		Vec2i goal;
#endif

		Unit *u = &g_unit[i];

		fwrite(&u->on, sizeof(bool), 1, fp);

		if(!u->on)
			continue;

		fwrite(&u->type, sizeof(int), 1, fp);
#if 0
		fwrite(&u->stateowner, sizeof(int), 1, fp);
		fwrite(&u->corpowner, sizeof(int), 1, fp);
		fwrite(&u->unitowner, sizeof(int), 1, fp);
#else
		fwrite(&u->owner, sizeof(int), 1, fp);
#endif

		fwrite(&u->drawpos, sizeof(Vec3f), 1, fp);
		fwrite(&u->cmpos, sizeof(Vec2i), 1, fp);
		fwrite(&u->rotation, sizeof(Vec2f), 1, fp);

		int pathsz = u->path.size();

		fwrite(&pathsz, sizeof(int), 1, fp);

		for(auto pathiter = u->path.begin(); pathiter != u->path.end(); pathiter++)
			fwrite(&*pathiter, sizeof(Vec2i), 1, fp);

		fwrite(&u->goal, sizeof(Vec2i), 1, fp);

#if 0
		int target;
		int target2;
		bool targetu;
		bool underorder;
		int fuelstation;
		int belongings[RESOURCES];
		int hp;
		bool passive;
		Vec2i prevpos;
		int taskframe;
		bool pathblocked;
		int frameslookjobago;
		int supplier;
		int reqamt;
		int targtype;
		int home;
		int car;
		//std::vector<TransportJob> bids;

		float frame[2];

		Vec2i subgoal;
#endif

		fwrite(&u->target, sizeof(int), 1, fp);
		fwrite(&u->target2, sizeof(int), 1, fp);
		fwrite(&u->targetu, sizeof(bool), 1, fp);
		fwrite(&u->underorder, sizeof(bool), 1, fp);
		fwrite(&u->fuelstation, sizeof(int), 1, fp);
		fwrite(u->belongings, sizeof(int), RESOURCES, fp);
		fwrite(&u->hp, sizeof(int), 1, fp);
		fwrite(&u->passive, sizeof(bool), 1, fp);
		fwrite(&u->prevpos, sizeof(Vec2i), 1, fp);
		fwrite(&u->taskframe, sizeof(int), 1, fp);
		fwrite(&u->pathblocked, sizeof(bool), 1, fp);
		fwrite(&u->frameslookjobago, sizeof(int), 1, fp);
		fwrite(&u->supplier, sizeof(int), 1, fp);
		fwrite(&u->reqamt, sizeof(int), 1, fp);
		fwrite(&u->targtype, sizeof(int), 1, fp);
		fwrite(&u->home, sizeof(int), 1, fp);
		fwrite(&u->car, sizeof(int), 1, fp);
		fwrite(&u->frame, sizeof(float), 2, fp);
		fwrite(&u->subgoal, sizeof(Vec2i), 1, fp);

#if 0
		unsigned char mode;
		int pathdelay;
		long long lastpath;

		bool threadwait;
#endif


	}
}

void ReadUnits(FILE *fp)
{
	for(int i=0; i<UNITS; i++)
	{
		Unit *u = &g_unit[i];

		fread(&u->on, sizeof(bool), 1, fp);

		if(!u->on)
			continue;

		fread(&u->type, sizeof(int), 1, fp);
#if 0
		fread(&u->stateowner, sizeof(int), 1, fp);
		fread(&u->corpowner, sizeof(int), 1, fp);
		fread(&u->unitowner, sizeof(int), 1, fp);
#else
		fread(&u->owner, sizeof(int), 1, fp);
#endif

		fread(&u->drawpos, sizeof(Vec3f), 1, fp);
		fread(&u->cmpos, sizeof(Vec2i), 1, fp);
		fread(&u->rotation, sizeof(Vec2f), 1, fp);

		int pathsz = 0;

		fread(&pathsz, sizeof(int), 1, fp);
		u->path.clear();

		for(int pathindex=0; pathindex<pathsz; pathindex++)
		{
			Vec2i waypoint;
			fread(&waypoint, sizeof(Vec2i), 1, fp);
			u->path.push_back(waypoint);
		}

		fread(&u->goal, sizeof(Vec2i), 1, fp);

		fread(&u->target, sizeof(int), 1, fp);
		fread(&u->target2, sizeof(int), 1, fp);
		fread(&u->targetu, sizeof(bool), 1, fp);
		fread(&u->underorder, sizeof(bool), 1, fp);
		fread(&u->fuelstation, sizeof(int), 1, fp);
		fread(u->belongings, sizeof(int), RESOURCES, fp);
		fread(&u->hp, sizeof(int), 1, fp);
		fread(&u->passive, sizeof(bool), 1, fp);
		fread(&u->prevpos, sizeof(Vec2i), 1, fp);
		fread(&u->taskframe, sizeof(int), 1, fp);
		fread(&u->pathblocked, sizeof(bool), 1, fp);
		fread(&u->frameslookjobago, sizeof(int), 1, fp);
		fread(&u->supplier, sizeof(int), 1, fp);
		fread(&u->reqamt, sizeof(int), 1, fp);
		fread(&u->targtype, sizeof(int), 1, fp);
		fread(&u->home, sizeof(int), 1, fp);
		fread(&u->car, sizeof(int), 1, fp);
		fread(&u->frame, sizeof(float), 2, fp);
		fread(&u->subgoal, sizeof(Vec2i), 1, fp);
	}
}

void SaveBuildings(FILE *fp)
{
	for(int i=0; i<BUILDINGS; i++)
	{
#if 0
		bool on;
		int type;
		int stateowner;
		int corpowner;
		int unitowner;

		Vec2i tilepos;	//position in tiles
		Vec3f drawpos;	//drawing position in centimeters

		bool finished;

		int pownetw;
		int crpipenetw;
		std::list<int> roadnetw;
#endif

		Building *b = &g_building[i];

		fwrite(&b->on, sizeof(bool), 1, fp);

		if(!b->on)
			continue;

		fwrite(&b->type, sizeof(int), 1, fp);
#if 0
		fwrite(&b->stateowner, sizeof(int), 1, fp);
		fwrite(&b->corpowner, sizeof(int), 1, fp);
		fwrite(&b->unitowner, sizeof(int), 1, fp);
#else
		fwrite(&b->owner, sizeof(int), 1, fp);
#endif

		fwrite(&b->tilepos, sizeof(Vec2i), 1, fp);
		fwrite(&b->drawpos, sizeof(Vec3f), 1, fp);

		fwrite(&b->finished, sizeof(bool), 1, fp);

		fwrite(&b->pownetw, sizeof(short), 1, fp);
		fwrite(&b->crpipenetw, sizeof(short), 1, fp);
		int nroadnetw = b->roadnetw.size();
		fwrite(&nroadnetw, sizeof(short), 1, fp);
		for(auto roadnetwiter = b->roadnetw.begin(); roadnetwiter != b->roadnetw.end(); roadnetwiter++)
			fwrite(&*roadnetwiter, sizeof(short), 1, fp);
	}
}

void ReadBuildings(FILE *fp)
{

	for(int i=0; i<BUILDINGS; i++)
	{
		Building *b = &g_building[i];

		fread(&b->on, sizeof(bool), 1, fp);

		if(!b->on)
			continue;

		fread(&b->type, sizeof(int), 1, fp);
#if 0
		fread(&b->stateowner, sizeof(int), 1, fp);
		fread(&b->corpowner, sizeof(int), 1, fp);
		fread(&b->unitowner, sizeof(int), 1, fp);
#else
		fread(&b->owner, sizeof(int), 1, fp);
#endif

		fread(&b->tilepos, sizeof(Vec2i), 1, fp);
		fread(&b->drawpos, sizeof(Vec3f), 1, fp);

		fread(&b->finished, sizeof(bool), 1, fp);

		fread(&b->pownetw, sizeof(short), 1, fp);
		fread(&b->crpipenetw, sizeof(short), 1, fp);
		short nroadnetw = -1;
		fread(&nroadnetw, sizeof(short), 1, fp);
		for(int i=0; i<nroadnetw; i++)
		{
			short roadnetw = -1;
			fread(&roadnetw, sizeof(short), 1, fp);
			b->roadnetw.push_back(roadnetw);
		}

		b->fillcollider();
	}
}

void SaveZoomCam(FILE *fp)
{
	Player* py = &g_player[g_curP];
	fwrite(&py->camera, sizeof(Camera), 1, fp);
	fwrite(&py->zoom, sizeof(float), 1, fp);
}

void ReadZoomCam(FILE *fp)
{
	Player* py = &g_player[g_curP];
	fread(&py->camera, sizeof(Camera), 1, fp);
	fread(&py->zoom, sizeof(float), 1, fp);
}

void SaveCo(FILE* fp)
{
	for(char ctype=0; ctype<CONDUIT_TYPES; ctype++)
	{
		ConduitType* ct = &g_cotype[ctype];

		for(int i=0; i<g_hmap.m_widthx*g_hmap.m_widthz; i++)
		{
			ConduitTile* ctile = &ct->cotiles[(int)false][i];

			fwrite(&ctile->on, sizeof(bool), 1, fp);

			if(!ctile->on)
				continue;

			fwrite(&ctile->conntype, sizeof(char), 1, fp);
			fwrite(&ctile->finished, sizeof(bool), 1, fp);
			fwrite(&ctile->owner, sizeof(char), 1, fp);
			fwrite(ctile->conmat, sizeof(int), RESOURCES, fp);
			fwrite(&ctile->netw, sizeof(short), 1, fp);
			fwrite(ctile->transporter, sizeof(short), RESOURCES, fp);
			fwrite(&ctile->drawpos, sizeof(Vec3f), 1, fp);
		}
	}
}

void ReadCo(FILE* fp)
{
	for(char ctype=0; ctype<CONDUIT_TYPES; ctype++)
	{
		ConduitType* ct = &g_cotype[ctype];

		for(int i=0; i<g_hmap.m_widthx*g_hmap.m_widthz; i++)
		{
			ConduitTile* ctile = &ct->cotiles[(int)false][i];

			fread(&ctile->on, sizeof(bool), 1, fp);

			if(!ctile->on)
				continue;

			fread(&ctile->conntype, sizeof(char), 1, fp);
			fread(&ctile->finished, sizeof(bool), 1, fp);
			fread(&ctile->owner, sizeof(char), 1, fp);
			fread(ctile->conmat, sizeof(int), RESOURCES, fp);
			fread(&ctile->netw, sizeof(short), 1, fp);
			fread(ctile->transporter, sizeof(short), RESOURCES, fp);
			fread(&ctile->drawpos, sizeof(Vec3f), 1, fp);
		}

		for(int x=0; x<g_hmap.m_widthx; x++)
			for(int z=0; z<g_hmap.m_widthz; z++)
			{
				if(!GetCo(ctype, x, z, false)->on)
					continue;

				RemeshCo(ctype, x, z, false);
			}
	}
}

bool SaveMap(const char* fullpath)
{
	FILE *fp = NULL;

	fp = fopen(fullpath, "wb");

	if(!fp)
		return false;

	char tag[] = MAP_TAG;
	int version = MAP_VERSION;

	fwrite(&tag, sizeof(tag), 1, fp);
	fwrite(&version, sizeof(version), 1, fp);

	SaveZoomCam(fp);
	SaveHeightmap(fp);
	SaveDeposits(fp);
	SaveFoliage(fp);
	SaveBuildings(fp);
	SaveUnits(fp);
	SaveCo(fp);

	fclose(fp);

	return true;
}

bool LoadMap(const char* fullpath)
{
	FreeMap();

	FILE *fp = NULL;

	fp = fopen(fullpath, "rb");

	if(!fp)
		return false;

	char realtag[] = MAP_TAG;
	int version = MAP_VERSION;
	char tag[ sizeof(realtag) ];

	fread(&tag, sizeof(tag), 1, fp);

	if(memcmp(tag, realtag, sizeof(tag)) != 0)
	{
		ErrorMessage("Error", "Incorrect header tag in map file.");
		return false;
	}

	fread(&version, sizeof(version), 1, fp);

	if(version != MAP_VERSION)
	{
		char msg[128];
		sprintf(msg, "Map file version (%i) doesn't match %i.", version, MAP_VERSION);
		ErrorMessage("Error", msg);
		return false;
	}

	ReadZoomCam(fp);
	ReadHeightmap(fp);
	ReadDeposits(fp);
	ReadFoliage(fp);
	ReadBuildings(fp);
	ReadUnits(fp);
	ReadCo(fp);

	FillColliderGrid();

	fclose(fp);

	return true;
}
