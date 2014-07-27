#include "../gmain.h"
#include "../../common/gui/gui.h"
#include "../keymap.h"
#include "../../common/render/heightmap.h"
#include "../../common/math/hmapmath.h"
#include "../../common/math/camera.h"
#include "../../common/render/shadow.h"
#include "../../common/render/screenshot.h"
#include "../../common/save/savemap.h"
#include "editorgui.h"
#include "playgui.h"
#include "../../common/sim/unit.h"
#include "../../common/sim/unittype.h"
#include "../../common/sim/building.h"
#include "../../common/sim/buildingtype.h"
#include "../../common/sim/selection.h"
#include "../../common/sim/road.h"
#include "../../common/sim/powl.h"
#include "../../common/sim/crpipe.h"
#include "../../common/render/water.h"
#include "../../common/sim/order.h"
#include "../../common/path/pathdebug.h"
#include "../../common/path/pathnode.h"
#include "../../common/sim/build.h"
#include "../../common/sim/player.h"
#include "../../common/gui/widgets/windoww.h"
#include "../../common/debug.h"
#include "../../common/script/console.h"

//bool g_canselect = true;

char g_lastsave[MAX_PATH+1];


#if 0
void Change_Fullscreen()
{
	int selected = gui->get("settings")->get("fullscreen", DROPDOWN)->selected;

	if(g_fullscreen == (bool)selected)
		return;

	DestroyWindow();
	g_fullscreen = selected;
	WriteConfig();
	MakeWindow();
	Reload();
	RedoGUI();
}

void Change_Resolution()
{
	int selected = gui->get("settings")->get("resolution", DROPDOWN)->selected;

	if(g_selectedRes.width == g_resolution[selected].width && g_selectedRes.height == g_resolution[selected].height)
		return;

	g_selectedRes = g_resolution[selected];
	WriteConfig();

	if(g_fullscreen)
	{
		DestroyWindow();
		MakeWindow();
		Reload();
		RedoGUI();
	}
	else
	{
		DWORD dwExStyle;
		DWORD dwStyle;
		RECT WindowRect;
		WindowRect.left=(long)0;
		WindowRect.right=(long)g_selectedRes.width;
		WindowRect.top=(long)0;
		WindowRect.bottom=(long)g_selectedRes.height;

		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
		//startx = CW_USEDEFAULT;
		//starty = CW_USEDEFAULT
		int startx = GetSystemMetrics(SM_CXSCREEN)/2 - g_selectedRes.width/2;
		int starty = GetSystemMetrics(SM_CYSCREEN)/2 - g_selectedRes.height/2;

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);
		MoveWindow(g_hWnd, startx, starty, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, false);

		//int startx = GetSystemMetrics(SM_CXSCREEN)/2 - g_selectedRes.width/2;
		//int starty = GetSystemMetrics(SM_CYSCREEN)/2 - g_selectedRes.height/2;

		//MoveWindow(g_hWnd, startx, starty, g_selectedRes.width, g_selectedRes.height, false);
	}
}

void Change_BPP()
{
	int selected = gui->get("settings")->get("bpp", DROPDOWN)->selected;

	if(py->bpp == g_bpps[selected])
		return;

	py->bpp = g_bpps[selected];
	WriteConfig();
	DestroyWindow();
	MakeWindow();
	Reload();
	RedoGUI();
}

void Click_BuildNum(int param)
{
	if(param == NOTHING && g_mode != APPMODE_EDITOR)
		g_canselect = true;

	//if(g_mode != APPMODE_EDITOR)
	gui->close("build selector");

	py->build = param;

	if(g_mode == APPMODE_EDITOR)
		g_edTool = EDTOOL::NOTOOL;
}

void Click_OutBuild()
{
	CView* v = gui->get("build selector");
	/*
	Widget* sw = &v->widget[0];
	sw->ldown = false;

	Widget* w;

	for(int i=0; i<sw->subwidg.size(); i++)
	{
		w = &sw->subwidg[i];
		w->ldown = false;
	}
	*/
	g_canselect = true;

	gui->close("build selector");
}

void Reload()
{
	g_mode = APPMODE_RELOADING;
	g_reStage = 0;
	g_lastLTex = -1;

	LoadTiles();
	LoadParticles();
	LoadProjectiles();
	LoadTerrainTextures();
	LoadHoverTex();
	LoadUnitSprites();
	BSprites();

	for(int i=0; i<MODELS; i++)
	{
		if(g_model[i].on)
			g_model[i].ReloadTexture();
	}
}
#endif

void Click_LoadMapButton()
{
#ifdef PLATFORM_WIN
	OPENFILENAME ofn;

	char filepath[MAX_PATH+1];

	ZeroMemory( &ofn , sizeof( ofn));

	char initdir[MAX_PATH+1];
	FullPath("map projects\\", initdir);
	CorrectSlashes(initdir);
	//strcpy(filepath, initdir);
	FullPath("map projects/map project", filepath);
	CorrectSlashes(filepath);

	ofn.lStructSize     = sizeof ( ofn );
	ofn.hwndOwner       = NULL;
	ofn.lpstrInitialDir = initdir;
	ofn.lpstrFile       = filepath;
	//ofn.lpstrFile[0]  = '\0';
	ofn.nMaxFile        = sizeof( filepath );
	//ofn.lpstrFilter   = "Save\0*.map\0All\0*.*\0";
	ofn.lpstrFilter     = "All\0*.*\0";
	ofn.nFilterIndex    = 1;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = MAX_PATH;	//0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags           = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;

	if(!GetOpenFileName(&ofn))
		return;

	//CorrectSlashes(filepath);
	FreeMap();

	if(LoadMap(filepath))
		strcpy(g_lastsave, filepath);
#endif //PLATFORM_WIN
}

void Click_SaveMapButton()
{
#ifdef PLATFORM_WIN
	OPENFILENAME ofn;

	char filepath[MAX_PATH+1];

	ZeroMemory( &ofn , sizeof( ofn));

	char initdir[MAX_PATH+1];
	FullPath("maps\\", initdir);
	CorrectSlashes(initdir);
	//strcpy(filepath, initdir);
	FullPath("maps/map", filepath);
	CorrectSlashes(filepath);

	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL  ;
	ofn.lpstrInitialDir = initdir;
	ofn.lpstrFile = filepath;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( filepath );
	//ofn.lpstrFilter = "Save\0*.map\0All\0*.*\0";
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex =1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;	//0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if(!GetSaveFileName(&ofn))
		return;

	//CorrectSlashes(filepath);
	SaveMap(filepath);
	strcpy(g_lastsave, filepath);
#endif //PLATFORM_WIN
}

void Click_QSaveMapButton()
{
	if(g_lastsave[0] == '\0')
	{
		Click_SaveMapButton();
		return;
	}

	SaveMap(g_lastsave);
}

void Resize_NewGameLink(Widget* thisw)
{
}

void Resize_LoadingStatus(Widget* thisw)
{
	Player* py = &g_player[g_curP];

	thisw->m_pos[0] = py->width/2;
	thisw->m_pos[1] = py->height/2;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = py->height;
	thisw->m_tpos[0] = py->width/2;
	thisw->m_tpos[1] = py->height/2;
}

void Resize_WinText(Widget* thisw)
{
	Widget* parent = thisw->m_parent;

	thisw->m_pos[0] = parent->m_pos[0];
	thisw->m_pos[1] = parent->m_pos[1];
	thisw->m_pos[2] = thisw->m_pos[0] + 400;
	thisw->m_pos[3] = thisw->m_pos[1] + 2000;
}

void Click_NewGame()
{
	CheckGLError(__FILE__, __LINE__);
#if 1
	//LoadJPGMap("heightmaps/heightmap0e2s.jpg");
	//LoadJPGMap("heightmaps/heightmap0e2.jpg");
	LoadJPGMap("heightmaps/heightmap0e.jpg");
#elif 1
	LoadJPGMap("heightmaps/heightmap0c.jpg");
#else
	char fullpath[MAX_PATH+1];
	FullPath("maps/testmap", fullpath);
	//FullPath("maps/bigmap256", fullpath);
	LoadMap(fullpath);
#endif

	CheckGLError(__FILE__, __LINE__);
#if 1
	for(int i=0; i<10; i++)
		for(int j=0; j<10; j++)
		{

			//Vec3i cmpos((g_hmap.m_widthx+4)*TILE_SIZE/2 + (i+2)*PATHNODE_SIZE, 0, g_hmap.m_widthz*TILE_SIZE/2 + (j+2)*PATHNODE_SIZE);
			//cmpos.y = g_hmap.accheight(cmpos.x, cmpos.z);
			Vec2i cmpos((g_hmap.m_widthx+4)*TILE_SIZE/2 + (i+2)*PATHNODE_SIZE, g_hmap.m_widthz*TILE_SIZE/2 + (j+2)*PATHNODE_SIZE);

			//if(rand()%2 == 1)
			//	PlaceUnit(UNIT_ROBOSOLDIER, cmpos, 0);
			//else
			//	PlaceUnit(UNIT_LABOURER, cmpos, -1, NULL);

			Vec3f fpos = Vec3f(cmpos.x, g_hmap.accheight(cmpos.x, cmpos.y), cmpos.y);
			
			if(rand()%2 == 1)
				PlaceBillboard(BILLB_LAB, fpos, 5, -1);
			else
				PlaceBillboard(BILLB_MER, fpos, 5, -1);
		}
#endif
	CheckGLError(__FILE__, __LINE__);

	Vec2i cmpos((g_hmap.m_widthx+1)/2*TILE_SIZE+TILE_SIZE/2, g_hmap.m_widthz/2*TILE_SIZE+TILE_SIZE/2);

	//if(rand()%2 == 1)
	//	PlaceUnit(UNIT_ROBOSOLDIER, cmpos, 0);
	//else
	//	PlaceUnit(UNIT_LABOURER, cmpos, -1, NULL);

	Vec3f fpos = Vec3f(cmpos.x, g_hmap.accheight(cmpos.x, cmpos.y), cmpos.y);

	PlaceBillboard(BILLB_BL, fpos, 30, -1);

	for(int i=0; i<PLAYERS; i++)
	{
		if(i == g_localP)
			continue;

		Player* p = &g_player[i];

		p->on = true;
		p->ai = true;

		p->global[RES_FUNDS] = 4000;
		p->global[RES_FARMPRODUCTS] = 4000;
		p->global[RES_RETFOOD] = 4000;
		p->global[RES_MINERALS] = 4000;
		p->global[RES_WSFUEL] = 4000;
		p->global[RES_URANIUM] = 4000;

#if 0	
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
	}

#if 0
	PlaceBuilding(BUILDING_HARBOUR, Vec2i(g_hmap.m_widthx/2-1, g_hmap.m_widthz/2-3), true, 0);
	PlaceBuilding(BUILDING_APARTMENT, Vec2i(g_hmap.m_widthx/2+2, g_hmap.m_widthz/2-2), true, 0);
	PlaceBuilding(BUILDING_APARTMENT, Vec2i(g_hmap.m_widthx/2+4, g_hmap.m_widthz/2-3), true, 0);
	PlaceBuilding(BUILDING_APARTMENT, Vec2i(g_hmap.m_widthx/2+6, g_hmap.m_widthz/2-3), true, 0);
	PlaceRoad(g_hmap.m_widthx/2+1, g_hmap.m_widthz/2-1, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+2, g_hmap.m_widthz/2-1, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+3, g_hmap.m_widthz/2-1, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+3, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+4, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+5, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+6, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+7, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+7, g_hmap.m_widthz/2-3, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+7, g_hmap.m_widthz/2-4, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+7, g_hmap.m_widthz/2-5, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+7, g_hmap.m_widthz/2-6, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+7, g_hmap.m_widthz/2-7, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+8, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+9, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+10, g_hmap.m_widthz/2-2, 1, false);
	PlaceBuilding(BUILDING_FACTORY, Vec2i(g_hmap.m_widthx/2+9, g_hmap.m_widthz/2-3), true, 0);
	PlaceBuilding(BUILDING_REFINERY, Vec2i(g_hmap.m_widthx/2+11, g_hmap.m_widthz/2-3), true, 0);
	PlaceBuilding(BUILDING_NUCPOW, Vec2i(g_hmap.m_widthx/2+13, g_hmap.m_widthz/2-3), true, 0);
	PlaceRoad(g_hmap.m_widthx/2+11, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+12, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+13, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+14, g_hmap.m_widthz/2-2, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+14, g_hmap.m_widthz/2-3, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+14, g_hmap.m_widthz/2-4, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+14, g_hmap.m_widthz/2-5, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+14, g_hmap.m_widthz/2-6, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+14, g_hmap.m_widthz/2-7, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+15, g_hmap.m_widthz/2-2, 1, false);
	PlaceBuilding(BUILDING_FARM, Vec2i(g_hmap.m_widthx/2+6, g_hmap.m_widthz/2-0), true, 0);
	PlaceRoad(g_hmap.m_widthx/2+10, g_hmap.m_widthz/2-1, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+10, g_hmap.m_widthz/2-0, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+10, g_hmap.m_widthz/2+1, 1, false);
	PlaceBuilding(BUILDING_STORE, Vec2i(g_hmap.m_widthx/2+9, g_hmap.m_widthz/2-1), true, 0);
	PlaceBuilding(BUILDING_OILWELL, Vec2i(g_hmap.m_widthx/2+9, g_hmap.m_widthz/2-0), true, 0);
	PlaceBuilding(BUILDING_MINE, Vec2i(g_hmap.m_widthx/2+11, g_hmap.m_widthz/2-0), true, 0);
	PlaceBuilding(BUILDING_MINE, Vec2i(g_hmap.m_widthx/2+12, g_hmap.m_widthz/2-0), true, 0);
	PlaceRoad(g_hmap.m_widthx/2+13, g_hmap.m_widthz/2-1, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+13, g_hmap.m_widthz/2-0, 1, false);
	PlaceRoad(g_hmap.m_widthx/2+13, g_hmap.m_widthz/2+1, 1, false);
	CheckGLError(__FILE__, __LINE__);
	PlaceBuilding(BUILDING_GASSTATION, Vec2i(g_hmap.m_widthx/2+14, g_hmap.m_widthz/2-1), true, 0);
	g_hmap.genvbo();
#endif

	CheckGLError(__FILE__, __LINE__);

	g_mode = APPMODE_PLAY;

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->closeall();
	gui->open("play gui");
	gui->open("play right opener");

	CheckGLError(__FILE__, __LINE__);

	gui->add(new WindowW(gui, "window", Resize_Window));
	//gui->open("window");

	WindowW* win = (WindowW*)gui->get("window");

	RichText bigtext;

	for(int i=0; i<2000; i++)
	{
		bigtext.m_part.push_back(RichTextP((unsigned int)(rand()%3000+16000)));

		if(rand()%10 == 1)
			bigtext.m_part.push_back(RichTextP((unsigned int)'\n'));
	}

	win->add(new TextBlock(win, "text block", bigtext, MAINFONT16, Resize_WinText));

	g_lastsave[0] = '\0';
}

void Click_OpenEditor()
{
	CheckGLError(__FILE__, __LINE__);
	LoadJPGMap("heightmaps/heightmap0e2.jpg");
	CheckGLError(__FILE__, __LINE__);

	g_mode = APPMODE_EDITOR;


	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->closeall();
	gui->open("editor gui");

	g_lastsave[0] = '\0';
}

void FillMenuGUI()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	// Main ViewLayer
	gui->add(new ViewLayer(gui, "mainmenu"));
	ViewLayer* mainmenuview = (ViewLayer*)gui->get("mainmenu");

	mainmenuview->add(new Link(NULL, "new game link", RichText("New Game"), MAINFONT8, Resize_NewGameLink, Click_NewGame));
}

void StartRoadPlacement()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	py->vdrag[0] = Vec3f(-1,-1,-1);
	py->vdrag[1] = Vec3f(-1,-1,-1);

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

	py->vdrag[0] = intersection;
	py->vdrag[1] = intersection;
}

void MouseLDown()
{
	if(g_mode == APPMODE_EDITOR)
	{
		int edtool = GetEdTool();

		if(edtool == EDTOOL_PLACEROADS ||
				edtool == EDTOOL_PLACECRUDEPIPES ||
				edtool == EDTOOL_PLACEPOWERLINES)
		{
			StartRoadPlacement();
		}
	}
	else if(g_mode == APPMODE_PLAY)
	{
		Player* py = &g_player[g_curP];
		py->mousestart = py->mouse;
	}
}

void EdPlaceUnit()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

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

	int type = GetPlaceUnitType();

	int country = GetPlaceUnitCountry();
	int company = GetPlaceUnitCompany();

	//PlaceUnit(type, Vec3i(intersection.x, intersection.y, intersection.z), country, company, -1);
	PlaceUnit(type, Vec2i(intersection.x, intersection.z), country, NULL);
#if 0
	g_hmap.setheight( intersection.x / TILE_SIZE, intersection.z / TILE_SIZE, 0);
	g_hmap.remesh();
#endif
}

void EdPlaceBuilding()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

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

	if(intersection.y < WATER_LEVEL && !GetMapIntersection2(&g_hmap, line, &intersection))
		return;


	int type = GetPlaceBuildingType();

	Vec2i tilepos (intersection.x/TILE_SIZE, intersection.z/TILE_SIZE);

	if(CheckCanPlace(type, tilepos))
		PlaceBuilding(type, tilepos, true, -1, NULL);
#if 0
	g_hmap.setheight( intersection.x / TILE_SIZE, intersection.z / TILE_SIZE, 0);
	g_hmap.remesh();
#endif
}

void EdDeleteObject()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	Selection sel = DoSel(c->zoompos(), c->m_strafe, c->up2(), c->m_view - c->m_pos);

	for(auto unititer = sel.units.begin(); unititer != sel.units.end(); unititer++)
	{
		g_unit[ *unititer ].destroy();
		return;
	}
}

void MouseLUp()
{
	if(g_mode == APPMODE_EDITOR)
	{
		int edtool = GetEdTool();

		if(edtool == EDTOOL_PLACEUNITS)
			EdPlaceUnit();
		else if(edtool == EDTOOL_PLACEBUILDINGS)
			EdPlaceBuilding();
		else if(edtool == EDTOOL_DELETEOBJECTS)
			EdDeleteObject();
		else if(edtool == EDTOOL_PLACEROADS)
			PlaceCo(CONDUIT_ROAD);
		else if(edtool == EDTOOL_PLACECRUDEPIPES)
			PlaceCo(CONDUIT_CRPIPE);
		else if(edtool == EDTOOL_PLACEPOWERLINES)
			PlaceCo(CONDUIT_POWL);
	}
	else if(g_mode == APPMODE_PLAY)
	{
		Player* py = &g_player[g_curP];
		Camera* c = &py->camera;
		GUI* gui = &py->gui;

		if(py->build == BUILDING_NONE)
		{
			gui->close("construction view");
			py->sel = DoSel(c->zoompos(), c->m_strafe, c->up2(), Normalize(c->m_view - c->zoompos()));
			AfterSel(&py->sel);
		}
		else if(py->build < BUILDING_TYPES)
		{
			if(py->canplace)
			{
				PlaceBuilding(py->build, Vec2i(py->vdrag[0].x/TILE_SIZE, py->vdrag[0].z/TILE_SIZE), false, g_localP, NULL);
			}
			else
			{
			}

			py->build = -1;
		}
		else if(py->build == BUILDING_ROAD)
		{
			PlaceCo(CONDUIT_ROAD);
			py->build = -1;
		}
		else if(py->build == BUILDING_POWL)
		{
			PlaceCo(CONDUIT_POWL);
			py->build = -1;
		}
		else if(py->build == BUILDING_CRPIPE)
		{
			PlaceCo(CONDUIT_CRPIPE);
			py->build = -1;
		}
	}
}

void RotateAbout()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	float dx = py->mouse.x - py->width/2;
	float dy = py->mouse.y - py->height/2;

	Camera oldcam = py->camera;
	Vec3f line[2];
	line[0] = c->zoompos();

	c->rotateabout(c->m_view, dy / 100.0f, c->m_strafe.x, c->m_strafe.y, c->m_strafe.z);
	c->rotateabout(c->m_view, dx / 100.0f, c->m_up.x, c->m_up.y, c->m_up.z);

	line[1] = c->zoompos();

	Vec3f ray = Normalize(line[1] - line[0]) * TILE_SIZE;

	//line[0] = line[0] - ray;
	line[1] = line[1] + ray;

	Vec3f clip;

#if 0
	if(GetMapIntersection(&g_hmap, line, &clip))
#else
	if(FastMapIntersect(&g_hmap, line, &clip))
#endif
		py->camera = oldcam;
	//else
	//	CalcMapView();

}

void UpdateRoadPlans()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	py->vdrag[1] = py->vdrag[0];

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

	py->vdrag[1] = intersection;

	UpdCoPlans(CONDUIT_ROAD, 0, py->vdrag[0], py->vdrag[1]);
}

void UpdateCrPipePlans()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	py->vdrag[1] = py->vdrag[0];

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

	py->vdrag[1] = intersection;

	UpdCoPlans(CONDUIT_CRPIPE, 0, py->vdrag[0], py->vdrag[1]);
}

void UpdatePowlPlans()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	py->vdrag[1] = py->vdrag[0];

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

	py->vdrag[1] = intersection;

	UpdCoPlans(CONDUIT_POWL, 0, py->vdrag[0], py->vdrag[1]);
}

void MouseMove()
{
	if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
	{
		Player* py = &g_player[g_curP];

		if(py->mousekeys[MOUSE_MIDDLE])
		{
			RotateAbout();
			CenterMouse();
		}

		UpdateSBuild();

		if(py->mousekeys[MOUSE_LEFT])
		{
			int edtool = GetEdTool();

			if(edtool == EDTOOL_PLACEROADS)
			{
				UpdateRoadPlans();
			}
			else if(edtool == EDTOOL_PLACECRUDEPIPES)
			{
				UpdateCrPipePlans();
			}
			else if(edtool == EDTOOL_PLACEPOWERLINES)
			{
				UpdatePowlPlans();
			}
		}
	}
}

void MouseRDown()
{
}

void MouseRUp()
{
	if(g_mode == APPMODE_PLAY)
	{
		Player* py = &g_player[g_curP];
		Camera* c = &py->camera;

		if(!py->keyintercepted)
		{
			Order(py->mouse.x, py->mouse.y, py->width, py->height, c->zoompos(), c->m_view, Normalize(c->m_view - c->zoompos()), c->m_strafe, c->up2());
		}
	}
}

void FillGUI()
{
	//DrawSceneFunc = DrawScene;
	//DrawSceneDepthFunc = DrawSceneDepth;

	g_log<<"2.1"<<std::endl;
	g_log.flush();

	for(int i=0; i<PLAYERS; i++)
	{
		g_log<<"2.1.1"<<std::endl;
		g_log.flush();

		Player* py = &g_player[i];
		GUI* gui = &py->gui;

		g_log<<"2.1.2"<<std::endl;
		g_log.flush();

		gui->assignkey(SDL_SCANCODE_F1, SaveScreenshot, NULL);
		gui->assignkey(SDL_SCANCODE_F2, LogPathDebug, NULL);
		gui->assignlbutton(MouseLDown, MouseLUp);
		gui->assignrbutton(MouseRDown, MouseRUp);
		gui->assignmousemove(MouseMove);

		g_log<<"2.1.3"<<std::endl;
		g_log.flush();
	}

	g_log<<"2.2"<<std::endl;
	g_log.flush();

	// Loading ViewLayer
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	g_log<<"2.2"<<std::endl;
	g_log.flush();

	gui->add(new ViewLayer(gui, "loading"));
	ViewLayer* loadingview = (ViewLayer*)gui->get("loading");

	g_log<<"2.3"<<std::endl;
	g_log.flush();

	loadingview->add(new Text(NULL, "status", RichText("Loading..."), MAINFONT8, Resize_LoadingStatus));

	gui->closeall();
	gui->open("loading");

	g_log<<"2.4"<<std::endl;
	g_log.flush();

	FillMenuGUI();

	g_log<<"2.5"<<std::endl;
	g_log.flush();

	FillEditorGUI();

	g_log<<"2.6"<<std::endl;
	g_log.flush();

	FillPlayGUI();

	g_log<<"2.7"<<std::endl;
	g_log.flush();

	FillConsole();
}
