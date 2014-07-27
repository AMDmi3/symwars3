#include "../gmain.h"
#include "../../common/gui/gui.h"
#include "../keymap.h"
#include "../../common/render/heightmap.h"
#include "../../common/math/camera.h"
#include "../../common/render/shadow.h"
#include "../../common/render/screenshot.h"
#include "../../common/save/savemap.h"
#include "ggui.h"
#include "../../common/sim/unittype.h"
#include "editorgui.h"
#include "../../common/save/savemap.h"
#include "../../common/sim/buildingtype.h"
#include "../../common/sim/player.h"

void Resize_LeftPanel(Widget* thisw)
{
	Player* py = &g_player[g_curP];
	thisw->m_pos[0] = 0;
	thisw->m_pos[1] = 0;
	thisw->m_pos[2] = LEFT_PANEL_WIDTH;
	thisw->m_pos[3] = py->height;
}

void Resize_EditorToolsSelector(Widget* thisw)
{
	thisw->m_pos[0] = 5;
	thisw->m_pos[1] = 5 + 64 + SCROLL_BORDER;
	thisw->m_pos[2] = LEFT_PANEL_WIDTH - 5;
	thisw->m_pos[3] = 5 + 64 + SCROLL_BORDER + g_font[thisw->m_font].gheight + 5;
}

int EdToolsSelection()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	ViewLayer* editorguiview = (ViewLayer*)gui->get("editor gui");

	Widget* edtoolsselector = editorguiview->get("editor tools selector");

	int selected = edtoolsselector->m_selected;

	return selected;
}

void CloseEdTools()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->close("borders");
	gui->close("place units");
	gui->close("place buildings");
	gui->close("place roads");
	gui->close("place powls");
	gui->close("place crude pipelines");

	py->build = BUILDING_NONE;
}

int GetEdTool()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	ViewLayer* editorguiview = (ViewLayer*)gui->get("editor gui");

	Widget* edtoolsselector = editorguiview->get("editor tools selector");

	int selected = edtoolsselector->m_selected;

	return selected;
}

void Change_EditorToolsSelector()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	int selected = GetEdTool();

	CloseEdTools();

	// Borders
	if(selected == EDTOOL_BORDERS)
	{
		gui->open("borders");
	}
	// Place Units
	else if(selected == EDTOOL_PLACEUNITS)
	{
		gui->open("place units");
	}
	// Place Buildings
	else if(selected == EDTOOL_PLACEBUILDINGS)
	{
		gui->open("place buildings");
	}
	// Delete Objects
	else if(selected == EDTOOL_DELETEOBJECTS)
	{
		gui->open("delete objects");
	}
	else if(selected == EDTOOL_PLACEROADS)
	{
		py->build = BUILDING_ROAD;
	}
	else if(selected == EDTOOL_PLACECRUDEPIPES)
	{
		py->build = BUILDING_CRPIPE;
	}
	else if(selected == EDTOOL_PLACEPOWERLINES)
	{
		py->build = BUILDING_POWL;
	}
#if 0
#define EDTOOL_PLACEROADS		4
#define EDTOOL_PLACEPOWERLINES	5
#define EDTOOL_PLACECRUDEPIPES	6
#endif
}

void Resize_UnitDropDownSelector(Widget* thisw)
{
	thisw->m_pos[0] = 5;
	thisw->m_pos[1] = 5 + 64 + SCROLL_BORDER + 20;
	thisw->m_pos[2] = LEFT_PANEL_WIDTH - 5;
	thisw->m_pos[3] = thisw->m_pos[1] + g_font[thisw->m_font].gheight + 5;
}

void Resize_UnitCountryDropDownSelector(Widget* thisw)
{
	thisw->m_pos[0] = 5;
	thisw->m_pos[1] = 5 + 64 + SCROLL_BORDER + 20*3;
	thisw->m_pos[2] = LEFT_PANEL_WIDTH - 5;
	thisw->m_pos[3] = thisw->m_pos[1] + g_font[thisw->m_font].gheight + 5;
}

void Resize_UnitCompanyDropDownSelector(Widget* thisw)
{
	thisw->m_pos[0] = 5;
	thisw->m_pos[1] = 5 + 64 + SCROLL_BORDER + 20*4;
	thisw->m_pos[2] = LEFT_PANEL_WIDTH - 5;
	thisw->m_pos[3] = thisw->m_pos[1] + g_font[thisw->m_font].gheight + 5;
}

void Resize_BuildingDropDownSelector(Widget* thisw)
{
	thisw->m_pos[0] = 5;
	thisw->m_pos[1] = 5 + 64 + SCROLL_BORDER + 20;
	thisw->m_pos[2] = LEFT_PANEL_WIDTH - 5;
	thisw->m_pos[3] = thisw->m_pos[1] + g_font[thisw->m_font].gheight + 5;
}

void Change_UnitDropDownSelector()
{
}

void FillPlaceUnitsView()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->add(new ViewLayer(gui, "place units"));
	ViewLayer* placeunitsview = (ViewLayer*)gui->get("place units");

	placeunitsview->add(new DropDownS(NULL, "unit selector", MAINFONT8, Resize_UnitDropDownSelector, NULL));

	Widget* unitselector = placeunitsview->get("unit selector");

	for(int i=0; i<UNIT_TYPES; i++)
		unitselector->m_options.push_back(RichText(g_utype[i].name));

	//unitselector->m_options.push_back(RichText("Labourer"));
	//unitselector->m_options.push_back(RichText("Truck"));
	//unitselector->m_options.push_back(RichText("Infantry"));

	//unitselector->select(0);

	placeunitsview->add(new DropDownS(NULL, "unit country selector", MAINFONT8, Resize_UnitCountryDropDownSelector, NULL));

	Widget* unitcountryselector = placeunitsview->get("unit country selector");

	unitcountryselector->m_options.push_back(RichText("No country"));
	for(int i=0; i<PLAYERS; i++)
		unitcountryselector->m_options.push_back(g_player[i].name);

	placeunitsview->add(new DropDownS(NULL, "unit company selector", MAINFONT8, Resize_UnitCompanyDropDownSelector, NULL));

	Widget* unitcompanyselector = placeunitsview->get("unit company selector");

	unitcompanyselector->m_options.push_back(RichText("No cmpany"));
}

int GetPlaceUnitCountry()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	ViewLayer* placeunitsview = (ViewLayer*)gui->get("place units");

	Widget* unitcountryselector = placeunitsview->get("unit country selector");

	return unitcountryselector->m_selected - 1;
}

int GetPlaceUnitCompany()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	ViewLayer* placeunitsview = (ViewLayer*)gui->get("place units");

	Widget* unitcompanyselector = placeunitsview->get("unit company selector");

	return unitcompanyselector->m_selected - 1;
}

void FillPlaceBuildingsView()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->add(new ViewLayer(gui, "place buildings"));
	ViewLayer* placebuildingsview = (ViewLayer*)gui->get("place buildings");

	placebuildingsview->add(new DropDownS(NULL, "building selector", MAINFONT8, Resize_BuildingDropDownSelector, NULL));

	Widget* buildingselector = placebuildingsview->get("building selector");

	for(int i=0; i<BUILDING_TYPES; i++)
	{
		buildingselector->m_options.push_back(RichText(g_bltype[i].name));
	}

	//placebuildingsview->add(new DropDownS(NULL, "country owner selector", MAINFONT8, Resize_BuildingCountryDropDownSelector, NULL));

	//placebuildingsview->add(new DropDownS(NULL, "company owner selector", MAINFONT8, Resize_BuildingCompanyDropDownSelector, NULL));
}

int GetPlaceUnitType()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	ViewLayer* placeunitsview = (ViewLayer*)gui->get("place units");

	Widget* unitselector = placeunitsview->get("unit selector");

	return unitselector->m_selected;
}

int GetPlaceBuildingType()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	ViewLayer* placebuildingsview = (ViewLayer*)gui->get("place buildings");

	Widget* buildingselector = placebuildingsview->get("building selector");

	return buildingselector->m_selected;
}

void Resize_LoadMapButton(Widget* thisw)
{
	thisw->m_pos[0] = 5 + 64*0;
	thisw->m_pos[1] = 5 + SCROLL_BORDER;
	thisw->m_pos[2] = 5 + 64*1;
	thisw->m_pos[3] = thisw->m_pos[1] + 64;
}

void Resize_SaveMapButton(Widget* thisw)
{
	thisw->m_pos[0] = 5 + 64*1;
	thisw->m_pos[1] = 5 + SCROLL_BORDER;
	thisw->m_pos[2] = 5 + 64*2;
	thisw->m_pos[3] = thisw->m_pos[1] + 64;
}

void Resize_QSaveMapButton(Widget* thisw)
{
	thisw->m_pos[0] = 5 + 64*2;
	thisw->m_pos[1] = 5 + SCROLL_BORDER;
	thisw->m_pos[2] = 5 + 64*3;
	thisw->m_pos[3] = thisw->m_pos[1] + 64;
}

void FillEditorGUI()
{
	g_lastsave[0] = '\0';

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->add(new ViewLayer(gui, "editor gui"));
	ViewLayer* editorguiview = (ViewLayer*)gui->get("editor gui");

	editorguiview->add(new Image(NULL, "gui/filled.jpg", true, Resize_LeftPanel));

#if 0
	Button(Widget* parent, const char* filepath, const RichText t, int f, int style, void (*reframef)(Widget* thisw), void (*click)(), void (*click2)(int p), void (*overf)(), void (*overf2)(int p), void (*out)(), int parm)
#endif

	editorguiview->add(new Button(NULL, "name", "gui/edload.png", RichText(""), RichText("Load map"), MAINFONT8, BUTTON_CORRODE, Resize_LoadMapButton, Click_LoadMapButton, NULL, NULL, NULL, NULL, -1));
	editorguiview->add(new Button(NULL, "name", "gui/edsave.png", RichText(""), RichText("Save map"), MAINFONT8, BUTTON_CORRODE, Resize_SaveMapButton, Click_SaveMapButton, NULL, NULL, NULL, NULL, -1));
	editorguiview->add(new Button(NULL, "name", "gui/qsave.png", RichText(""), RichText("Quick save"), MAINFONT8, BUTTON_CORRODE, Resize_QSaveMapButton, Click_QSaveMapButton, NULL, NULL, NULL, NULL, -1));

	editorguiview->add(new DropDownS(NULL, "editor tools selector", MAINFONT8, Resize_EditorToolsSelector, Change_EditorToolsSelector));

	Widget* edtoolsselector = editorguiview->get("editor tools selector");

#if 0
#define EDTOOL_BORDERS			0
#define EDTOOL_PLACEUNITS		1
#define EDTOOL_PLACEBUILDINGS	2
#define EDTOOL_DELETEOBJECTS	3
#endif

	edtoolsselector->m_options.push_back(RichText("Borders"));
	edtoolsselector->m_options.push_back(RichText("Place Units"));
	edtoolsselector->m_options.push_back(RichText("Place Buildings"));
	edtoolsselector->m_options.push_back(RichText("Delete Objects"));
	edtoolsselector->m_options.push_back(RichText("Place Roads"));
	edtoolsselector->m_options.push_back(RichText("Place Powerline"));
	edtoolsselector->m_options.push_back(RichText("Place Crude Oil Pipeline"));

	//edtoolsselector->select(1);

	FillPlaceUnitsView();
	FillPlaceBuildingsView();
}
