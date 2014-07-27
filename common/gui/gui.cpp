#include "gui.h"
#include "../render/shader.h"
#include "../texture.h"
#include "font.h"
#include "../math/3dmath.h"
#include "../platform.h"
#include "../window.h"
#include "draw2d.h"
#include "../render/shadow.h"
#include "../render/heightmap.h"
#include "../../game/gmain.h"
#include "cursor.h"
#include "../sim/player.h"
#include "../debug.h"

void GUI::draw()
{
	Player* py = &g_player[g_curP];

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	CheckGLError(__FILE__, __LINE__);
	Ortho(py->width, py->height, 1, 1, 1, 1);
	CheckGLError(__FILE__, __LINE__);

#if 0
	DrawImage(g_texture[0].texname, py->width - 300, 0, py->width, 300, 0, 1, 1, 0);
#endif

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
	{
#ifdef DEBUG
	g_log<<"draw "<<(*i)->m_name<<" "<<__FILE__<<" "<<__LINE__<<std::endl;
    g_log.flush();
#endif

		(*i)->draw();
    }

	CheckGLError(__FILE__, __LINE__);

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->drawover();

#if 0
	DrawImage(g_depth, py->width - 300, 0, py->width, 300, 0, 1, 1, 0);
#endif

#if 0
	if(g_depth != -1)
		DrawImage(g_depth, 0, 0, 150, 150, 0, 1, 1, 0);
#endif

#if 0
	if(g_mode == APPMODE_PLAY)
		DrawImage(g_tiletexs[TILE_PRERENDER], 0, 0, 150, 150, 0, 1, 1, 0);
#endif

	Sprite* sp = &g_cursor[py->curst];
	DrawImage(g_texture[sp->texindex].texname, py->mouse.x-sp->offset[0], py->mouse.y-sp->offset[1], py->mouse.x-sp->offset[0]+32, py->mouse.y-sp->offset[1]+32);

	CheckGLError(__FILE__, __LINE__);

	EndS();
	CheckGLError(__FILE__, __LINE__);

	UseS(SHADER_COLOR2D);
	glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_WIDTH], (float)py->width);
	glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_HEIGHT], (float)py->height);
	glUniform4f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_COLOR], 0, 1, 0, 0.75f);
	//glEnable(GL_DEPTH_TEST);
	//DrawSelector();
	DrawMarquee();

	CheckGLError(__FILE__, __LINE__);
	EndS();
	CheckGLError(__FILE__, __LINE__);

	glEnable(GL_DEPTH_TEST);
}

void GUI::inev(InEv* ev)
{
	for(auto w=m_subwidg.rbegin(); w!=m_subwidg.rend(); w++)
		(*w)->inev(ev);

	if(!ev->intercepted)
	{
		if(ev->type == INEV_MOUSEMOVE && mousemovefunc) mousemovefunc();
		else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_LEFT && lbuttondownfunc) lbuttondownfunc();
		else if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_LEFT && lbuttonupfunc) lbuttonupfunc();
		else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_MIDDLE && mbuttondownfunc) mbuttondownfunc();
		else if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_MIDDLE && mbuttonupfunc) mbuttonupfunc();
		else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_RIGHT && rbuttondownfunc) rbuttondownfunc();
		else if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_RIGHT && rbuttonupfunc) rbuttonupfunc();
		else if(ev->type == INEV_MOUSEWHEEL && mousewheelfunc) mousewheelfunc(ev->amount);
		else if(ev->type == INEV_KEYDOWN && keydownfunc[ev->scancode]) keydownfunc[ev->scancode]();
		else if(ev->type == INEV_KEYUP && keyupfunc[ev->scancode]) keyupfunc[ev->scancode]();
	}
}

void GUI::closeall()
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->m_opened = false;
}

void GUI::close(const char* name)
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		if(stricmp((*i)->m_name.c_str(), name) == 0)
		{
			(*i)->close();
		}
}

void GUI::open(const char* name)
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		if(stricmp((*i)->m_name.c_str(), name) == 0)
			(*i)->m_opened = true;
}

void GUI::reframe()
{
	Player* py = &g_player[g_curP];

	m_pos[0] = 0;
	m_pos[1] = 0;
	m_pos[2] = py->width-1;
	m_pos[3] = py->height-1;

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->reframe();
}

void Status(const char* status, bool logthis)
{
	if(logthis)
	{
		g_log<<status<<std::endl;
		g_log.flush();
	}

#if 1
	g_log<<status<<std::endl;
	g_log.flush();
#endif
	/*
	char upper[1024];
	int i;
	for(i=0; i<strlen(status); i++)
	{
	upper[i] = toupper(status[i]);
	}
	upper[i] = '\0';*/

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	//gui->get("loading")->get("status", WIDGET_TEXT)->m_text = upper;
	ViewLayer* loadingview = (ViewLayer*)gui->get("loading");

	if(!loadingview)
		return;

	Widget* statustext = loadingview->get("status");

	if(!statustext)
		return;

	statustext->m_text = RichText(UString(status));
}

bool MousePosition()
{
	Player* py = &g_player[g_curP];

	Vec2i old = py->mouse;
	SDL_GetMouseState(&py->mouse.x, &py->mouse.y);

	if(py->mouse.x == old.x && py->mouse.y == old.y)
		return false;

	return true;
}

void CenterMouse()
{
	Player* py = &g_player[g_curP];

	py->mouse.x = py->width/2;
	py->mouse.y = py->height/2;
	SDL_WarpMouseInWindow(g_window, py->mouse.x, py->mouse.y);
}

void Ortho(int width, int height, float r, float g, float b, float a)
{
	CheckGLError(__FILE__, __LINE__);
	Player* py = &g_player[g_curP];
	UseS(SHADER_ORTHO);
	Shader* s = &g_shader[g_curS];
	glUniform1f(g_shader[SHADER_ORTHO].m_slot[SSLOT_WIDTH], (float)width);
	glUniform1f(g_shader[SHADER_ORTHO].m_slot[SSLOT_HEIGHT], (float)height);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], r, g, b, a);
	//glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
	//glEnableVertexAttribArray(g_shader[SHADER_ORTHO].m_slot[SSLOT_TEXCOORD0]);
	//glEnableVertexAttribArray(g_shader[SHADER_ORTHO].m_slot[SSLOT_NORMAL]);
	py->currw = width;
	py->currh = height;
	CheckGLError(__FILE__, __LINE__);
}
