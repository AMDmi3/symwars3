#include "../gmain.h"
#include "../../common/gui/gui.h"
#include "../../common/gui/widgets/windoww.h"
#include "../keymap.h"
#include "../../common/render/heightmap.h"
#include "../../common/math/camera.h"
#include "../../common/render/shadow.h"
#include "../../common/render/screenshot.h"
#include "../../common/save/savemap.h"
#include "ggui.h"
#include "playgui.h"
#include "../../common/gui/icon.h"
#include "../../common/gui/widgets/spez/resticker.h"
#include "../../common/gui/widgets/spez/bottompanel.h"
#include "../../common/gui/widgets/spez/buildpreview.h"
#include "../../common/gui/widgets/spez/constructionview.h"
#include "gviewport.h"
#include "../../common/sim/buildingtype.h"
#include "../../common/sim/building.h"
#include "../../common/sim/road.h"
#include "../../common/sim/powl.h"
#include "../../common/sim/crpipe.h"
#include "../../common/sim/player.h"

void Resize_ResNamesTextBlock(Widget* thisw)
{
	Player* py = &g_player[g_curP];
	thisw->m_pos[0] = 0;
	thisw->m_pos[1] = 10;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = py->height;
}

void Resize_ResAmtsTextBlock(Widget* thisw)
{
	Player* py = &g_player[g_curP];
	thisw->m_pos[0] = 150;
	thisw->m_pos[1] = 10;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = py->height;
}

void Resize_ResDeltasTextBlock(Widget* thisw)
{
	Player* py = &g_player[g_curP];
	thisw->m_pos[0] = 250;
	thisw->m_pos[1] = 10;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = py->height;
}

void Out_Build()
{
}

void Resize_ResTicker(Widget* thisw)
{
	Player* py = &g_player[g_curP];
	thisw->m_pos[0] = 0;
	thisw->m_pos[1] = 0;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = g_font[MAINFONT16].gheight+5;
	thisw->m_tpos[0] = 0;
	thisw->m_tpos[1] = 0;
}

void UpdateResTicker()
{
	//return;

	static float tickerpos = 0;

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	ViewLayer* playguiview = (ViewLayer*)gui->get("play gui");
	ResTicker* restickerw = (ResTicker*)playguiview->get("res ticker");
	Widget* restickertw = &restickerw->restext;
	RichText restext;

	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_DOLLARS));
	restext.m_part.push_back(RichTextP(" Funds: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_HOUSING));
	restext.m_part.push_back(RichTextP(" Housing: 100/120"));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_FARMPRODUCT));
	restext.m_part.push_back(RichTextP(" Farm Products: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_RETFOOD));
	restext.m_part.push_back(RichTextP(" Retail Food: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_PRODUCTION));
	restext.m_part.push_back(RichTextP(" Production: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_IRONORE));
	restext.m_part.push_back(RichTextP(" Minerals: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_CRUDEOIL));
	restext.m_part.push_back(RichTextP(" Crude Oil: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_WSFUEL));
	restext.m_part.push_back(RichTextP(" Wholesale Fuel: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_RETFUEL));
	restext.m_part.push_back(RichTextP(" Retail Fuel: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_ENERGY));
	restext.m_part.push_back(RichTextP(" Energy: 100/120"));
	restext.m_part.push_back(RichTextP("    "));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_ENRICHEDURAN));
	restext.m_part.push_back(RichTextP(" Uranium: 100 +1/"));
	restext.m_part.push_back(RichTextP(RICHTEXT_ICON, ICON_TIME));
	restext.m_part.push_back(RichTextP("    "));

	int len = restext.texlen();

	tickerpos += 0.5f * g_drawfrinterval * 20;

	if((int)tickerpos > len)
		tickerpos = 0;

	RichText restext2 = restext.substr((int)tickerpos, len-(int)tickerpos) + restext.substr(0, (int)tickerpos);

	restickertw->m_text = restext2;
}

void Resize_BottomPanel(Widget* thisw)
{
	Player* py = &g_player[g_curP];

	thisw->m_pos[0] = 0;
	thisw->m_pos[1] = py->height - MINIMAP_SIZE - 32;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = py->height;
}

void Out_BuildButton()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	gui->close("build preview");
}

void Click_BuildButton(int bwhat)
{
	Player* py = &g_player[g_curP];
	py->build = bwhat;
	//g_log<<"b "<<py->build<<std::endl;
	Out_BuildButton();
}

void Over_BuildButton(int bwhat)
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	if(gui->get("construction view")->m_opened)
		return;

	py->bptype = bwhat;

	gui->open("build preview");

	py->bpcam.position(TILE_SIZE*3, TILE_SIZE*3, TILE_SIZE*3, 0, 0, 0, 0, 1, 0);
}

void Click_NextBuildButton(int nextpage)
{
	if(nextpage == 1)
		BuildMenu_OpenPage1();
	else if(nextpage == 2)
		BuildMenu_OpenPage2();
}

void BuildMenu_OpenPage1()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	ViewLayer* playguiview = (ViewLayer*)gui->get("play gui");

	BottomPanel* bp = (BottomPanel*)playguiview->get("bottom panel");

#if 0
	Button(Widget* parent, const char* filepath, const RichText t, int f, int style, void (*reframef)(Widget* thisw), void (*click)(), void (*click2)(int p), void (*overf)(), void (*overf2)(int p), void (*out)(), int parm)
#endif

	bp->bottomright_button[0] = Button(bp, "name", "gui/brbut/apartment2.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_APARTMENT);
	bp->bottomright_button_on[0] = true;

	bp->bottomright_button[1] = Button(bp, "name", "gui/brbut/store1.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_STORE);
	bp->bottomright_button_on[1] = true;

	bp->bottomright_button[2] = Button(bp, "name", "gui/brbut/farm2.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_FARM);
	bp->bottomright_button_on[2] = true;

	bp->bottomright_button[3] = Button(bp, "name", "gui/brbut/oilwell2.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_OILWELL);
	bp->bottomright_button_on[3] = true;

	bp->bottomright_button[4] = Button(bp, "name", "gui/brbut/refinery2.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_REFINERY);
	bp->bottomright_button_on[4] = true;

	bp->bottomright_button[5] = Button(bp, "name", "gui/brbut/gasstation2.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_GASSTATION);
	bp->bottomright_button_on[5] = true;

	bp->bottomright_button[6] = Button(bp, "name", "gui/brbut/mine.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_MINE);
	bp->bottomright_button_on[6] = true;

	bp->bottomright_button[7] = Button(bp, "name", "gui/brbut/factory3.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_FACTORY);
	bp->bottomright_button_on[7] = true;

	bp->bottomright_button[8] = Button(bp, "name", "gui/next.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_NextBuildButton, NULL, NULL, NULL, 2);
	bp->bottomright_button_on[8] = true;

	bp->reframe();
}


void BuildMenu_OpenPage2()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	ViewLayer* playguiview = (ViewLayer*)gui->get("play gui");

	BottomPanel* bp = (BottomPanel*)playguiview->get("bottom panel");

	bp->bottomright_button[0] = Button(bp, "name", "gui/brbut/nucpow2.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_NUCPOW);
	bp->bottomright_button_on[0] = true;

	bp->bottomright_button[1] = Button(bp, "name", "gui/brbut/harbour2.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_HARBOUR);
	bp->bottomright_button_on[1] = true;

	bp->bottomright_button[2] = Button(bp, "name", "gui/brbut/road.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_ROAD);
	bp->bottomright_button_on[2] = true;

	bp->bottomright_button[3] = Button(bp, "name", "gui/brbut/crudepipeline.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_CRPIPE);
	bp->bottomright_button_on[3] = true;

	bp->bottomright_button[4] = Button(bp, "name", "gui/brbut/powerline.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_POWL);
	bp->bottomright_button_on[4] = true;

	bp->bottomright_button[5] = Button(bp, "name", "gui/next.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_NextBuildButton, NULL, NULL, NULL, 1);
	bp->bottomright_button_on[5] = true;

	bp->bottomright_button[6] = Button(bp, "name", "gui/brbut/mine.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_MINE);
	bp->bottomright_button_on[6] = false;

	bp->bottomright_button[7] = Button(bp, "name", "gui/brbut/factory3.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_BuildButton, NULL, Over_BuildButton, Out_BuildButton, BUILDING_FACTORY);
	bp->bottomright_button_on[7] = false;

	bp->bottomright_button[8] = Button(bp, "name", "gui/next.png", RichText(""), RichText(""), MAINFONT8, BUTTON_CORRODE, NULL, NULL, Click_NextBuildButton, NULL, NULL, NULL, 2);
	bp->bottomright_button_on[8] = false;

	bp->reframe();
}

void Resize_Fullscreen(Widget* thisw)
{
	Player* py = &g_player[g_curP];

	thisw->m_pos[0] = 0;
	thisw->m_pos[1] = 0;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = py->height;
}

void Resize_BuildPreview(Widget* thisw)
{
	Player* py = &g_player[g_curP];

#if 0
	int centerx = py->width/2;
	int centery = py->height/2;

	thisw->m_pos[0] = centerx-200;
	thisw->m_pos[1] = centery-200;
	thisw->m_pos[2] = centerx+200;
	thisw->m_pos[3] = centery+200;
#elif 1
	thisw->m_pos[0] = py->width - 400;
	thisw->m_pos[1] = py->height - MINIMAP_SIZE - 32 - 400;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = py->height - MINIMAP_SIZE - 32;
#endif
}

void Resize_ConstructionView(Widget* thisw)
{
	Player* py = &g_player[g_curP];

	thisw->m_pos[0] = 0;
	thisw->m_pos[1] = py->height - MINIMAP_SIZE - 32 - 400;
	thisw->m_pos[2] = 400;
	thisw->m_pos[3] = py->height - MINIMAP_SIZE - 32;
}

void Click_MoveConstruction()
{
	int alloced[RESOURCES];
	Zero(alloced);
	int totalloc = 0;

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	for(auto siter = py->sel.buildings.begin(); siter != py->sel.buildings.end(); siter++)
	{
		int bi = *siter;
		Building* b = &g_building[bi];

		for(int i=0; i<RESOURCES; i++)
		{
			alloced[i] += b->conmat[i];
			totalloc += b->conmat[i];
		}
	}

#if 0
	for(auto siter = py->sel.roads.begin(); siter != py->sel.roads.end(); siter++)
	{
		Vec2i tpos = *siter;
		RoadTile* r = RoadAt(tpos.x, tpos.y);

		for(int i=0; i<RESOURCES; i++)
		{
			alloced[i] += r->conmat[i];
			totalloc += r->conmat[i];
		}
	}

	for(auto siter = py->sel.powls.begin(); siter != py->sel.powls.end(); siter++)
	{
		Vec2i tpos = *siter;
		PowlTile* p = PowlAt(tpos.x, tpos.y);

		for(int i=0; i<RESOURCES; i++)
		{
			alloced[i] += p->conmat[i];
			totalloc += p->conmat[i];
		}
	}

	for(auto siter = py->sel.crpipes.begin(); siter != py->sel.crpipes.end(); siter++)
	{
		Vec2i tpos = *siter;
		CrPipeTile* p = CrPipeAt(tpos.x, tpos.y);

		for(int i=0; i<RESOURCES; i++)
		{
			alloced[i] += p->conmat[i];
			totalloc += p->conmat[i];
		}
	}
#endif

	if(totalloc <= 0)
	{
		for(auto siter = py->sel.buildings.begin(); siter != py->sel.buildings.end(); siter++)
		{
			int bi = *siter;
			Building* b = &g_building[bi];
			b->on = false;
		}

#if 0
		for(auto siter = py->sel.roads.begin(); siter != py->sel.roads.end(); siter++)
		{
			Vec2i tpos = *siter;
			RoadTile* r = RoadAt(tpos.x, tpos.y);
			r->on = false;
		}

		for(auto siter = py->sel.powls.begin(); siter != py->sel.powls.end(); siter++)
		{
			Vec2i tpos = *siter;
			PowlTile* p = PowlAt(tpos.x, tpos.y);
			p->on = false;
		}

		for(auto siter = py->sel.crpipes.begin(); siter != py->sel.crpipes.end(); siter++)
		{
			Vec2i tpos = *siter;
			CrPipeTile* p = CrPipeAt(tpos.x, tpos.y);
			p->on = false;
		}
#endif

		ClearSel(&py->sel);
		gui->close("construction view");
	}
	else
	{
		ShowMessage(RichText("You've already invested resources in this project."));
	}
}

void Click_CancelConstruction()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	for(auto siter = py->sel.buildings.begin(); siter != py->sel.buildings.end(); siter++)
	{
		int bi = *siter;
		Building* b = &g_building[bi];
		b->on = false;
	}

#if 0
	for(auto siter = py->sel.roads.begin(); siter != py->sel.roads.end(); siter++)
	{
		Vec2i tpos = *siter;
		RoadTile* r = RoadAt(tpos.x, tpos.y);
		r->on = false;
	}

	for(auto siter = py->sel.powls.begin(); siter != py->sel.powls.end(); siter++)
	{
		Vec2i tpos = *siter;
		PowlTile* p = PowlAt(tpos.x, tpos.y);
		p->on = false;
	}

	for(auto siter = py->sel.crpipes.begin(); siter != py->sel.crpipes.end(); siter++)
	{
		Vec2i tpos = *siter;
		CrPipeTile* p = CrPipeAt(tpos.x, tpos.y);
		p->on = false;
	}
#endif

	ClearSel(&py->sel);
	gui->close("construction view");
}

void Click_ProceedConstruction()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	ViewLayer* cv = (ViewLayer*)gui->get("construction view");
	ConstructionView* cvw = (ConstructionView*)cv->get("construction view");

	int maxcost[RESOURCES];
	Zero(maxcost);

	for(auto rriter = cvw->reqrows.begin(); rriter != cvw->reqrows.end(); rriter++)
	{
		int resi = rriter->restype;
		maxcost[resi] = StrToInt(rriter->editbox.m_value.rawstr().c_str());
	}

	for(auto siter = py->sel.buildings.begin(); siter != py->sel.buildings.end(); siter++)
	{
		int bi = *siter;
		Building* b = &g_building[bi];

		for(int i=0; i<RESOURCES; i++)
			b->maxcost[i] = maxcost[i];

		b->allocres();
	}
	
#if 0
	for(auto siter = py->sel.roads.begin(); siter != py->sel.roads.end(); siter++)
	{
		Vec2i tpos = *siter;
		RoadTile* r = RoadAt(tpos.x, tpos.y);

		for(int i=0; i<RESOURCES; i++)
			r->maxcost[i] = maxcost[i];

		r->allocate();
	}

	for(auto siter = py->sel.powls.begin(); siter != py->sel.powls.end(); siter++)
	{
		Vec2i tpos = *siter;
		PowlTile* p = PowlAt(tpos.x, tpos.y);

		for(int i=0; i<RESOURCES; i++)
			p->maxcost[i] = maxcost[i];

		p->allocate();
	}

	for(auto siter = py->sel.crpipes.begin(); siter != py->sel.crpipes.end(); siter++)
	{
		Vec2i tpos = *siter;
		CrPipeTile* p = CrPipeAt(tpos.x, tpos.y);

		for(int i=0; i<RESOURCES; i++)
			p->maxcost[i] = maxcost[i];

		p->allocate();
	}
#endif

	ClearSel(&py->sel);
	gui->close("construction view");
}

void Click_EstimateConstruction()
{
	Player* py = &g_player[g_curP];
	ClearSel(&py->sel);
	GUI* gui = &py->gui;
	gui->close("construction view");
}

void Resize_Message(Widget* thisw)
{
	Player* py = &g_player[g_curP];
	thisw->m_pos[0] = py->width/2 - 200;
	thisw->m_pos[1] = py->height/2 - 100;
	thisw->m_pos[2] = py->width/2 + 200;
	thisw->m_pos[3] = py->height/2 + 100;
}

void Resize_MessageContinue(Widget* thisw)
{
	Player* py = &g_player[g_curP];
	thisw->m_pos[0] = py->width/2 - 80;
	thisw->m_pos[1] = py->height/2 + 70;
	thisw->m_pos[2] = py->width/2 + 80;
	thisw->m_pos[3] = py->height/2 + 90;
}

void Click_MessageContinue()
{
#if 0
	auto viter = gui->view.begin();
	while(viter != gui->view.end())
	{
		if(stricmp(viter->name.c_str(), "message view") == 0)
		{
			InfoMessage("f", "view found");

			viter = gui->view.erase(viter);

			continue;
		}

		viter++;
	}
#endif

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	gui->close("message view");
}

void ShowMessage(const RichText& msg)
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	ViewLayer* msgview = (ViewLayer*)gui->get("message view");
	TextBlock* msgblock = (TextBlock*)msgview->get("message");
	msgblock->m_text = msg;
	gui->open("message view");
}

void Resize_Window(Widget* thisw)
{
	Player* py = &g_player[g_curP];
	thisw->m_pos[0] = py->width/2 - 200;
	thisw->m_pos[1] = py->height/2 - 200;
	thisw->m_pos[2] = py->width/2 + 200;
	thisw->m_pos[3] = py->height/2 + 200;
}

void FillPlayGUI()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	g_viewportT[VIEWPORT_MINIMAP] = ViewportT(Vec3f(0, 0, 0), Vec3f(0, 1, 0), "Minimap", true);
	g_viewportT[VIEWPORT_ENTVIEW] = ViewportT(Vec3f(0, MAX_DISTANCE/2, 0), Vec3f(0, 1, 0), "EntView", false);

	//for(int i=0; i<25; i++)
	//Sleep(6000);

	gui->add(new ViewLayer(gui, "play gui"));
	ViewLayer* playguiview = (ViewLayer*)gui->get("play gui");

	//playguiview->add(new Image(NULL, "gui/backg/white.png", Resize_ResTicker));
	//playguiview->add(new Text(NULL, "res ticker", RichText(" "), MAINFONT16, Resize_ResTicker, true, 1, 1, 1, 1));
	playguiview->add(new ResTicker(NULL, "res ticker", Resize_ResTicker));
	playguiview->add(new BottomPanel(NULL, "bottom panel", Resize_BottomPanel));

	//preload all the button images
	BuildMenu_OpenPage2();
	BuildMenu_OpenPage1();

	gui->add(new ViewLayer(gui, "construction view"));
	ViewLayer* constrview = (ViewLayer*)gui->get("construction view");

	constrview->add(new ConstructionView(NULL, "construction view", Resize_ConstructionView, Click_MoveConstruction, Click_CancelConstruction, Click_ProceedConstruction, Click_EstimateConstruction));

	gui->add(new ViewLayer(gui, "build preview"));
	ViewLayer* buildpreview = (ViewLayer*)gui->get("build preview");

	//buildpreview->add(new TouchListener(NULL, Resize_Fullscreen, NULL, NULL, NULL, -1));
	buildpreview->add(new BuildPreview(NULL, "build preview", Resize_BuildPreview));

	gui->add(new ViewLayer(gui, "construction estimate view"));
	ViewLayer* cev = (ViewLayer*)gui->get("construction estimate view");

	gui->add(new ViewLayer(gui, "message view"));
	ViewLayer* msgview = (ViewLayer*)gui->get("message view");

	msgview->add(new Image(NULL, "gui/backg/white.jpg", true, Resize_Message));
	msgview->add(new TextBlock(NULL, "message", RichText(""), MAINFONT16, Resize_Message));
	msgview->add(new TouchListener(NULL, Resize_Fullscreen, NULL, NULL, NULL, -1));
	msgview->add(new Button(NULL, "continue button", "gui/transp.png", RichText("Continue"), RichText(""), MAINFONT16, BUTTON_LEFTIMAGE, Resize_MessageContinue, Click_MessageContinue, NULL, NULL, NULL, NULL, -1));
}
