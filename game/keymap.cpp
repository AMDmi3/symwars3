#include "gmain.h"
#include "keymap.h"
#include "../common/platform.h"
#include "../common/gui/gui.h"
#include "../common/math/camera.h"
#include "../common/math/vec3f.h"
#include "../common/math/hmapmath.h"
#include "../common/render/heightmap.h"
#include "../common/window.h"
#include "../common/sim/player.h"
#include "../common/script/console.h"

void MouseMidButtonDown()
{
	if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
	{
		Player* py = &g_player[g_curP];

		if(py->mousekeys[MOUSE_MIDDLE])
		{
			CenterMouse();
		}
	}
}

void MouseMidButtonUp()
{
}

void MouseWheel(int delta)
{
	if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
	{
		Player* py = &g_player[g_curP];
		Camera* c = &py->camera;

		if(py->zoom <= MIN_ZOOM && delta < 0)
			return;

		if(py->zoom >= MAX_ZOOM && delta > 0)
			return;

		float oldzoom = py->zoom;
		Vec3f line[2];
		line[0] = c->zoompos();

		py->zoom *= 1.0f + (float)delta / 10.0f;
		line[1] = c->zoompos();

		Vec3f ray = Normalize( line[1] - line[0] );

		line[0] = line[0] - ray;
		line[1] = line[1] + ray;

		Vec3f clip;
#if 0
		if(GetMapIntersection(&g_hmap, line, &clip))
#else
		if(FastMapIntersect(&g_hmap, line, &clip))
#endif
			py->zoom = oldzoom;
		//else
		//	CalcMapView();
	}
}


void ZoomIn()
{
	MouseWheel(-1);
}

void ZoomOut()
{
	MouseWheel(1);
}

void MapKeys()
{
#if 0
	AssignKey(SDLK_ESCAPE, &Escape, NULL);
	AssignLButton(&MouseLeftButtonDown, &MouseLeftButtonUp);
	AssignRButton(NULL, &MouseRightButtonUp);
#endif

	for(int i=0; i<PLAYERS; i++)
	{
		Player* py = &g_player[i];
		GUI* gui = &py->gui;
		gui->assignmousewheel(&MouseWheel);
		gui->assignmbutton(MouseMidButtonDown, MouseMidButtonUp);
		gui->assignkey(SDL_SCANCODE_R, ZoomOut, NULL);
		gui->assignkey(SDL_SCANCODE_F, ZoomIn, NULL);
		gui->assignkey(SDL_SCANCODE_GRAVE, NULL, ToggleConsole);
	}

	/*
	int key;
	void (*down)();
	void (*up)();
	ifstream f("keymap.ini");
	std::string line;
	char keystr[32];
	char actstr[32];

	while(!f.eof())
	{
		key = -1;
		down = NULL;
		up = NULL;
		strcpy(keystr, "");
		strcpy(actstr, "");

		getline(f, line);
		sscanf(line.c_str(), "%s %s", keystr, actstr);

		if(stricmp(keystr, "SDLK_ESCAPE") == 0)			key = SDLK_ESCAPE;
		else if(stricmp(keystr, "SDLK_SHIFT") == 0)		key = SDLK_SHIFT;
		else if(stricmp(keystr, "SDLK_CONTROL") == 0)		key = SDLK_CONTROL;
		else if(stricmp(keystr, "SDLK_SPACE") == 0)		key = SDLK_SPACE;
		else if(stricmp(keystr, "MouseLButton") == 0)	key = -2;
		else if(stricmp(keystr, "F1") == 0)				key = SDLK_F1;
		else if(stricmp(keystr, "F2") == 0)				key = SDLK_F2;
		else if(stricmp(keystr, "F3") == 0)				key = SDLK_F3;
		else if(stricmp(keystr, "F4") == 0)				key = SDLK_F4;
		else if(stricmp(keystr, "F5") == 0)				key = SDLK_F5;
		else if(stricmp(keystr, "F6") == 0)				key = SDLK_F6;
		else if(stricmp(keystr, "F7") == 0)				key = SDLK_F7;
		else if(stricmp(keystr, "F8") == 0)				key = SDLK_F8;
		else if(stricmp(keystr, "F9") == 0)				key = SDLK_F9;
		else if(stricmp(keystr, "F10") == 0)			key = SDLK_F10;
		else if(stricmp(keystr, "F11") == 0)			key = SDLK_F11;
		else if(stricmp(keystr, "F12") == 0)			key = SDLK_F12;
		else if(stricmp(keystr, "'A'") == 0)			key = 'A';
		else if(stricmp(keystr, "'B'") == 0)			key = 'B';
		else if(stricmp(keystr, "'C'") == 0)			key = 'C';
		else if(stricmp(keystr, "'D'") == 0)			key = 'D';
		else if(stricmp(keystr, "'E'") == 0)			key = 'E';
		else if(stricmp(keystr, "'F'") == 0)			key = 'F';
		else if(stricmp(keystr, "'G'") == 0)			key = 'G';
		else if(stricmp(keystr, "'H'") == 0)			key = 'H';
		else if(stricmp(keystr, "'I'") == 0)			key = 'I';
		else if(stricmp(keystr, "'J'") == 0)			key = 'J';
		else if(stricmp(keystr, "'K'") == 0)			key = 'K';
		else if(stricmp(keystr, "'L'") == 0)			key = 'L';
		else if(stricmp(keystr, "'M'") == 0)			key = 'M';
		else if(stricmp(keystr, "'N'") == 0)			key = 'N';
		else if(stricmp(keystr, "'O'") == 0)			key = 'O';
		else if(stricmp(keystr, "'P'") == 0)			key = 'P';
		else if(stricmp(keystr, "'Q'") == 0)			key = 'Q';
		else if(stricmp(keystr, "'R'") == 0)			key = 'R';
		else if(stricmp(keystr, "'S'") == 0)			key = 'S';
		else if(stricmp(keystr, "'T'") == 0)			key = 'T';
		else if(stricmp(keystr, "'U'") == 0)			key = 'U';
		else if(stricmp(keystr, "'V'") == 0)			key = 'V';
		else if(stricmp(keystr, "'W'") == 0)			key = 'W';
		else if(stricmp(keystr, "'X'") == 0)			key = 'X';
		else if(stricmp(keystr, "'Y'") == 0)			key = 'Y';
		else if(stricmp(keystr, "'Z'") == 0)			key = 'Z';
		else if(stricmp(keystr, "'0'") == 0)			key = '0';
		else if(stricmp(keystr, "'1'") == 0)			key = '1';
		else if(stricmp(keystr, "'2'") == 0)			key = '2';
		else if(stricmp(keystr, "'3'") == 0)			key = '3';
		else if(stricmp(keystr, "'4'") == 0)			key = '4';
		else if(stricmp(keystr, "'5'") == 0)			key = '5';
		else if(stricmp(keystr, "'6'") == 0)			key = '6';
		else if(stricmp(keystr, "'7'") == 0)			key = '7';
		else if(stricmp(keystr, "'8'") == 0)			key = '8';
		else if(stricmp(keystr, "'9'") == 0)			key = '9';

		if(key == -1)
		{
			g_log<<"Unknown input: "<<keystr<<std::endl;
			continue;
		}

		if(stricmp(actstr, "Escape();") == 0)				{	down = &Escape;			up = NULL;			}
		else if(stricmp(actstr, "Forward();") == 0)			{	down = &Forward;		up = NULL;			}
		else if(stricmp(actstr, "Left();") == 0)			{	down = &Left;			up = NULL;			}
		else if(stricmp(actstr, "Right();") == 0)			{	down = &Right;			up = NULL;			}
		else if(stricmp(actstr, "Back();") == 0)			{	down = &Back;			up = NULL;			}

		if(down == NULL)		g_log<<"Unknown action: "<<actstr<<std::endl;
		else if(key == -2)		AssignLButton(down, up);
		else					AssignKey(key, down, up);
	}
	*/
}
