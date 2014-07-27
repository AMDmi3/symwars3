#include "sim.h"
#include "../script/console.h"
#include "infrastructure.h"
#include "road.h"
#include "building.h"

long long g_simframe = 0;

// Queue all the game resources and define objects
void Queue()
{
	// Cursor types

#if 0
#define CU_NONE		0	//cursor off?
#define CU_DEFAULT	1
#define CU_MOVE		2	//move window
#define CU_RESZL	3	//resize width (horizontal) from left side
#define CU_RESZR	4	//resize width (horizontal) from right side
#define CU_RESZT	5	//resize height (vertical) from top side
#define CU_RESZB	6	//resize height (vertical) from bottom side
#define CU_RESZTL	7	//resize top left corner
#define CU_RESZTR	8	//resize top right corner
#define CU_RESZBL	9	//resize bottom left corner
#define CU_RESZBR	10	//resize bottom right corner
#define CU_WAIT		11	//shows a hourglass?
#define CU_DRAG		12	//drag some object between widgets?
#define CU_STATES	13
#endif

	DefS("gui/transp.png", &g_cursor[CU_NONE], 0, 0);
	DefS("gui/cursors/default.png", &g_cursor[CU_DEFAULT], 0, 0);
	DefS("gui/cursors/move.png", &g_cursor[CU_MOVE], 16, 16);
	DefS("gui/cursors/reszh.png", &g_cursor[CU_RESZL], 16, 16);
	DefS("gui/cursors/reszh.png", &g_cursor[CU_RESZR], 16, 16);
	DefS("gui/cursors/reszv.png", &g_cursor[CU_RESZT], 16, 16);
	DefS("gui/cursors/reszv.png", &g_cursor[CU_RESZB], 16, 16);
	DefS("gui/cursors/reszd2.png", &g_cursor[CU_RESZTL], 16, 16);
	DefS("gui/cursors/reszd1.png", &g_cursor[CU_RESZTR], 16, 16);
	DefS("gui/cursors/reszd1.png", &g_cursor[CU_RESZBL], 16, 16);
	DefS("gui/cursors/reszd2.png", &g_cursor[CU_RESZBR], 16, 16);
	DefS("gui/cursors/default.png", &g_cursor[CU_WAIT], 16, 16);
	DefS("gui/cursors/default.png", &g_cursor[CU_DRAG], 16, 16);


	// Icons

#if 0
#define ICON_DOLLARS		0
#define ICON_PESOS			1
#define ICON_EUROS			2
#define ICON_POUNDS			3
#define ICON_FRANCS			4
#define ICON_YENS			5
#define ICON_RUPEES			6
#define ICON_ROUBLES		7
#define ICON_LABOUR			8
#define ICON_HOUSING		9
#define ICON_FARMPRODUCT	10
#define ICON_FOOD			11
#define ICON_CHEMICALS		12
#define ICON_ELECTRONICS	13
#define ICON_RESEARCH		14
#define ICON_PRODUCTION		15
#define ICON_IRONORE		16
#define ICON_URANIUMORE		17
#define ICON_STEEL			18
#define ICON_CRUDEOIL		19
#define ICON_WSFUEL			20
#define ICON_STONE			21
#define ICON_CEMENT			22
#define ICON_ENERGY			23
#define ICON_ENRICHEDURAN	24
#define ICON_COAL			25
#define ICON_TIME			26
#define ICON_RETFUEL		27
#define ICON_LOGS			28
#define ICON_LUMBER			29
#define ICON_WATER			30
#define ICONS				31
#endif

	DefI(ICON_DOLLARS, "gui/icons/dollars.png", ":dollar:");
	DefI(ICON_PESOS, "gui/icons/pesos.png", ":peso:");
	DefI(ICON_EUROS, "gui/icons/euros.png", ":euro:");
	DefI(ICON_POUNDS, "gui/icons/pounds.png", ":pound:");
	DefI(ICON_FRANCS, "gui/icons/francs.png", ":franc:");
	DefI(ICON_YENS, "gui/icons/yens.png", ":yen:");
	DefI(ICON_RUPEES, "gui/icons/rupees.png", ":rupee:");
	DefI(ICON_ROUBLES, "gui/icons/roubles.png", ":rouble:");
	DefI(ICON_LABOUR, "gui/icons/labour.png", ":labour:");
	DefI(ICON_HOUSING, "gui/icons/housing.png", ":housing:");
	DefI(ICON_FARMPRODUCT, "gui/icons/farmproducts.png", ":farmprod:");
	DefI(ICON_WSFOOD, "gui/icons/wsfood.png", ":wsfood:");
	DefI(ICON_RETFOOD, "gui/icons/retfood.png", ":retfood:");
	DefI(ICON_CHEMICALS, "gui/icons/chemicals.png", ":chemicals:");
	DefI(ICON_ELECTRONICS, "gui/icons/electronics.png", ":electronics:");
	DefI(ICON_RESEARCH, "gui/icons/research.png", ":research:");
	DefI(ICON_PRODUCTION, "gui/icons/production.png", ":production:");
	DefI(ICON_IRONORE, "gui/icons/ironore.png", ":ironore:");
	DefI(ICON_URANIUMORE, "gui/icons/uraniumore.png", ":uraniumore:");
	DefI(ICON_STEEL, "gui/icons/steel.png", ":steel:");
	DefI(ICON_CRUDEOIL, "gui/icons/crudeoil.png", ":crudeoil:");
	DefI(ICON_WSFUEL, "gui/icons/fuelwholesale.png", ":wsfuel:");
	DefI(ICON_STONE, "gui/icons/stone.png", ":stone:");
	DefI(ICON_CEMENT, "gui/icons/cement.png", ":cement:");
	DefI(ICON_ENERGY, "gui/icons/energy.png", ":energy:");
	DefI(ICON_ENRICHEDURAN, "gui/icons/uranium.png", ":enricheduran:");
	DefI(ICON_COAL, "gui/icons/coal.png", ":coal:");
	DefI(ICON_TIME, "gui/icons/time.png", ":time:");
	DefI(ICON_RETFUEL, "gui/icons/fuelretail.png", ":retfuel:");
	DefI(ICON_LOGS, "gui/icons/logs.png", ":logs:");
	DefI(ICON_LUMBER, "gui/icons/lumber.png", ":lumber:");
	DefI(ICON_WATER, "gui/icons/water.png", ":water:");
	DefI(ICON_EXCLAMATION, "gui/icons/exclamation.png", ":exclam:");


	// Resource types

#if 0
#define RES_NONE			-1
#define RES_FUNDS			0
#define RES_LABOUR			1
#define RES_HOUSING			2
#define RES_FARMPRODUCTS	3
#define RES_RETFOOD			4
#define RES_PRODUCTION		5
#define RES_MINERALS		6
#define RES_CRUDEOIL		7
#define RES_WSFUEL			8
#define RES_RETFUEL			9
#define RES_ENERGY			10
#define RES_URANIUM			11
#define RESOURCES			12
#endif

#if 0
	void DefR(int resi, const char* n, const char* depn, int iconindex, bool phys, bool cap, bool glob, float r, float g, float b, float a)
#endif

	DefR(RES_FUNDS,			"Funds",				"",							ICON_DOLLARS,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_NONE);
	DefR(RES_LABOUR,		"Labour",				"",							ICON_LABOUR,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_NONE);
	DefR(RES_HOUSING,		"Housing",				"",							ICON_HOUSING,		true,	true,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_FARMPRODUCTS,	"Farm Products",		"Fertile",					ICON_FARMPRODUCT,	true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_PRODUCTION,	"Production",			"",							ICON_PRODUCTION,	true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_RETFOOD,		"Retail Food",			"",							ICON_RETFOOD,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_MINERALS,		"Minerals",				"Iron Ore Deposit",			ICON_IRONORE,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_CRUDEOIL,		"Crude Oil",			"Oil Deposit",				ICON_CRUDEOIL,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_CRPIPE);
	DefR(RES_WSFUEL,		"Wholesale Fuel",		"",							ICON_WSFUEL,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_RETFUEL,		"Retail Fuel",			"",							ICON_RETFUEL,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_ENERGY,		"Energy",				"",							ICON_ENERGY,		false,	true,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_POWL);
	DefR(RES_URANIUM,		"Uranium",				"",							ICON_ENRICHEDURAN,	true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);


	// Various environment textures
#if 0
	QueueTexture(&g_tiletexs[TILE_SAND], "textures/terrain/default/sand.jpg", false, false);
	QueueTexture(&g_tiletexs[TILE_GRASS], "textures/terrain/default/grass.png", false, false);
	//QueueTexture(&g_tiletexs[TILE_ROCK], "textures/terrain/default/rock.png", false, false);
	QueueTexture(&g_tiletexs[TILE_ROCK], "textures/terrain/default/rock.jpg", false, true);
	QueueTexture(&g_tiletexs[TILE_ROCK_NORM], "textures/terrain/default/rock.norm.jpg", false, false);
	QueueTexture(&g_tiletexs[TILE_CRACKEDROCK], "textures/terrain/default/crackedrock.jpg", false, false);
	QueueTexture(&g_tiletexs[TILE_CRACKEDROCK_NORM], "textures/terrain/default/crackedrock.norm.jpg", false, false);

	QueueTexture(&g_rimtexs[TEX_DIFF], "textures/terrain/default/underground.jpg", false, false);
	QueueTexture(&g_rimtexs[TEX_SPEC], "textures/terrain/default/underground.spec.jpg", false, false);
	QueueTexture(&g_rimtexs[TEX_NORM], "textures/terrain/default/underground.norm.jpg", false, false);

	//QueueTexture(&g_water, "textures/terrain/default/water.png", false);
	QueueTexture(&g_watertex[WATER_TEX_GRADIENT], "textures/terrain/default/water.gradient.png", false, false);
	QueueTexture(&g_watertex[WATER_TEX_DETAIL], "textures/terrain/default/water.detail.jpg", false, false);
	//QueueTexture(&g_watertex[WATER_TEX_DETAIL], "textures/terrain/default/water2.png", false, true);
	QueueTexture(&g_watertex[WATER_TEX_SPECULAR], "textures/terrain/default/water.spec.jpg", false, false);
	//QueueTexture(&g_watertex[WATER_TEX_NORMAL], "textures/terrain/default/water.norm.jpg", false, true);
	QueueTexture(&g_watertex[WATER_TEX_NORMAL], "textures/terrain/default/water5.norm.jpg", false, false);
#endif
	QueueTexture(&g_circle, "gui/circle.png", true, true);

#if 0
	LoadParticles();
	LoadSkyBox("textures/terrain/default/skydome");
#endif

	// Players

#if 1
	QueueModel(&g_playerm, "models/brain/brain.ms3d", Vec3f(50, 50, 50), Vec3f(0,0,0), true);

	for(int i=0; i<PLAYERS; i++)
	{
		Player* p = &g_player[i];
		PlayerColor* pyc = &g_pycols[i];

		char name[64];
		sprintf(name, "Player %d (%s)", i, pyc->name);
		
		DefP(i, pyc->color[0]/255.0f, pyc->color[1]/255.0f, pyc->color[2]/255.0f, 1, RichText(name));

		SubmitConsole(&p->name);
	}
#endif

	// Unit types
#if 0
	DefU(UNIT_ROBOSOLDIER, "models/battlecomp2011simp/battlecomp.ms3d", Vec3f(1,1,1)*182.0f/70.0f, Vec3f(0,0,0)*182.0f/70.0f, Vec3i(125, 250, 125), "Robot Soldier", 100, true, true, false, false, false, 6, true);
	DefU(UNIT_LABOURER, "models/labourer/labourer.ms3d", Vec3f(1,1,1)*182.0f/70.0f, Vec3f(0,0,0)*182.0f/70.0f, Vec3i(125, 250, 125), "Labourer", 100, true, true, false, false, false, 6, false);
	DefU(UNIT_TRUCK, "models/truck/truck.ms3d", Vec3f(1,1,1)*30.0f, Vec3f(0,0,0), Vec3i(125, 250, 125), "Truck", 100, true, false, true, false, false, 30, false);
#endif

	// Foliage types
#if 0
	DefF(FOLIAGE_TREE1, "models/pine/pine.ms3d", Vec3f(200,200,200), Vec3f(0,0,0), Vec3i(40, 60, 500)*20);
	DefF(FOLIAGE_TREE2, "models/pine/pine.ms3d", Vec3f(200,200,200), Vec3f(0,0,0), Vec3i(40, 60, 500)*20);
	DefF(FOLIAGE_TREE3, "models/pine/pine.ms3d", Vec3f(200,200,200), Vec3f(0,0,0), Vec3i(40, 60, 500)*20);
#endif

	// Building types
	
#if 1
	DefBillb(BILLB_BL, 23478);
	DefBillb(BILLB_LAB, 936);
	DefBillb(BILLB_MER, 934);
#else
	DefBillb(BILLB_BL, 'B');
	DefBillb(BILLB_LAB, 'l');
	DefBillb(BILLB_MER, 'm');
#endif

#if 0
	//DefB(BUILDING_APARTMENT, "Apartment Building", Vec2i(2,1), "models/apartment1/basebuilding.ms3d", Vec3f(100,100,100), Vec3f(0,0,0), "models/apartment1/basebuilding.ms3d", Vec3f(100,100,100), Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	DefB(BUILDING_APARTMENT, "Apartment Building", Vec2i(2,2),  false, "models/apartment2/b1911", Vec3f(1,1,1), Vec3f(0,0,0), "models/apartment2/b1911", Vec3f(1,1,1), Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	BConMat(BUILDING_APARTMENT, RES_MINERALS, 5);
	BConMat(BUILDING_APARTMENT, RES_LABOUR, 10);
	BInput(BUILDING_APARTMENT, RES_ENERGY, 5);
	BOutput(BUILDING_APARTMENT, RES_HOUSING, 15);

	DefB(BUILDING_FACTORY, "Factory", Vec2i(1,1),  false, "models/factory3/factory3", Vec3f(1,1,1)/2, Vec3f(0,0,0), "models/factory3/factory3", Vec3f(1,1,1)/2, Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	BConMat(BUILDING_FACTORY, RES_MINERALS, 5);
	BConMat(BUILDING_FACTORY, RES_LABOUR, 10);
	BInput(BUILDING_FACTORY, RES_ENERGY, 5);
	BOutput(BUILDING_FACTORY, RES_PRODUCTION, 5);

	DefB(BUILDING_REFINERY, "Refinery", Vec2i(2,2),  false, "models/refinery2/refinery2", Vec3f(1,1,1), Vec3f(0,0,0), "models/refinery2/refinery2", Vec3f(1,1,1), Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	BConMat(BUILDING_REFINERY, RES_MINERALS, 5);
	BConMat(BUILDING_REFINERY, RES_LABOUR, 10);
	BInput(BUILDING_REFINERY, RES_ENERGY, 5);
	BInput(BUILDING_REFINERY, RES_CRUDEOIL, 5);
	BOutput(BUILDING_REFINERY, RES_WSFUEL, 5);
	BEmitter(BUILDING_REFINERY, 0, PARTICLE_EXHAUST, Vec3f(TILE_SIZE*5.7/10, TILE_SIZE*3/2, TILE_SIZE*-5/10));
	BEmitter(BUILDING_REFINERY, 1, PARTICLE_EXHAUST2, Vec3f(TILE_SIZE*5.7/10, TILE_SIZE*3/2, TILE_SIZE*-5/10));
	BEmitter(BUILDING_REFINERY, 2, PARTICLE_EXHAUST, Vec3f(TILE_SIZE*-4.5/10, TILE_SIZE*1.75, TILE_SIZE*3.0f/10));
	BEmitter(BUILDING_REFINERY, 3, PARTICLE_EXHAUST2, Vec3f(TILE_SIZE*-4.5/10, TILE_SIZE*1.75, TILE_SIZE*3.0f/10));

	DefB(BUILDING_NUCPOW, "Nuclear Powerplant", Vec2i(2,2), false, "models/nucpow2/nucpow2", Vec3f(1,1,1), Vec3f(0,0,0), "models/nucpow2/nucpow2", Vec3f(1,1,1), Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	BConMat(BUILDING_NUCPOW, RES_MINERALS, 5);
	BConMat(BUILDING_NUCPOW, RES_LABOUR, 10);
	BInput(BUILDING_NUCPOW, RES_URANIUM, 5);
	BOutput(BUILDING_NUCPOW, RES_ENERGY, 200);
	BEmitter(BUILDING_NUCPOW, 0, PARTICLE_EXHAUSTBIG, Vec3f(TILE_SIZE*-0.63f, TILE_SIZE*1.5f, TILE_SIZE*0));
	BEmitter(BUILDING_NUCPOW, 1, PARTICLE_EXHAUSTBIG, Vec3f(TILE_SIZE*0.17f, TILE_SIZE*1.5f, TILE_SIZE*-0.64f));

	DefB(BUILDING_FARM, "Farm", Vec2i(4,2), true, "models/farm2/farm2", Vec3f(1,1,1), Vec3f(0,0,0), "models/farm2/farm2", Vec3f(1,1,1), Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	BConMat(BUILDING_FARM, RES_MINERALS, 5);
	BConMat(BUILDING_FARM, RES_LABOUR, 10);
	BInput(BUILDING_FARM, RES_ENERGY, 5);
	BOutput(BUILDING_FARM, RES_FARMPRODUCTS, 15);

	DefB(BUILDING_STORE, "Store", Vec2i(2,1), true, "models/store1/hugterr.ms3d", Vec3f(100,100,100), Vec3f(0,0,0), "models/store1/hugterr.ms3d", Vec3f(100,100,100), Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	BConMat(BUILDING_STORE, RES_MINERALS, 5);
	BConMat(BUILDING_STORE, RES_LABOUR, 10);
	BInput(BUILDING_STORE, RES_ENERGY, 5);
	BInput(BUILDING_STORE, RES_FARMPRODUCTS, 500);
	BInput(BUILDING_STORE, RES_PRODUCTION, 5);
	BOutput(BUILDING_STORE, RES_RETFOOD, 500);

	DefB(BUILDING_HARBOUR, "Harbour", Vec2i(2,2), false, "models/harbour2/harbour2", Vec3f(1,1,1), Vec3f(0,0,0), "models/harbour2/harbour2", Vec3f(1,1,1), Vec3f(0,0,0), FOUNDATION_COASTAL, RES_NONE);
	BConMat(BUILDING_HARBOUR, RES_MINERALS, 5);
	BConMat(BUILDING_HARBOUR, RES_LABOUR, 10);
	BInput(BUILDING_HARBOUR, RES_ENERGY, 5);

	DefB(BUILDING_OILWELL, "Oil Well", Vec2i(1,1), false, "models/oilwell2/oilwell2", Vec3f(1,1,1), Vec3f(0,0,0), "models/oilwell2/oilwell2", Vec3f(1,1,1), Vec3f(0,0,0), FOUNDATION_LAND, RES_CRUDEOIL);
	BConMat(BUILDING_OILWELL, RES_MINERALS, 5);
	BConMat(BUILDING_OILWELL, RES_LABOUR, 10);
	BInput(BUILDING_OILWELL, RES_ENERGY, 5);
	BOutput(BUILDING_OILWELL, RES_CRUDEOIL, 5);

	DefB(BUILDING_MINE, "Mine", Vec2i(1,1), false, "models/mine/nobottom.ms3d", Vec3f(1,1,1)/32.0f*TILE_SIZE, Vec3f(0,0,0), "models/mine/nobottom.ms3d", Vec3f(1,1,1)/32.0f*TILE_SIZE, Vec3f(0,0,0), FOUNDATION_LAND, -1);
	BConMat(BUILDING_MINE, RES_MINERALS, 5);
	BConMat(BUILDING_MINE, RES_LABOUR, 10);
	BInput(BUILDING_MINE, RES_ENERGY, 5);
	BOutput(BUILDING_MINE, RES_MINERALS, 100);
	BOutput(BUILDING_MINE, RES_URANIUM, 10);

	DefB(BUILDING_GASSTATION, "Gas Station", Vec2i(1,1), true, "models/gasstation2/gasstation2.ms3d", Vec3f(1,1,1)/80.0f*TILE_SIZE, Vec3f(0,0,0), "models/gasstation2/gasstation2.ms3d", Vec3f(1,1,1)/80.0f*TILE_SIZE, Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	BConMat(BUILDING_GASSTATION, RES_MINERALS, 5);
	BConMat(BUILDING_GASSTATION, RES_LABOUR, 10);
	BInput(BUILDING_GASSTATION, RES_ENERGY, 5);
	BOutput(BUILDING_GASSTATION, RES_RETFUEL, 5);
#endif

	// Conduit types
	
	Vec3f scale(TILE_SIZE/16.0f, TILE_SIZE/16.0f, TILE_SIZE/16.0f);
	Vec3f trans(0,0,0);

#if 0
	DefCo(CONDUIT_ROAD, offsetof(Building,roadnetw), offsetof(Selection,roads), ROAD_MAX_FOREW_INCLINE, ROAD_MAX_SIDEW_INCLINE, false, false, Vec2i(TILE_SIZE/2, TILE_SIZE/2), Vec3f(TILE_SIZE/2, 0, TILE_SIZE/2));
	CoConMat(CONDUIT_ROAD, RES_LABOUR, 1);
	CoConMat(CONDUIT_ROAD, RES_MINERALS, 1);
	DefConn(CONDUIT_ROAD, CONNECTION_NOCONNECTION, CONSTRUCTION, "models/road/1_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTH, CONSTRUCTION, "models/road/n_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EAST, CONSTRUCTION, "models/road/e_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_SOUTH, CONSTRUCTION, "models/road/s_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_WEST, CONSTRUCTION, "models/road/w_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEAST, CONSTRUCTION, "models/road/ne_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHSOUTH, CONSTRUCTION, "models/road/ns_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTSOUTH, CONSTRUCTION, "models/road/es_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHWEST, CONSTRUCTION, "models/road/nw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTWEST, CONSTRUCTION, "models/road/ew_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_SOUTHWEST, CONSTRUCTION, "models/road/sw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTSOUTHWEST, CONSTRUCTION, "models/road/esw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHSOUTHWEST, CONSTRUCTION, "models/road/nsw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTWEST, CONSTRUCTION, "models/road/new_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTSOUTH, CONSTRUCTION, "models/road/nes_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTSOUTHWEST, CONSTRUCTION, "models/road/nesw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NOCONNECTION, FINISHED, "models/road/1.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTH, FINISHED, "models/road/n.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EAST, FINISHED, "models/road/e.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_SOUTH, FINISHED, "models/road/s.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_WEST, FINISHED, "models/road/w.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEAST, FINISHED, "models/road/ne.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHSOUTH, FINISHED, "models/road/ns.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTSOUTH, FINISHED, "models/road/es.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHWEST, FINISHED, "models/road/nw.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTWEST, FINISHED, "models/road/ew.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_SOUTHWEST, FINISHED, "models/road/sw.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTSOUTHWEST, FINISHED, "models/road/esw.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHSOUTHWEST, FINISHED, "models/road/nsw.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTWEST, FINISHED, "models/road/new.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTSOUTH, FINISHED, "models/road/nes.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTSOUTHWEST, FINISHED, "models/road/nesw.ms3d", scale, trans);
	
	DefCo(CONDUIT_POWL, offsetof(Building,pownetw), offsetof(Selection,powls), TILE_SIZE*2, TILE_SIZE*2, true, true, Vec2i(0, 0), Vec3f(0, 0, 0));
	CoConMat(CONDUIT_POWL, RES_LABOUR, 1);
	CoConMat(CONDUIT_POWL, RES_MINERALS, 1);
	DefConn(CONDUIT_POWL, CONNECTION_NOCONNECTION, CONSTRUCTION, "models/powl/1_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTH, CONSTRUCTION, "models/powl/n_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EAST, CONSTRUCTION, "models/powl/e_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_SOUTH, CONSTRUCTION, "models/powl/s_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_WEST, CONSTRUCTION, "models/powl/w_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEAST, CONSTRUCTION, "models/powl/ne_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHSOUTH, CONSTRUCTION, "models/powl/ns_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTSOUTH, CONSTRUCTION, "models/powl/es_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHWEST, CONSTRUCTION, "models/powl/nw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTWEST, CONSTRUCTION, "models/powl/ew_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_SOUTHWEST, CONSTRUCTION, "models/powl/sw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTSOUTHWEST, CONSTRUCTION, "models/powl/esw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHSOUTHWEST, CONSTRUCTION, "models/powl/nsw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTWEST, CONSTRUCTION, "models/powl/new_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTSOUTH, CONSTRUCTION, "models/powl/nes_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTSOUTHWEST, CONSTRUCTION, "models/powl/nesw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NOCONNECTION, FINISHED, "models/powl/1.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTH, FINISHED, "models/powl/n.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EAST, FINISHED, "models/powl/e.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_SOUTH, FINISHED, "models/powl/s.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_WEST, FINISHED, "models/powl/w.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEAST, FINISHED, "models/powl/ne.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHSOUTH, FINISHED, "models/powl/ns.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTSOUTH, FINISHED, "models/powl/es.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHWEST, FINISHED, "models/powl/nw.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTWEST, FINISHED, "models/powl/ew.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_SOUTHWEST, FINISHED, "models/powl/sw.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTSOUTHWEST, FINISHED, "models/powl/esw.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHSOUTHWEST, FINISHED, "models/powl/nsw.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTWEST, FINISHED, "models/powl/new.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTSOUTH, FINISHED, "models/powl/nes.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTSOUTHWEST, FINISHED, "models/powl/nesw.ms3d", scale, trans);

	trans = Vec3f(-TILE_SIZE/2, 0, TILE_SIZE/2);
	DefCo(CONDUIT_CRPIPE, offsetof(Building,crpipenetw), offsetof(Selection,crpipes), TILE_SIZE*2, TILE_SIZE*2, true, true, Vec2i(0, 0), Vec3f(0, 0, 0));
	CoConMat(CONDUIT_CRPIPE, RES_LABOUR, 1);
	CoConMat(CONDUIT_CRPIPE, RES_MINERALS, 1);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NOCONNECTION, CONSTRUCTION, "models/crpipe/1_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTH, CONSTRUCTION, "models/crpipe/n_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EAST, CONSTRUCTION, "models/crpipe/e_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_SOUTH, CONSTRUCTION, "models/crpipe/s_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_WEST, CONSTRUCTION, "models/crpipe/w_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEAST, CONSTRUCTION, "models/crpipe/ne_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHSOUTH, CONSTRUCTION, "models/crpipe/ns_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTSOUTH, CONSTRUCTION, "models/crpipe/es_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHWEST, CONSTRUCTION, "models/crpipe/nw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTWEST, CONSTRUCTION, "models/crpipe/ew_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_SOUTHWEST, CONSTRUCTION, "models/crpipe/sw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTSOUTHWEST, CONSTRUCTION, "models/crpipe/esw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHSOUTHWEST, CONSTRUCTION, "models/crpipe/nsw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTWEST, CONSTRUCTION, "models/crpipe/new_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTSOUTH, CONSTRUCTION, "models/crpipe/nes_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTSOUTHWEST, CONSTRUCTION, "models/crpipe/nesw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NOCONNECTION, FINISHED, "models/crpipe/1.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTH, FINISHED, "models/crpipe/n.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EAST, FINISHED, "models/crpipe/e.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_SOUTH, FINISHED, "models/crpipe/s.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_WEST, FINISHED, "models/crpipe/w.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEAST, FINISHED, "models/crpipe/ne.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHSOUTH, FINISHED, "models/crpipe/ns.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTSOUTH, FINISHED, "models/crpipe/es.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHWEST, FINISHED, "models/crpipe/nw.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTWEST, FINISHED, "models/crpipe/ew.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_SOUTHWEST, FINISHED, "models/crpipe/sw.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTSOUTHWEST, FINISHED, "models/crpipe/esw.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHSOUTHWEST, FINISHED, "models/crpipe/nsw.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTWEST, FINISHED, "models/crpipe/new.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTSOUTH, FINISHED, "models/crpipe/nes.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTSOUTHWEST, FINISHED, "models/crpipe/nesw.ms3d", scale, trans);
#endif

	// Sounds

	g_ordersnd.clear();
	g_ordersnd.push_back(Sound("sounds/aaa000/gogogo.wav"));
	g_ordersnd.push_back(Sound("sounds/aaa000/moveout2.wav"));
	g_ordersnd.push_back(Sound("sounds/aaa000/spreadout.wav"));
	g_ordersnd.push_back(Sound("sounds/aaa000/wereunderattack3.wav"));
	//g_zpainSnd.push_back(Sound("sounds/zpain.wav"));
}
