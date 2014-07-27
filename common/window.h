#ifndef WINDOW_H
#define WINDOW_H

#include "platform.h"
#include "math/3dmath.h"
#include "math/vec2i.h"

#define INI_WIDTH			800
#define INI_HEIGHT			600
#define INI_BPP				32
#define DRAW_FRAME_RATE			30
#define MIN_DISTANCE		1.0f
//#define MAX_DISTANCE		(100 * 1000 * 10.0f) //10km
//#define MAX_DISTANCE		(10 * 1000 * 10.0f) //10km
#define MAX_DISTANCE		(5.0f * 1000.0f * 10.0f) //10km
#define FIELD_OF_VIEW		45.0f
//#define PROJ_LEFT			(-16*4)
#define PROJ_RIGHT			600 //(500)	//(30*12.5f)
//#define PROJ_TOP			(16*4)
//#define PROJ_BOTTOM			(-16*4)
//#define INI_ZOOM			0.025f
//#define MIN_ZOOM		0.1f
//#define MIN_ZOOM		0.025f
#define MIN_ZOOM		0.005f
//#define MAX_ZOOM		0.7f
#define MAX_ZOOM		0.7f
//#define MAX_ZOOM		0.1f
#define INI_ZOOM			MIN_ZOOM

extern double g_drawfrinterval;
extern bool g_quit;
extern bool g_active;
extern bool g_fullscreen;
#if 0
extern double g_currentTime;
extern double g_lastTime;
extern double g_framesPerSecond;
#endif
extern double g_instantdrawfps;

struct Resolution
{
	int width;
	int height;
};

extern Resolution g_selectedRes;
extern std::vector<Resolution> g_resolution;
extern std::vector<int> g_bpps;

void AddRes(int w, int h);
void CalcDrawRate();
bool DrawNextFrame(int desiredFrameRate);
void EnumerateDisplay();
void Resize(int width, int height);
void DestroyWindow(const char* title);
bool MakeWindow(const char* title);

#endif
