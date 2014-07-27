#include "../widget.h"
#include "barbutton.h"
#include "button.h"
#include "checkbox.h"
#include "editbox.h"
#include "dropdowns.h"
#include "image.h"
#include "insdraw.h"
#include "link.h"
#include "listbox.h"
#include "text.h"
#include "textarea.h"
#include "textblock.h"
#include "touchlistener.h"
#include "viewportw.h"
#include "../../platform.h"
#include "../../window.h"
#include "../../render/shader.h"
#include "../gui.h"
#include "../../sim/player.h"
#include "../../debug.h"

ViewportW::ViewportW()
{
	m_parent = NULL;
	m_type = WIDGET_VIEWPORT;
	m_name = "";
	reframefunc = NULL;
	m_ldown = false;
	m_param = -1;
	drawfunc = NULL;
	ldownfunc = NULL;
	lupfunc = NULL;
	mousemovefunc = NULL;
	rdownfunc = NULL;
	rupfunc = NULL;
	mousewfunc = NULL;
	//reframe();
}

ViewportW::ViewportW(Widget* parent, const char* n, void (*reframef)(Widget* thisw),
					 void (*drawf)(int p, int x, int y, int w, int h),
					 bool (*ldownf)(int p, int x, int y, int w, int h),
					 bool (*lupf)(int p, int x, int y, int w, int h),
					 bool (*mousemovef)(int p, int x, int y, int w, int h),
					 bool (*rdownf)(int p, int relx, int rely, int w, int h),
					 bool (*rupf)(int p, int relx, int rely, int w, int h),
					 bool (*mousewf)(int p, int d),
					 int parm)
{
	m_parent = parent;
	m_type = WIDGET_VIEWPORT;
	m_name = n;
	reframefunc = reframef;
	m_ldown = false;
	m_param = parm;
	drawfunc = drawf;
	ldownfunc = ldownf;
	lupfunc = lupf;
	mousemovefunc = mousemovef;
	rdownfunc = rdownf;
	rupfunc = rupf;
	mousewfunc = mousewf;
	reframe();
}

void ViewportW::draw()
{
	//g_log<<m_pos[0]<<","<<m_pos[1]<<","<<m_pos[2]<<","<<m_pos[3]<<std::endl;

	Player* py = &g_player[g_curP];
	int w = m_pos[2] - m_pos[0];
	int h = m_pos[3] - m_pos[1];

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(m_pos[0], py->height-m_pos[3], w, h);
	glUniform1f(g_shader[SHADER_ORTHO].m_slot[SSLOT_WIDTH], (float)w);
	glUniform1f(g_shader[SHADER_ORTHO].m_slot[SSLOT_HEIGHT], (float)h);

	EndS();

	CheckGLError(__FILE__, __LINE__);

	if(drawfunc != NULL)
		drawfunc(m_param, m_pos[0], m_pos[1], w, h);

	CheckGLError(__FILE__, __LINE__);

	//glViewport(0, 0, py->width, py->height);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	//glUniform1f(g_shader[SHADER_ORTHO].m_slot[SSLOT_WIDTH], (float)py->width);
	//glUniform1f(g_shader[SHADER_ORTHO].m_slot[SSLOT_HEIGHT], (float)py->height);

	CheckGLError(__FILE__, __LINE__);
	Ortho(py->width, py->height, 1, 1, 1, 1);
}

void ViewportW::inev(InEv* ev)
{
	Player* py = &g_player[g_curP];

	if(ev->type == INEV_MOUSEMOVE)
	{
		if(py->mouse.x >= m_pos[0] && py->mouse.x <= m_pos[2] && py->mouse.y >= m_pos[1] && py->mouse.y <= m_pos[3])
		{}
		else
			m_over = false;
	}
	else if(ev->type == INEV_MOUSEMOVE && !ev->intercepted)
	{
		if(py->mouse.x >= m_pos[0] && py->mouse.x <= m_pos[2] && py->mouse.y >= m_pos[1] && py->mouse.y <= m_pos[3])
			m_over = true;

		if(mousemovefunc != NULL)
		{
			int relx = py->mouse.x - m_pos[0];
			int rely = py->mouse.y - m_pos[1];
			int w = m_pos[2] - m_pos[0];
			int h = m_pos[3] - m_pos[1];
			ev->intercepted = mousemovefunc(m_param, relx, rely, w, h);
		}

		return;
	}
	else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		if(!m_over)
			return;

		if(ldownfunc != NULL)
		{
			Player* py = &g_player[g_curP];
			int relx = py->mouse.x - m_pos[0];
			int rely = py->mouse.y - m_pos[1];
			int w = m_pos[2] - m_pos[0];
			int h = m_pos[3] - m_pos[1];
			ev->intercepted = ldownfunc(m_param, relx, rely, w, h);
		}
	}
	else if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		if(lupfunc != NULL)
		{
			Player* py = &g_player[g_curP];
			int relx = py->mouse.x - m_pos[0];
			int rely = py->mouse.y - m_pos[1];
			int w = m_pos[2] - m_pos[0];
			int h = m_pos[3] - m_pos[1];
			ev->intercepted = lupfunc(m_param, relx, rely, w, h);
		}
	}
	else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_RIGHT && !ev->intercepted)
	{
		if(!m_over)
			return;

		if(rdownfunc != NULL)
		{
			Player* py = &g_player[g_curP];
			int relx = py->mouse.x - m_pos[0];
			int rely = py->mouse.y - m_pos[1];
			int w = m_pos[2] - m_pos[0];
			int h = m_pos[3] - m_pos[1];
			ev->intercepted = rdownfunc(m_param, relx, rely, w, h);
		}
	}
	else if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_RIGHT && !ev->intercepted)
	{
		if(rupfunc != NULL)
		{
			Player* py = &g_player[g_curP];
			int relx = py->mouse.x - m_pos[0];
			int rely = py->mouse.y - m_pos[1];
			int w = m_pos[2] - m_pos[0];
			int h = m_pos[3] - m_pos[1];
			ev->intercepted = rupfunc(m_param, relx, rely, w, h);
		}
	}
	else if(ev->type == INEV_MOUSEWHEEL && !ev->intercepted)
	{
		if(mousewfunc != NULL)
		{
			ev->intercepted = mousewfunc(m_param, ev->amount);
		}
	}
}
