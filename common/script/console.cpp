#include "console.h"
#include "../gui/gui.h"
#include "../sim/player.h"

void Resize_ConsoleLine(Widget* thisw)
{
	Font* f = &g_font[thisw->m_font];
	Player* py = &g_player[g_curP];

	int i = 0;
	sscanf(thisw->m_name.c_str(), "%d", &i);

	thisw->m_pos[0] = 0;
	thisw->m_pos[1] = 30 + f->gheight * i;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = 30 + f->gheight * (i+1);
}

void Resize_Console(Widget* thisw)
{
	Font* f = &g_font[thisw->m_font];
	Player* py = &g_player[g_curP];
	int i = CONSOLE_LINES;
	thisw->m_pos[0] = 0;
	thisw->m_pos[1] = 30 + f->gheight * i;
	thisw->m_pos[2] = py->width;
	thisw->m_pos[3] = 30 + f->gheight * (i+1);
}

void Change_Console(unsigned int key, unsigned int scancode, bool down)
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	ViewLayer* conview = (ViewLayer*)gui->get("console");
	EditBox* con = (EditBox*)conview->get("console");

#if 0
	int caret = con->m_caret;

	if(caret <= 0)
		return;

	unsigned int enter = con->m_value.substr(caret-1, 1).m_part.begin()->m_text.m_data[0];

	if(enter == '\n' || enter == '\r')
		InfoMessage("console", "enter");
#endif

	if(scancode == SDL_SCANCODE_ESCAPE && !down)
		ToggleConsole();
}

void SubmitConsole(RichText* rt)
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->add(new ViewLayer(gui, "console"));
	ViewLayer* con = (ViewLayer*)gui->get("console");

	auto witer = con->m_subwidg.begin();
	for(int i=0; i<CONSOLE_LINES-1; i++)
	{
		auto witer2 = witer;
		witer2++;
		(*witer)->m_text = (*witer2)->m_text;
		witer = witer2;
	}
	
	auto witer2 = witer;
	witer2++;
	(*witer)->m_text = ParseTags(*rt, NULL);
}

void Submit_Console()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->add(new ViewLayer(gui, "console"));
	ViewLayer* con = (ViewLayer*)gui->get("console");
	
	EditBox* coned = (EditBox*)con->get("console");
	SubmitConsole(&coned->m_value);
	coned->changevalue("");
}

void FillConsole()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	gui->add(new ViewLayer(gui, "console"));
	ViewLayer* con = (ViewLayer*)gui->get("console");

	int y = 30;
	for(int i=0; i<CONSOLE_LINES; i++)
	{
		char name[32];
		sprintf(name, "%d", i);
		con->add(new Text(con, name, RichText(name), MAINFONT16, Resize_ConsoleLine));
	}

	con->add(new EditBox(con, "console", RichText("console"), MAINFONT16, Resize_Console, false, 128, Change_Console, Submit_Console, -1));
}

void ToggleConsole()
{
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	ViewLayer* con = (ViewLayer*)gui->get("console");
	con->get("console")->m_opened = true;
	con->m_opened =! con->m_opened;
}