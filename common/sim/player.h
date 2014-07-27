#ifndef PLAYER_H
#define PLAYER_H

#include "resources.h"
#include "../gui/richtext.h"
#include "../gui/gui.h"
#include "../math/camera.h"
#include "selection.h"
#include "../../libs/objectscript/objectscript.h"

struct PlayerColor
{
	unsigned char color[3];
	char name[32];
};

#define PLAYER_COLORS	48

extern PlayerColor g_pycols[PLAYER_COLORS];

class Player
{
public:
	bool on;
	bool ai;

	int local[RESOURCES];	// used just for counting; cannot be used
	int global[RESOURCES];

	float colorcode[4];
	RichText name;

	Camera camera;
	GUI gui;
	int currw;
	int currh;
	int width;
	int height;
	int bpp;
	Vec2i mouse;
	Vec2i mousestart;
	bool keyintercepted;
	bool keys[SDL_NUM_SCANCODES];
	bool mousekeys[5];
	float zoom;
	bool mouseout;
	bool moved;
	bool canplace;
	int bpcol;
	int build;
	Vec3f vdrag[2];
	Camera bpcam;
	int bptype;
	float bpyaw;
	Selection sel;
	bool mouseoveraction;
	int curst;	//cursor state
	int kbfocus;	//keyboad focus counter

	/*
	Determines if the cursor is over an actionable widget, like a drop-down selector.
	If it is, we don't want to scroll if the mouse is at the edge of the screen because
	the user is trying to do something.
	*/
	//bool py->mouseoveraction = false;

	Player();
	~Player();
};

//#define PLAYERS 32
#define PLAYERS ARRSZ(g_pycols)

extern Player g_player[PLAYERS];
extern int g_localP;
extern int g_curP;
extern int g_playerm;


void DefP(int ID, float red, float green, float blue, float alpha, RichText name);
void DrawPy();
void Bankrupt(int player, const char* reason);

#endif
