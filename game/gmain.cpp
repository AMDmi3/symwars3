#include "gmain.h"
#include "../common/gui/gui.h"
#include "keymap.h"
#include "../common/render/shader.h"
#include "gres.h"
#include "../common/gui/font.h"
#include "../common/texture.h"
#include "../common/render/model.h"
#include "../common/math/frustum.h"
#include "gui/ggui.h"
#include "../common/gui/gui.h"
#include "../common/debug.h"
#include "../common/render/heightmap.h"
#include "../common/math/camera.h"
#include "../common/render/shadow.h"
#include "../common/window.h"
#include "../common/utils.h"
#include "../common/sim/sim.h"
#include "../common/math/hmapmath.h"
#include "../common/sim/unit.h"
#include "../common/sim/building.h"
#include "../common/sim/build.h"
#include "../common/sim/buildingtype.h"
#include "../common/render/foliage.h"
#include "../common/render/water.h"
#include "../common/sim/road.h"
#include "../common/sim/crpipe.h"
#include "../common/sim/powl.h"
#include "../common/sim/deposit.h"
#include "../common/sim/selection.h"
#include "../common/sim/player.h"
#include "../common/sim/order.h"
#include "../common/render/transaction.h"
#include "../common/path/collidertile.h"
#include "../common/path/pathdebug.h"
#include "gui/playgui.h"
#include "../common/gui/widgets/spez/bottompanel.h"
#include "../common/texture.h"
#include "../common/render/skybox.h"
#include "../common/script/script.h"
#include "../common/ai/ai.h"

int g_mode = APPMODE_LOADING;

double g_instantupdfps = 0;
double g_updfrinterval = 0;

//static long long g_lasttime = GetTickCount();

void SkipLogo()
{
	g_mode = APPMODE_LOADING;
	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	gui->closeall();
	gui->open("loading");
}

void UpdateLogo()
{
	static int stage = 0;

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	if(stage < 60)
	{
		float a = (float)stage / 60.0f;
		gui->get("logo")->get("logo")->m_rgba[3] = a;
	}
	else if(stage < 120)
	{
		float a = 1.0f - (float)(stage-60) / 60.0f;
		gui->get("logo")->get("logo")->m_rgba[3] = a;
	}
	else
		SkipLogo();

	stage++;
}

void UpdateLoading()
{
	static int stage = 0;

	switch(stage)
	{
	case 0:
		if(!Load1Model()) stage++;
		break;
	case 1:
		if(!Load1Texture())
		{
			g_mode = APPMODE_MENU;
			//g_mode = APPMODE_PLAY;
			Click_NewGame();
			//Click_OpenEditor();
		}
		break;
	}
}

int g_reStage = 0;
void UpdateReloading()
{
	switch(g_reStage)
	{
	case 0:
		if(!Load1Texture())
		{
			g_mode = APPMODE_MENU;
		}
		break;
	}
}

void CalcUpdRate()
{
	static unsigned int frametime = 0;				// This stores the last frame's time
	static int framecounter = 0;
	static unsigned int lasttime;

	// Get the current time in seconds
	unsigned int currtime = timeGetTime();

	// We added a small value to the frame interval to account for some video
	// cards (Radeon's) with fast computers falling through the floor without it.

	// Here we store the elapsed time between the current and last frame,
	// then keep the current frame in our static variable for the next frame.
	g_updfrinterval = (currtime - frametime) / 1000.0f;	// + 0.005f;

	//g_instantdrawfps = 1.0f / (g_currentTime - frameTime);
	//g_instantdrawfps = 1.0f / g_drawfrinterval;

	frametime = currtime;

	// Increase the frame counter
	++framecounter;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if( currtime - lasttime > 1000 )
	{
		g_instantupdfps = framecounter;

		// Here we set the lastTime to the currentTime
		lasttime = currtime;

		// Reset the frames per second
		framecounter = 0;
	}
}

bool UpdNextFrame(int desiredFrameRate)
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

void UpdSimState()
{
	g_simframe ++;

	UpdateAI();
	StartTimer(TIMER_UPDATEUNITS);
	UpdUnits();
	StopTimer(TIMER_UPDATEUNITS);
	StartTimer(TIMER_UPDATEBUILDINGS);
	UpdBls();
	StopTimer(TIMER_UPDATEBUILDINGS);
}

void UpdateEditor()
{
#if 0
	UpdateFPS();
#endif
}

void Update()
{
	if(g_mode == APPMODE_LOGO)
		UpdateLogo();
	//else if(g_mode == APPMODE_INTRO)
	//	UpdateIntro();
	else if(g_mode == APPMODE_LOADING)
		UpdateLoading();
	else if(g_mode == APPMODE_RELOADING)
		UpdateReloading();
	else if(g_mode == APPMODE_PLAY)
		UpdSimState();
	else if(g_mode == APPMODE_EDITOR)
		UpdateEditor();
}

void DrawScene(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float lightpos[3], float lightdir[3])
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

#if 1
	Matrix mvpmat;
	mvpmat.set(projection.m_matrix);
	mvpmat.postmult(viewmat);
	g_cammvp = mvpmat;

	g_frustum.construct(projection.m_matrix, viewmat.m_matrix);

	UseShadow(SHADER_SKYBOX, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	DrawSkyBox(c->zoompos());
	//DrawSkyBox(Vec3f(0,0,0));
	EndS();
	CheckGLError(__FILE__, __LINE__);

	StartTimer(TIMER_DRAWMAP);
#if 1
	UseShadow(SHADER_MAPTILES, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glUniform1i(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 8);
	g_hmap.draw();
	EndS();
#endif
	StopTimer(TIMER_DRAWMAP);
	CheckGLError(__FILE__, __LINE__);

	StartTimer(TIMER_DRAWRIM);
#if 1
	UseShadow(SHADER_RIM, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	CheckGLError(__FILE__, __LINE__);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glUniform1i(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 8);
	CheckGLError(__FILE__, __LINE__);
	//g_hmap.drawrim();
	CheckGLError(__FILE__, __LINE__);
	EndS();
#endif
	StopTimer(TIMER_DRAWRIM);
	CheckGLError(__FILE__, __LINE__);

	StartTimer(TIMER_DRAWWATER);
#if 1
	UseShadow(SHADER_WATER, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glUniform1i(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 4);
	//DrawWater3();
	EndS();
#endif
	StopTimer(TIMER_DRAWWATER);
	CheckGLError(__FILE__, __LINE__);

#if 1
	UseShadow(SHADER_OWNED, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	//UseShadow(SHADER_UNIT, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glUniform1i(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 5);
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_OWNCOLOR], 1, 0, 0, 1);
	//DrawPy();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	StartTimer(TIMER_DRAWBL);
	DrawBl();
	StopTimer(TIMER_DRAWBL);
	StartTimer(TIMER_DRAWROADS);
	DrawCo(CONDUIT_ROAD);
	StopTimer(TIMER_DRAWROADS);
	StartTimer(TIMER_DRAWCRPIPES);
	DrawCo(CONDUIT_CRPIPE);
	StopTimer(TIMER_DRAWCRPIPES);
	StartTimer(TIMER_DRAWPOWLS);
	DrawCo(CONDUIT_POWL);
	StopTimer(TIMER_DRAWPOWLS);
	DrawSBuild();
	EndS();
	CheckGLError(__FILE__, __LINE__);
#endif

	StartTimer(TIMER_DRAWUNITS);
#if 1
	UseShadow(SHADER_UNIT, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glUniform1i(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 5);
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_OWNCOLOR], 1, 0, 0, 1);
	DrawUnits();
	EndS();
#endif
	StopTimer(TIMER_DRAWUNITS);
	CheckGLError(__FILE__, __LINE__);

#if 1
	StartTimer(TIMER_DRAWFOLIAGE);
	UseShadow(SHADER_FOLIAGE, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glUniform1i(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 5);
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	//DrawFoliage(c->zoompos(), c->m_up, c->m_strafe);
	EndS();
	StopTimer(TIMER_DRAWFOLIAGE);
#endif

	CheckGLError(__FILE__, __LINE__);

#if 0
	UseShadow(SHADER_BORDERS, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	//glActiveTexture(GL_TEXTURE8);
	//glBindTexture(GL_TEXTURE_2D, g_depth);
	//glUniform1i(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 8);
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 1, 1, 1, 0.5f);
	DrawBorderLines();
#endif

#if 1
	UseShadow(SHADER_COLOR3D, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	DrawGrid();
#if 0
	DrawUnitSquares();
	DrawPaths();
	DrawSteps();
	DrawVelocities();
#endif
	EndS();
#endif

	DrawSel(&projection, &modelmat, &viewmat);
	CheckGLError(__FILE__, __LINE__);
	DrawOrders(&projection, &modelmat, &viewmat);
	CheckGLError(__FILE__, __LINE__);

#if 1
	//UseShadow(SHADER_BILLBOARD, projection, viewmat, modelmat, modelviewinv, lightpos, lightdir);
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, g_depth);
	//glUniform1i(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 4);
	UseS(SHADER_BILLBOARD);
	Shader* s = &g_shader[g_curS];
	glUniformMatrix4fv(s->m_slot[SSLOT_PROJECTION], 1, 0, projection.m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, 0, modelmat.m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_VIEWMAT], 1, 0, viewmat.m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_MVP], 1, 0, mvpmat.m_matrix);
	//glUniformMatrix4fv(s->m_slot[SSLOT_NORMALMAT], 1, 0, modelviewinv.m_matrix);
	//glUniformMatrix4fv(s->m_slot[SSLOT_INVMODLVIEWMAT], 1, 0, modelviewinv.m_matrix);
	glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	CheckGLError(__FILE__, __LINE__);
	UpdateParticles();
	StartTimer(TIMER_SORTPARTICLES);
	SortBillboards();
	StopTimer(TIMER_SORTPARTICLES);
	DrawBillboards();
	EndS();
#endif

#if 1
	StartTimer(TIMER_DRAWGUI);
	CheckGLError(__FILE__, __LINE__);
	Ortho(py->width, py->height, 1, 1, 1, 1);
	glDisable(GL_DEPTH_TEST);
	//DrawDeposits(projection, viewmat);
	DrawTransactions(mvpmat);
	DrawBReason(&mvpmat, py->width, py->height, true);
	glEnable(GL_DEPTH_TEST);
	EndS();
	StopTimer(TIMER_DRAWGUI);
#endif
#endif

#if 0
	CheckGLError(__FILE__, __LINE__);
	Ortho(py->width, py->height, 1, 1, 1, 1);
	glDisable(GL_DEPTH_TEST);
	FoliageT* t = &g_foliageT[FOLIAGE_TREE1];
	Model* m = &g_model[t->model];
	for(int i=0; i<30000; i++)
	{
		m->usetex();
		Texture* tex = &g_texture[m->m_diffusem];

		int x = rand()%py->width;
		int y = rand()%py->height;

		DrawImage(tex->texname, x, y, x+2, y+4, 0, 0, 1, 1);
	}
	glEnable(GL_DEPTH_TEST);
	EndS();
#endif
}

void DrawSceneDepth()
{
	StartTimer(TIMER_DRAWSCENEDEPTH);

#if 1
	Player* py = &g_player[g_curP];

	CheckGLError(__FILE__, __LINE__);
	//if(rand()%2 == 1)
	StartTimer(TIMER_DRAWMAPDEPTH);
	g_hmap.draw2();
	StopTimer(TIMER_DRAWMAPDEPTH);
	CheckGLError(__FILE__, __LINE__);
	//g_hmap.drawrim();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CheckGLError(__FILE__, __LINE__);
	//g_hmap.draw2();
	DrawBl();
	CheckGLError(__FILE__, __LINE__);
	for(char i=0; i<CONDUIT_TYPES; i++)
		DrawCo(i);
	CheckGLError(__FILE__, __LINE__);
	StartTimer(TIMER_DRAWUNITSDEPTH);
	DrawUnits();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	StopTimer(TIMER_DRAWUNITSDEPTH);
	CheckGLError(__FILE__, __LINE__);
	//DrawPy();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CheckGLError(__FILE__, __LINE__);
#if 1
	//DrawFoliage(g_lightpos, Vec3f(0,1,0), Cross(Vec3f(0,1,0), Normalize(g_lighteye - g_lightpos)));
	CheckGLError(__FILE__, __LINE__);
#endif
#endif

	StopTimer(TIMER_DRAWSCENEDEPTH);
}

void Draw()
{
	StartTimer(TIMER_DRAWSETUP);

	CheckGLError(__FILE__, __LINE__);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CheckGLError(__FILE__, __LINE__);

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	Camera* c = &py->camera;

	StopTimer(TIMER_DRAWSETUP);

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

#if 2
	if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
	{
		StartTimer(TIMER_DRAWSETUP);

		float aspect = fabsf((float)py->width / (float)py->height);
		Matrix projection = PerspProj(FIELD_OF_VIEW, aspect, MIN_DISTANCE, MAX_DISTANCE/py->zoom);
		//Matrix projection = OrthoProj(-PROJ_RIGHT*aspect/py->zoom, PROJ_RIGHT*aspect/py->zoom, PROJ_RIGHT/py->zoom, -PROJ_RIGHT/py->zoom, MIN_DISTANCE, MAX_DISTANCE);

		g_camproj = projection;

		Vec3f focusvec = c->m_view;
		Vec3f posvec = c->zoompos();
		Vec3f upvec = c->m_up;

		Matrix viewmat = LookAt(posvec.x, posvec.y, posvec.z, focusvec.x, focusvec.y, focusvec.z, upvec.x, upvec.y, upvec.z);

		g_camview = viewmat;

		Matrix modelview;
		Matrix modelmat;
		float translation[] = {0, 0, 0};
		modelview.translation(translation);
		//modelmat.translation(translation);
		modelmat.reset();
		modelview.postmult(viewmat);

		g_cammodelview = modelview;

		Matrix mvpmat;
		mvpmat.set(projection.m_matrix);
		mvpmat.postmult(viewmat);

		Vec3f focus;
		Vec3f vLine[2];
		Vec3f ray = Normalize(c->m_view - posvec);
		Vec3f onnear = posvec;	//OnNear(py->width/2, py->height/2);
		vLine[0] = onnear;
		vLine[1] = onnear + (ray * 10000000.0f);
		//if(!GetMapIntersection(&g_hmap, vLine, &focus))
		//if(!FastMapIntersect(&g_hmap, vLine, &focus))
		//if(!GetMapIntersection(&g_hmap, vLine, &focus))
		//GetMapIntersection2(&g_hmap, vLine, &focus);
		//if(!GetMapIntersection2(&g_hmap, vLine, &focus))
		//GetMapIntersection(&g_hmap, vLine, &focus);
		focus = c->m_view;
		CheckGLError(__FILE__, __LINE__);

		StopTimer(TIMER_DRAWSETUP);

		RenderToShadowMap(projection, viewmat, modelmat, focus, focus + g_lightoff / py->zoom, DrawSceneDepth);
		CheckGLError(__FILE__, __LINE__);
		RenderShadowedScene(projection, viewmat, modelmat, modelview, DrawScene);
		CheckGLError(__FILE__, __LINE__);
	}
	CheckGLError(__FILE__, __LINE__);
#endif

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

	StartTimer(TIMER_DRAWGUI);
	gui->frameupd();

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

	CheckGLError(__FILE__, __LINE__);
	gui->draw();
	StopTimer(TIMER_DRAWGUI);

#if 0
	for(int i=0; i<30; i++)
	{
		int x = rand()%py->width;
		int y = rand()%py->height;

		Blit(blittex, &blitscreen, Vec2i(x,y));
	}

	glDrawPixels(blitscreen.sizeX, blitscreen.sizeY, GL_RGB, GL_BYTE, blitscreen.data);
#endif

	CheckGLError(__FILE__, __LINE__);
	Ortho(py->width, py->height, 1, 1, 1, 1);
	CheckGLError(__FILE__, __LINE__);
	glDisable(GL_DEPTH_TEST);
	CheckGLError(__FILE__, __LINE__);

#if 0
	RichText uni;

	for(int i=16000; i<19000; i++)
		//for(int i=0; i<3000; i++)
	{
		uni.m_part.push_back(RichTextP(i));
	}

	float color[] = {1,1,1,1};
	DrawBoxShadText(MAINFONT8, 0, 0, py->width, py->height, &uni, color, 0, -1);
#endif

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

#if 1
	char fpsstr[256];
	sprintf(fpsstr, "draw fps: %lf (%lf s/frame), upd fps: %lf (%lf s/frame), zoom: %f, simframe: %lld", g_instantdrawfps, 1.0/g_instantdrawfps, g_instantupdfps, 1.0/g_instantupdfps, py->zoom, g_simframe);
	RichText fpsrstr(fpsstr);
	DrawShadowedText(MAINFONT8, 0, py->height-MINIMAP_SIZE-32-10, &fpsrstr);
	CheckGLError(__FILE__, __LINE__);
	glEnable(GL_DEPTH_TEST);
	EndS();
	CheckGLError(__FILE__, __LINE__);
#endif

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

	SDL_GL_SwapWindow(g_window);

	//CheckNum("post draw");
}

bool OverMinimap()
{
	return false;
}

void Scroll()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	if(py->mouseout)
		return;

	bool moved = false;

	//const Uint8 *keys = SDL_GetKeyboardState(NULL);
	//SDL_BUTTON_LEFT;
	if((!py->keyintercepted && (py->keys[SDL_SCANCODE_UP] || py->keys[SDL_SCANCODE_W])) || (py->mouse.y <= SCROLL_BORDER))
	{
		c->accelerate(CAMERA_SPEED / py->zoom * g_drawfrinterval);
		moved = true;
	}

	if((!py->keyintercepted && (py->keys[SDL_SCANCODE_DOWN] || py->keys[SDL_SCANCODE_S])) || (py->mouse.y >= py->height-SCROLL_BORDER))
	{
		c->accelerate(-CAMERA_SPEED / py->zoom * g_drawfrinterval);
		moved = true;
	}

	if((!py->keyintercepted && (py->keys[SDL_SCANCODE_LEFT] || py->keys[SDL_SCANCODE_A])) || (py->mouse.x <= SCROLL_BORDER))
	{
		c->accelstrafe(-CAMERA_SPEED / py->zoom * g_drawfrinterval);
		moved = true;
	}

	if((!py->keyintercepted && (py->keys[SDL_SCANCODE_RIGHT] || py->keys[SDL_SCANCODE_D])) || (py->mouse.x >= py->width-SCROLL_BORDER))
	{
		c->accelstrafe(CAMERA_SPEED / py->zoom * g_drawfrinterval);
		moved = true;
	}

#if 0
	if(moved)
#endif
	{
#if 0
		if(c->zoompos().x < -g_hmap.m_widthx*TILE_SIZE)
		{
			float d = -g_hmap.m_widthx*TILE_SIZE - c->zoompos().x;
			c->move(Vec3f(d, 0, 0));
		}
		else if(c->zoompos().x > g_hmap.m_widthx*TILE_SIZE)
		{
			float d = c->zoompos().x - g_hmap.m_widthx*TILE_SIZE;
			c->move(Vec3f(-d, 0, 0));
		}

		if(c->zoompos().z < -g_hmap.m_widthz*TILE_SIZE)
		{
			float d = -g_hmap.m_widthz*TILE_SIZE - c->zoompos().z;
			c->move(Vec3f(0, 0, d));
		}
		else if(c->zoompos().z > g_hmap.m_widthz*TILE_SIZE)
		{
			float d = c->zoompos().z - g_hmap.m_widthz*TILE_SIZE;
			c->move(Vec3f(0, 0, -d));
		}
#else

		if(c->m_view.x < 0)
		{
			float d = 0 - c->m_view.x;
			c->move(Vec3f(d, 0, 0));
		}
		else if(c->m_view.x > g_hmap.m_widthx*TILE_SIZE)
		{
			float d = c->m_view.x - g_hmap.m_widthx*TILE_SIZE;
			c->move(Vec3f(-d, 0, 0));
		}

		if(c->m_view.z < 0)
		{
			float d = 0 - c->m_view.z;
			c->move(Vec3f(0, 0, d));
		}
		else if(c->m_view.z > g_hmap.m_widthz*TILE_SIZE)
		{
			float d = c->m_view.z - g_hmap.m_widthz*TILE_SIZE;
			c->move(Vec3f(0, 0, -d));
		}
#endif

#if 0
		UpdateMouse3D();

		if(g_mode == APPMODE_EDITOR && py->mousekeys[MOUSEKEY_LEFT])
		{
			EdApply();
		}

		if(!py->mousekeys[MOUSEKEY_LEFT])
		{
			g_vStart = g_vTile;
			g_vMouseStart = g_vMouse;
		}
#endif
	}

	Vec3f line[2];
	line[0] = c->zoompos();
	Camera oldcam = *c;
	c->frameupd();
	line[1] = c->zoompos();

	Vec3f ray = Normalize(line[1] - line[0]) * TILE_SIZE;
	//line[0] = line[0] - ray;
	line[1] = line[1] + ray;

	Vec3f clip;

#if 0
	if(GetMapIntersection(&g_hmap, line, &clip))
#else
	if(FastMapIntersect(&g_hmap, line, &clip))
	{
#endif
		*c = oldcam;
	}
	else
	{
		//CalcMapView();
	}

	c->friction2();
}

void LoadConfig()
{
	char cfgfull[MAX_PATH+1];
	FullPath(CONFIGFILE, cfgfull);

	std::ifstream f(cfgfull);

	if(!f)
		return;

	std::string line;
	char keystr[128];
	char actstr[128];

	Player* py = &g_player[g_curP];

	while(!f.eof())
	{
		strcpy(keystr, "");
		strcpy(actstr, "");

		getline(f, line);

		if(line.length() > 127)
			continue;

		sscanf(line.c_str(), "%s %s", keystr, actstr);

		float valuef = StrToFloat(actstr);
		int valuei = StrToInt(actstr);
		bool valueb = (bool)valuei;

		if(stricmp(keystr, "fullscreen") == 0)					g_fullscreen = valueb;
		else if(stricmp(keystr, "client_width") == 0)			py->width = g_selectedRes.width = valuei;
		else if(stricmp(keystr, "client_height") == 0)			py->height = g_selectedRes.height = valuei;
		else if(stricmp(keystr, "screen_bpp") == 0)				py->bpp = valuei;
	}
}

int testfunc(ObjectScript::OS* os, int nparams, int closure_values, int need_ret_values, void * param)
{
	InfoMessage("os", "test");
	return 1;
}

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void SignalCallback(int signum)
{
	//printf("Caught signal %d\n",signum);
	// Cleanup and close up stuff here

	// Terminate program
	g_quit = true;
}

void Init()
{
#ifdef PLATFORM_LINUX
	signal(SIGINT, SignalCallback);
#endif

	SDL_Init(SDL_INIT_VIDEO);

	OpenLog("log.txt", VERSION);

	srand(GetTickCount64());

	LoadConfig();

	//g_os = ObjectScript::OS::create();
	//g_os->pushCFunction(testfunc);
	//g_os->setGlobal("testfunc");
	//os->eval("testfunc();");
	//os->eval("function require(){ /* if(relative == \"called.os\") */ { testfunc(); } }");
	char autoexecpath[MAX_PATH+1];
	FullPath("scripts/autoexec.os", autoexecpath);
	//g_os->require(autoexecpath);
	//g_os->release();

	//EnumerateMaps();
	//EnumerateDisplay();
	MapKeys();

	InitProfiles();
}

void Deinit()
{
	WriteProfiles(-1, 0);
	DestroyWindow(TITLE);
	// Clean up
	SDL_Quit();
}

void EventLoop()
{
#if 0
	key->keysym.scancode
		SDLMod  e.key.keysym.mod
		key->keysym.unicode

		if( mod & KMOD_NUM ) printf( "NUMLOCK " );
	if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
	if( mod & KMOD_LCTRL ) printf( "LCTRL " );
	if( mod & KMOD_RCTRL ) printf( "RCTRL " );
	if( mod & KMOD_RSHIFT ) printf( "RSHIFT " );
	if( mod & KMOD_LSHIFT ) printf( "LSHIFT " );
	if( mod & KMOD_RALT ) printf( "RALT " );
	if( mod & KMOD_LALT ) printf( "LALT " );
	if( mod & KMOD_CTRL ) printf( "CTRL " );
	if( mod & KMOD_SHIFT ) printf( "SHIFT " );
	if( mod & KMOD_ALT ) printf( "ALT " );
#endif

	//SDL_EnableUNICODE(SDL_ENABLE);

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	while (!g_quit)
	{
		StartTimer(TIMER_FRAME);
		StartTimer(TIMER_EVENT);

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InEv ev;
			ev.intercepted = false;

			switch(e.type) {
			case SDL_QUIT:
				g_quit = true;
				break;
			case SDL_KEYDOWN:
				ev.type = INEV_KEYDOWN;
				ev.key = e.key.keysym.sym;
				ev.scancode = e.key.keysym.scancode;

				gui->inev(&ev);

				if(!ev.intercepted)
					py->keys[e.key.keysym.scancode] = true;

				py->keyintercepted = ev.intercepted;
				break;
			case SDL_KEYUP:
				ev.type = INEV_KEYUP;
				ev.key = e.key.keysym.sym;
				ev.scancode = e.key.keysym.scancode;

				gui->inev(&ev);

				if(!ev.intercepted)
					py->keys[e.key.keysym.scancode] = false;

				py->keyintercepted = ev.intercepted;
				break;
			case SDL_TEXTINPUT:
				//g_GUI.charin(e.text.text);	//UTF8
				ev.type = INEV_TEXTIN;
				ev.text = e.text.text;

				g_log<<"SDL_TEXTINPUT:";
				for(int i=0; i<strlen(e.text.text); i++)
				{
					g_log<<"[#"<<(unsigned int)(unsigned char)e.text.text[i]<<"]";
				}
				g_log<<std::endl;
				g_log.flush();

				gui->inev(&ev);
				break;
			case SDL_TEXTEDITING:
				//g_GUI.charin(e.text.text);	//UTF8
				ev.type = INEV_TEXTED;
				ev.text = e.text.text;
				ev.cursor = e.edit.start;
				ev.sellen = e.edit.length;

				g_log<<"SDL_TEXTEDITING:";
				for(int i=0; i<strlen(e.text.text); i++)
				{
					g_log<<"[#"<<(unsigned int)(unsigned char)e.text.text[i]<<"]";
				}
				g_log<<std::endl;
				g_log.flush();

				g_log<<"texted: cursor:"<<ev.cursor<<" sellen:"<<ev.sellen<<std::endl;
				g_log.flush();

				gui->inev(&ev);
#if 0
				ev.intercepted = false;
				ev.type = INEV_TEXTIN;
				ev.text = e.text.text;

				gui->inev(&ev);
#endif
				break;
#if 0
			case SDL_TEXTINPUT:
				/* Add new text onto the end of our text */
				strcat(text, event.text.text);
#if 0
				ev.type = INEV_CHARIN;
				ev.key = wParam;
				ev.scancode = 0;

				gui->inev(&ev);
#endif
				break;
			case SDL_TEXTEDITING:
				/*
				Update the composition text.
				Update the cursor position.
				Update the selection length (if any).
				*/
				composition = event.edit.text;
				cursor = event.edit.start;
				selection_len = event.edit.length;
				break;
#endif
				//else if(e.type == SDL_BUTTONDOWN)
				//{
				//}
			case SDL_MOUSEWHEEL:
				ev.type = INEV_MOUSEWHEEL;
				ev.amount = e.wheel.y;

				gui->inev(&ev);
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (e.button.button) {
				case SDL_BUTTON_LEFT:
					py->mousekeys[MOUSE_LEFT] = true;
					py->moved = false;

					ev.type = INEV_MOUSEDOWN;
					ev.key = MOUSE_LEFT;
					ev.amount = 1;
					ev.x = py->mouse.x;
					ev.y = py->mouse.y;

					gui->inev(&ev);

					py->keyintercepted = ev.intercepted;
					break;
				case SDL_BUTTON_RIGHT:
					py->mousekeys[MOUSE_RIGHT] = true;

					ev.type = INEV_MOUSEDOWN;
					ev.key = MOUSE_RIGHT;
					ev.amount = 1;
					ev.x = py->mouse.x;
					ev.y = py->mouse.y;

					gui->inev(&ev);
					break;
				case SDL_BUTTON_MIDDLE:
					py->mousekeys[MOUSE_MIDDLE] = true;

					ev.type = INEV_MOUSEDOWN;
					ev.key = MOUSE_MIDDLE;
					ev.amount = 1;
					ev.x = py->mouse.x;
					ev.y = py->mouse.y;

					gui->inev(&ev);
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (e.button.button) {
				case SDL_BUTTON_LEFT:
					py->mousekeys[MOUSE_LEFT] = false;

					ev.type = INEV_MOUSEUP;
					ev.key = MOUSE_LEFT;
					ev.amount = 1;
					ev.x = py->mouse.x;
					ev.y = py->mouse.y;

					gui->inev(&ev);
					break;
				case SDL_BUTTON_RIGHT:
					py->mousekeys[MOUSE_RIGHT] = false;

					ev.type = INEV_MOUSEUP;
					ev.key = MOUSE_RIGHT;
					ev.amount = 1;
					ev.x = py->mouse.x;
					ev.y = py->mouse.y;

					gui->inev(&ev);
					break;
				case SDL_BUTTON_MIDDLE:
					py->mousekeys[MOUSE_MIDDLE] = false;

					ev.type = INEV_MOUSEUP;
					ev.key = MOUSE_MIDDLE;
					ev.amount = 1;
					ev.x = py->mouse.x;
					ev.y = py->mouse.y;

					gui->inev(&ev);
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				//py->mouse.x = e.motion.x;
				//py->mouse.y = e.motion.y;

				if(py->mouseout) {
					//TrackMouse();
					py->mouseout = false;
				}
				if(MousePosition()) {
					py->moved = true;

					ev.type = INEV_MOUSEMOVE;
					ev.x = py->mouse.x;
					ev.y = py->mouse.y;

					gui->inev(&ev);
				}
				break;
			}
		}

		StopTimer(TIMER_EVENT);
#if 1
		if ((g_mode == APPMODE_LOADING || g_mode == APPMODE_RELOADING) || true /* DrawNextFrame(DRAW_FRAME_RATE) */ )
#endif
		{
			StartTimer(TIMER_DRAW);

#ifdef DEBUG
			g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
			g_log.flush();
#endif
			CalcDrawRate();

			CheckGLError(__FILE__, __LINE__);

#ifdef DEBUG
			g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
			g_log.flush();
#endif

			Draw();
			CheckGLError(__FILE__, __LINE__);

			if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
			{
#ifdef DEBUG
				g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
				g_log.flush();
#endif
				Scroll();
#ifdef DEBUG
				g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
				g_log.flush();
#endif
				UpdateResTicker();
			}

			StopTimer(TIMER_DRAW);
		}

		if((g_mode == APPMODE_LOADING || g_mode == APPMODE_RELOADING) || UpdNextFrame(SIM_FRAME_RATE) )
		{
			StartTimer(TIMER_UPDATE);

#ifdef DEBUG
			g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
			g_log.flush();
#endif
			CalcUpdRate();
			Update();

			StopTimer(TIMER_UPDATE);
		}

		StopTimer(TIMER_FRAME);
	}
}

#ifdef PLATFORM_WIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	//g_log << "Log start"    << std::endl; /* TODO, include date */
	//g_log << "Init: "       << std::endl;
	//g_log.flush();

	Init();

	g_log << "MakeWindow: " << std::endl;
	g_log.flush();

	MakeWindow(TITLE);

	g_log << "FillGUI: "    << std::endl;
	g_log.flush();

	FillGUI();
	
	g_log << "Queue: "      << std::endl;
	g_log.flush();

	SDL_ShowCursor(false);
	Queue();

	g_log << "EventLoop: "  << std::endl;
	g_log.flush();

	EventLoop();

	g_log << "Deinit: "     << std::endl;
	g_log.flush();

	Deinit();
	SDL_ShowCursor(true);

	return 0;
}
