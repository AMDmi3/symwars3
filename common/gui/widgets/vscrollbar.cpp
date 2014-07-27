#include "vscrollbar.h"
#include "../../sim/player.h"
#include "../../render/shader.h"
#include "../../debug.h"

VScroll::VScroll() : Widget()
{
	m_parent = NULL;
	m_type = WIDGET_VSCROLLBAR;
	m_name = "";
	m_opened = false;
	m_selected = 0;
	m_scroll[1] = 0;
	m_mousescroll = false;
	m_ldown = false;
	m_ldownbar = false;
	m_domain = 1;
	CreateTexture(m_uptex, "gui/up.jpg", true, false);
	changefunc = NULL;
	reframefunc = NULL;
}

VScroll::VScroll(Widget* parent, const char* n) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_VSCROLLBAR;
	m_name = n;
	m_opened = false;
	m_selected = 0;
	m_scroll[1] = 0;
	m_mousescroll = false;
	m_ldown = false;
	m_ldownbar = false;
	m_domain = 0.5f;
	CreateTexture(m_uptex, "gui/up.jpg", true, false);
	changefunc = NULL;
	reframefunc = NULL;
	//reframe();
}

void VScroll::reframe()
{
	int w = m_pos[2]-m_pos[0];

#if 0
	float m_barpos[4];
	float m_uppos[4];
	float m_downpos[4];
#endif

	m_uppos[0] = m_pos[0];
	m_uppos[1] = m_pos[1];
	m_uppos[2] = m_pos[2];
	m_uppos[3] = m_pos[1]+w;

	m_downpos[0] = m_pos[0];
	m_downpos[1] = m_pos[3]-w;
	m_downpos[2] = m_pos[2];
	m_downpos[3] = m_pos[3];

	m_barpos[0] = m_pos[0];
	m_barpos[1] = m_uppos[3] + (m_downpos[1]-m_uppos[3])*m_scroll[1];
	m_barpos[2] = m_pos[2];
	m_barpos[3] = m_barpos[1] + fmax(w, m_domain*(m_downpos[1]-m_uppos[3]));



	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->reframe();
}

void VScroll::draw()
{
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 1, 1, 1, 0.8f);
	DrawImage(g_texture[ m_uptex ].texname, m_uppos[0], m_uppos[1], m_uppos[2], m_uppos[3], 0, 0, 1, 1);
	DrawImage(g_texture[ m_uptex ].texname, m_downpos[0], m_downpos[1], m_downpos[2], m_downpos[3], 0, 1, 1, 0);

	Player* py = &g_player[g_curP];

	EndS();

	UseS(SHADER_COLOR2D);
	glUniform1f(g_shader[g_curS].m_slot[SSLOT_WIDTH], (float)py->currw);
	glUniform1f(g_shader[g_curS].m_slot[SSLOT_HEIGHT], (float)py->currh);

	float midcolor[] = {0.7f,0.7f,0.7f,0.8f};
	float lightcolor[] = {0.8f,0.8f,0.8f,0.8f};
	float darkcolor[] = {0.5f,0.5f,0.5f,0.8f};

	if(m_over)
	{
		for(int i=0; i<3; i++)
		{
			midcolor[i] = 0.8f;
			lightcolor[i] = 0.9f;
			darkcolor[i] = 0.6f;
		}
	}

	DrawSquare(midcolor[0], midcolor[1], midcolor[2], midcolor[3], m_barpos[0], m_barpos[1], m_barpos[2], m_barpos[3]);

	DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], m_barpos[2], m_barpos[1], m_barpos[2], m_barpos[3]-1);
	DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], m_barpos[0], m_barpos[1], m_barpos[2]-1, m_barpos[1]);

	DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], m_barpos[0]+1, m_barpos[3], m_barpos[2], m_barpos[3]);
	DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], m_barpos[2], m_barpos[1]+1, m_barpos[2], m_barpos[3]);

	EndS();
	CheckGLError(__FILE__, __LINE__);
	Ortho(py->currw, py->currh, 1, 1, 1, 1);

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->draw();
}

void VScroll::drawover()
{
	if(!m_opened)
		return;

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->drawover();
}

void VScroll::inev(InEv* ev)
{
	for(auto w=m_subwidg.rbegin(); w!=m_subwidg.rend(); w++)
		(*w)->inev(ev);

	Player* py = &g_player[g_curP];

#if 1
	if(m_ldown)
	{
		if(ev->type == INEV_MOUSEMOVE ||
				( (ev->type == INEV_MOUSEDOWN || ev->type == INEV_MOUSEUP) && ev->key == MOUSE_LEFT) )
			ev->intercepted = true;

		if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_LEFT)
		{
			m_ldown = false;
			m_ldownbar = false;
		}

		if(ev->type == INEV_MOUSEMOVE && m_ldownbar)
		{
			if(py->mouse.y < m_barpos[1] || py->mouse.y > m_barpos[3])
				return;

			int dx = py->mouse.x - m_mousedown[0];
			int dy = py->mouse.y - m_mousedown[1];
			m_mousedown[0] = py->mouse.x;
			m_mousedown[1] = py->mouse.y;

			float origscroll = m_scroll[1];

			m_scroll[1] += (float)dy / (m_downpos[1] - m_uppos[3]);

			int w = m_pos[2]-m_pos[0];

			m_barpos[1] = m_uppos[3] + (m_downpos[1]-m_uppos[3])*m_scroll[1];
			m_barpos[3] = m_barpos[1] + fmax(w, m_domain*(m_downpos[1]-m_uppos[3]));

			float overy = m_barpos[3] - m_downpos[1];

			if(overy > 0)
			{
				m_barpos[1] -= overy;
				m_barpos[3] -= overy;
				m_scroll[1] = (m_barpos[1] - m_uppos[3]) / (m_downpos[1] - m_uppos[3]);
			}

			float undery = m_uppos[3] - m_barpos[1];

			if(undery > 0)
			{
				m_barpos[1] += undery;
				m_barpos[3] += undery;
				m_scroll[1] = (m_barpos[1] - m_uppos[3]) / (m_downpos[1] - m_uppos[3]);
			}

			if(m_parent)
			{
				VScroll::ScrollEv sev;
				sev.delta = m_scroll[1] - origscroll;
				sev.newpos = m_scroll[1];

				m_parent->chcall(this, CHCALL_VSCROLL, (void*)&sev);
			}
		}
	}
#endif

	if(m_over && ev->type == INEV_MOUSEDOWN && !ev->intercepted)
	{
		if(ev->key == MOUSE_LEFT)
		{
			m_ldown = true;

			if(py->mouse.x >= m_barpos[0] &&
					py->mouse.y >= m_barpos[1] &&
					py->mouse.x <= m_barpos[2] &&
					py->mouse.y <= m_barpos[3])
			{
				m_ldownbar = true;
				m_mousedown[0] = py->mouse.x;
				m_mousedown[1] = py->mouse.y;
				ev->intercepted = true;
			}
		}
	}

	if(ev->type == INEV_MOUSEMOVE)
	{
		if(m_ldown)
		{
			ev->intercepted = true;
			return;
		}

		if(!ev->intercepted &&
				py->mouse.x >= m_pos[0] &&
				py->mouse.y >= m_pos[1] &&
				py->mouse.x <= m_pos[2] &&
				py->mouse.y <= m_pos[3])
		{
			m_over = true;

			if(py->mousekeys[MOUSE_MIDDLE])
				return;

			ev->intercepted = true;

			if(py->curst == CU_DRAG)
				return;

			py->curst = CU_DEFAULT;
		}
		else
		{
			if(!ev->intercepted)
			{
				if(m_over)
				{
					py->curst = CU_DEFAULT;
				}
			}
			else
			{
				// to do: this will be replaced by code in other
				//widgets that will set the cursor
				py->curst = CU_DEFAULT;
			}

			m_over = false;
		}
	}
}
