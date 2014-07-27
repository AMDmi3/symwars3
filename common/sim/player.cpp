#include "player.h"
#include "resources.h"
#include "../render/model.h"
#include "../render/heightmap.h"
#include "../sim/build.h"
#include "../sim/building.h"
#include "../gui/cursor.h"
#include "../script/console.h"

PlayerColor g_pycols[PLAYER_COLORS] =
{
	{{0x7e, 0x1e, 0x9c}, "Purple"},
	{{0x15, 0xb0, 0x1a}, "Green"},
	{{0x03, 0x43, 0xdf}, "Blue"},
	{{0xff, 0x81, 0xc0}, "Pink"},
	{{0x65, 0x37, 0x00}, "Brown"},
	{{0xe5, 0x00, 0x00}, "Red"},
	{{0x95, 0xd0, 0xfc}, "Light Blue"},
	{{0x02, 0x93, 0x86}, "Teal"},
	{{0xf9, 0x73, 0x06}, "Orange"},
	{{0x96, 0xf9, 0x7b}, "Light Green"},
	{{0xc2, 0x00, 0x78}, "Magenta"},
	{{0xff, 0xff, 0x14}, "Yellow"},
	{{0x75, 0xbb, 0xfd}, "Sky Blue"},
	{{0x92, 0x95, 0x91}, "Grey"},
	{{0x89, 0xfe, 0x05}, "Lime Green"},
	{{0xbf, 0x77, 0xf6}, "Light Purple"},
	{{0x9a, 0x0e, 0xea}, "Violet"},
	{{0x33, 0x35, 0x00}, "Dark Green"},
	{{0x06, 0xc2, 0xac}, "Turquoise"},
	{{0xc7, 0x9f, 0xef}, "Lavender"},
	{{0x00, 0x03, 0x5b}, "Dark Blue"},
	{{0xd1, 0xb2, 0x6f}, "Tan"},
	{{0x00, 0xff, 0xff}, "Cyan"},
	{{0x13, 0xea, 0xc9}, "Aqua"},
	{{0x06, 0x47, 0x0c}, "Forest Green"},
	{{0xae, 0x71, 0x81}, "Mauve"},
	{{0x35, 0x06, 0x3e}, "Dark Purple"},
	{{0x01, 0xff, 0x07}, "Bright Green"},
	{{0x65, 0x00, 0x21}, "Maroon"},
	{{0x6e, 0x75, 0x0e}, "Olive"},
	{{0xff, 0x79, 0x6c}, "Salmon"},
	{{0xe6, 0xda, 0xa6}, "Beige"},
	{{0x05, 0x04, 0xaa}, "Royal Blue"},
	{{0x00, 0x11, 0x46}, "Navy Blue"},
	{{0xce, 0xa2, 0xfd}, "Lilac"},
	{{0x00, 0x00, 0x00}, "Black"},
	{{0xff, 0x02, 0x8d}, "Hot Pink"},
	{{0xad, 0x81, 0x50}, "Light Brown"},
	{{0xc7, 0xfd, 0xb5}, "Pale Green"},
	{{0xff, 0xb0, 0x7c}, "Peach"},
	{{0x67, 0x7a, 0x04}, "Olive Green"},
	{{0xcb, 0x41, 0x6b}, "Dark Pink"},
	{{0x8e, 0x82, 0xfe}, "Periwinkle"},
	{{0x53, 0xfc, 0xa1}, "Sea Green"},
	{{0xaa, 0xff, 0x32}, "Lime"},
	{{0x38, 0x02, 0x82}, "Indigo"},
	{{0xce, 0xb3, 0x01}, "Mustard"},
	{{0xff, 0xd1, 0xdf}, "Light Pink"}
};

Player g_player[PLAYERS];
int g_localP = 0;
int g_curP = g_localP;
int g_playerm;

Player::Player()
{
	Zero(local);
	Zero(global);

	global[RES_MINERALS] = 10000;
	//global[RES_LABOUR] = 10000;

	on = false;
	zoom = INI_ZOOM;
	width = INI_WIDTH;
	height = INI_HEIGHT;
	bpp = INI_BPP;
	keyintercepted = false;
	mouseout = false;
	moved = false;
	canplace = false;
	bpcol = -1;
	build = BUILDING_NONE;
	bptype = -1;
	bpyaw = 0;
	mouseoveraction = false;
	curst = CU_DEFAULT;
	kbfocus = 0;
}

Player::~Player()
{
}

void DefP(int ID, float red, float green, float blue, float alpha, RichText name)
{
	Player* py = &g_player[ID];
	py->colorcode[0] = red;
	py->colorcode[1] = green;
	py->colorcode[2] = blue;
	py->colorcode[3] = alpha;
	py->name = name;
}

void DrawPy()
{
	Model* m = &g_model[g_playerm];

	m->draw(0, Vec3f(TILE_SIZE*10, TILE_SIZE*5, TILE_SIZE*10), 30);
}

void Bankrupt(int player, const char* reason)
{
	Player* p = &g_player[player];
    
	if(player == g_localP)
	{
		RichText lm(UString("You've gone bankrupt"));
		SubmitConsole(&lm);
	}
	else //if(p->activity != ACTIVITY_NONE)
	{
		//LogTransx(player, 0.0f, "BANKRUPT");
        
		char msg[256];
		sprintf(msg, "%s has gone bankrupt", g_player[player].name.rawstr());
        
		char add[64];
        
		if(reason[0] != '\0')
			sprintf(add, " (reason: %s).", reason);
		else
			sprintf(add, ".");
        
		strcat(msg, add);
		
		RichText lm;
		lm.m_part.push_back(RichTextP(UString(msg)));
		SubmitConsole(&lm);
	}
}