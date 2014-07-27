#include "window.h"
#include "texture.h"
#include "render/shader.h"
#include "gui/gui.h"
#include "gui/font.h"
#include "render/shadow.h"
#include "math/3dmath.h"
#include "gui/cursor.h"
#include "sim/player.h"

bool g_quit = false;
double g_drawfrinterval = 0.0f;
bool g_fullscreen = false;
Resolution g_selectedRes;
std::vector<Resolution> g_resolution;
std::vector<int> g_bpps;
#if 0
double g_currentTime;
double g_lastTime = 0.0f;		// This will hold the time from the last frame
double g_framesPerSecond = 0.0f;		// This will store our fps
#endif
double g_instantdrawfps = 0.0f;
long long g_lasttime = GetTickCount();

void AddRes(int w, int h)
{
	Resolution r;
	r.width = w;
	r.height = h;
	g_resolution.push_back(r);
}

void EnumerateDisplay()
{
#ifdef PLATFORM_WIN
	DEVMODE dm;
	int index=0;
	while(0 != EnumDisplaySettings(NULL, index++, &dm))
	{
		Resolution r;
		r.width = dm.dmPelsWidth;
		r.height = dm.dmPelsHeight;

		bool found = false;

		for(int i=0; i<g_resolution.size(); i++)
		{
			if(g_resolution[i].width == r.width && g_resolution[i].height == r.height)
			{
				found = true;
				break;
			}
		}

		if(!found)
			g_resolution.push_back(r);

		found = false;

		int bpp = dm.dmBitsPerPel;

		for(int i=0; i<g_bpps.size(); i++)
		{
			if(g_bpps[i] == bpp)
			{
				found = true;
				break;
			}
		}

		if(!found)
			g_bpps.push_back(bpp);
	}
#endif // PLATFORM_WIN
}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;

	glViewport(0, 0, width, height);

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	if(py->width != width || py->height != height)
	{
		py->width = width;
		py->height = height;

		//if(g_fullscreen)
		//Reload();
		//loadtex();
		gui->reframe();
	}
}

void CalcDrawRate()
{
	static unsigned int frametime = 0.0f;				// This stores the last frame's time
	static int framecounter = 0;
	static unsigned int lasttime;

	// Get the current time in seconds
	unsigned int currtime = timeGetTime();

	// We added a small value to the frame interval to account for some video
	// cards (Radeon's) with fast computers falling through the floor without it.

	// Here we store the elapsed time between the current and last frame,
	// then keep the current frame in our static variable for the next frame.
	g_drawfrinterval = (currtime - frametime) / 1000.0f;	// + 0.005f;

	//g_instantdrawfps = 1.0f / (g_currentTime - frameTime);
	//g_instantdrawfps = 1.0f / g_drawfrinterval;

	frametime = currtime;

	// Increase the frame counter
	++framecounter;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if( currtime - lasttime > 1000 )
	{
		g_instantdrawfps = framecounter;

		// Here we set the lastTime to the currentTime
		lasttime = currtime;

		// Reset the frames per second
		framecounter = 0;
	}
}

bool DrawNextFrame(int desiredFrameRate)
{
	static long long lastTime = GetTickCount64();
	static long long elapsedTime = 0;

	long long currentTime = GetTickCount64(); // Get the time (milliseconds = seconds * .001)
	long long deltaTime = currentTime - lastTime; // Get the slice of time
	int desiredFPS = 1000 / (float)desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		elapsedTime -= desiredFPS; // Adjust the elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
	/*
	long long currentTime = GetTickCount();
	float desiredFPMS = 1000.0f/(float)desiredFrameRate;
	int deltaTime = currentTime - g_lasttime;

	if(deltaTime >= desiredFPMS)
	{
		g_lasttime = currentTime;
		return true;
	}

	return false;*/
}

bool InitWindow()
{
	g_log<<"Renderer1: "<<(char*)glGetString(GL_RENDERER)<<std::endl;
	g_log<<"GL_VERSION1 = "<<(char*)glGetString(GL_VERSION)<<std::endl;
	g_log.flush();

	char path[MAX_PATH+1];
	FullPath("gui/trigear-64x64.png", path);
	LoadedTex* pixels = LoadPNG(path);

	if(!pixels)
	{
		ErrorMessage("Error", "Couldn't load icon");
	}

	SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(pixels->data, pixels->sizeX, pixels->sizeY, pixels->channels*8, pixels->channels*pixels->sizeX, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

	delete pixels;

	if(!surf)
	{
		char message[256];
		sprintf(message, "Couldn't create icon: %s", SDL_GetError());
		ErrorMessage("Error", message);
	}

	// The icon is attached to the window pointer
	SDL_SetWindowIcon(g_window, surf);

	// ...and the surface containing the icon pixel data is no longer required.
	SDL_FreeSurface(surf);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	InitGLSL();
	InitShadows();
	LoadFonts();

	return true;
}

void DestroyWindow(const char* title)
{
	FreeTextures();
	ReleaseShaders();

	// Close and destroy the window
	SDL_GL_DeleteContext(g_glcontext);
#if 0
	SDL_DestroyRenderer(g_renderer);
#endif
	SDL_DestroyWindow(g_window);
}

bool MakeWindow(const char* title)
{

	g_log<<"samw0"<<std::endl;
	g_log.flush();

	//g_log<<"GL_VERSION: "<<(char*)glGetString(GL_VERSION)<<std::endl;
	//g_log.flush();

	g_log<<"sa"<<std::endl;
	g_log.flush();

	// Request compatibility because GLEW doesn't play well with core contexts.
#if 1
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	unsigned int flags;
	int startx;
	int starty;

	if(g_fullscreen)
	{
		startx = SDL_WINDOWPOS_UNDEFINED;
		starty = SDL_WINDOWPOS_UNDEFINED;
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
	}
	else
	{
#if 0
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		int screenw = current.w;
		int screenh = current.h;

		startx = screenw/2 - g_selectedRes.width/2;
		starty = screenh/2 - g_selectedRes.height/2;
#else
		startx = SDL_WINDOWPOS_UNDEFINED;
		starty = SDL_WINDOWPOS_UNDEFINED;
#endif
		flags = SDL_WINDOW_OPENGL;
	}

	// Create an application window with the following settings:
	g_window = SDL_CreateWindow(
				   title,                  // window title
				   startx,           // initial x position
				   starty,           // initial y position
				   g_selectedRes.width,                               // width, in pixels
				   g_selectedRes.height,                               // height, in pixels
				   flags                  // flags - see below
			   );

	// Check that the window was successfully made
	if (g_window == NULL)
	{
		// In the event that the window could not be made...
		char msg[256];
		sprintf(msg, "Could not create window: %s\n", SDL_GetError());
		ErrorMessage("Error", msg);
		return false;
	}

#if 0
	g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (g_renderer == NULL)
	{
		// In the event that the window could not be made...
		char msg[256];
		sprintf(msg, "Could not create renderer: %s\n", SDL_GetError());
		ErrorMessage("Error", msg);
		return false;
	}
#endif
	//SDL_GL_SetSwapInterval(0);

	g_glcontext = SDL_GL_CreateContext(g_window);

	g_log<<"GL_VERSION: "<<glGetString(GL_VERSION)<<std::endl;
	g_log.flush();

	if(!g_glcontext)
	{
		DestroyWindow(title);
		ErrorMessage("Error", "Couldn't create GL context");
		return false;
	}

	SDL_GL_MakeCurrent(g_window, g_glcontext);

	SDL_GL_SetSwapInterval(0);
	//SDL_Delay(7000);
	//SDL_Delay(7000);

	Vec2i winsz;
	SDL_GetWindowSize(g_window, &winsz.x, &winsz.y);
	Resize(winsz.x, winsz.y);

	if(!InitWindow())
	{
		DestroyWindow(title);
		ErrorMessage("Error", "Initialization failed");
		return false;
	}

	return true;
}
