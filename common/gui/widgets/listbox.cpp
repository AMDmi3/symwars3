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
#include "../../sim/player.h"


ListBox::ListBox(Widget* parent, const char* n, int f, void (*reframef)(Widget* thisw), void (*change)()) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_LISTBOX;
	m_name = n;
	m_font = f;
	reframefunc = reframef;
	m_opened = false;
	m_selected = -1;
	m_scroll[1] = 0;
	m_mousescroll = false;
	m_ldown = false;
	changefunc = change;
	CreateTexture(m_frametex, "gui/frame.jpg", true, false);
	CreateTexture(m_filledtex, "gui/filled.jpg", true, false);
	CreateTexture(m_uptex, "gui/up.jpg", true, false);
	//CreateTexture(m_downtex, "gui/down.jpg", true, false);
	reframe();
}

void ListBox::erase(int which)
{
	m_options.erase( m_options.begin() + which );
	if(m_selected == which)
		m_selected = -1;

	if(m_scroll[1] + rowsshown() > m_options.size())
		m_scroll[1] = m_options.size() - rowsshown();

	if(m_scroll[1] < 0)
		m_scroll[1] = 0;
}

int ListBox::rowsshown()
{
	int rows = (m_pos[3]-m_pos[1])/g_font[m_font].gheight;

	if(rows > m_options.size())
		rows = m_options.size();

	return rows;
}

int ListBox::square()
{
	return g_font[m_font].gheight;
}

float ListBox::scrollspace()
{
	return (m_pos[3]-m_pos[1]-square()*2);
}

void ListBox::draw()
{
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	Font* f = &g_font[m_font];
	int rows = rowsshown();

	DrawImage(m_frametex, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

	DrawImage(g_texture[m_frametex].texname, m_pos[2]-square(), m_pos[1], m_pos[2], m_pos[3]);
	DrawImage(g_texture[m_uptex].texname, m_pos[2]-square(), m_pos[1], m_pos[2], m_pos[1]+square());
	DrawImage(g_texture[m_downtex].texname, m_pos[2]-square(), m_pos[3]-square(), m_pos[2], m_pos[3]);
	DrawImage(g_texture[m_filledtex].texname, m_pos[2]-square(), m_pos[1]+square()+scrollspace()*topratio(), m_pos[2], m_pos[1]+square()+scrollspace()*bottomratio());

	if(m_selected >= 0 && m_selected >= (int)m_scroll[1] && m_selected < (int)m_scroll[1]+rowsshown())
	{
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 0.5f);
		DrawImage(g_texture[m_filledtex].texname, m_pos[0], m_pos[1]+(m_selected-(int)m_scroll[1])*f->gheight, m_pos[2]-square(), m_pos[1]+(m_selected-(int)m_scroll[1]+1)*f->gheight);
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	}

	for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
		DrawShadowedText(m_font, m_pos[0]+3, m_pos[1]+g_font[m_font].gheight*(i-(int)m_scroll[1]), &m_options[i]);
}

void ListBox::inev(InEv* ev)
{
	Player* py = &g_player[g_curP];

	if(ev->type == INEV_MOUSEMOVE && !ev->intercepted)
	{
		if(!m_mousescroll)
			return;

		int dy = py->mouse.y - m_mousedown[1];

		float topy = m_pos[3]+square()+scrollspace()*topratio();
		float newtopy = topy + dy;

		//topratio = (float)scroll / (float)(options.size());
		//topy = pos[3]+square+scrollspace*topratio
		//topy = pos[3]+square+scrollspace*((float)scroll / (float)(options.size()))
		//topy - pos[3] - square = scrollspace*(float)scroll / (float)(options.size())
		//(topy - pos[3] - square)*(float)(options.size())/scrollspace = scroll

		m_scroll[1] = (newtopy - m_pos[3] - square())*(float)(m_options.size())/scrollspace();

		if(m_scroll[1] < 0)
		{
			m_scroll[1] = 0;
			ev->intercepted = true;
			return;
		}
		else if(m_scroll[1] + rowsshown() > m_options.size())
		{
			m_scroll[1] = m_options.size() - rowsshown();
			ev->intercepted = true;
			return;
		}

		m_mousedown[1] = py->mouse.y;

		ev->intercepted = true;
	}
	else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		Font* f = &g_font[m_font];

		for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
		{
			int row = i-(int)m_scroll[1];
			// std::list item?
			if(py->mouse.x >= m_pos[0] && py->mouse.x <= m_pos[2]-square() && py->mouse.y >= m_pos[1]+f->gheight*row
					&& py->mouse.y <= m_pos[1]+f->gheight*(row+1))
			{
				m_ldown = true;
				ev->intercepted = true;
				return;	// intercept mouse event
			}
		}

		// scroll bar?
		if(py->mouse.x >= m_pos[2]-square() && py->mouse.y >= m_pos[1]+square()+scrollspace()*topratio() && py->mouse.x <= m_pos[2] &&
				py->mouse.y <= m_pos[1]+square()+scrollspace()*bottomratio())
		{
			m_ldown = true;
			m_mousescroll = true;
			m_mousedown[1] = py->mouse.y;
			ev->intercepted = true;
			return;	// intercept mouse event
		}

		// up button?
		if(py->mouse.x >= m_pos[2]-square() && py->mouse.y >= m_pos[1] && py->mouse.x <= m_pos[2] && py->mouse.y <= m_pos[1]+square())
		{
			m_ldown = true;
			ev->intercepted = true;
			return;
		}

		// down button?
		if(py->mouse.x >= m_pos[2]-square() && py->mouse.y >= m_pos[3]-square() && py->mouse.x <= m_pos[2] && py->mouse.y <= m_pos[3])
		{
			m_ldown = true;
			ev->intercepted = true;
			return;
		}
	}
	else if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		if(!m_ldown)
			return;

		m_ldown = false;

		if(m_mousescroll)
		{
			m_mousescroll = false;
			ev->intercepted = true;
			return;	// intercept mouse event
		}

		Font* f = &g_font[m_font];

		for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
		{
			int row = i-(int)m_scroll[1];

			// std::list item?
			if(py->mouse.x >= m_pos[0] && py->mouse.x <= m_pos[2]-square() && py->mouse.y >= m_pos[1]+f->gheight*row
					&& py->mouse.y <= m_pos[1]+f->gheight*(row+1))
			{
				m_selected = i;
				if(changefunc != NULL)
					changefunc();

				ev->intercepted = true;
				return;	// intercept mouse event
			}
		}

		// up button?
		if(py->mouse.x >= m_pos[2]-square() && py->mouse.y >= m_pos[1] && py->mouse.x <= m_pos[2] && py->mouse.y <= m_pos[1]+square())
		{
			if(rowsshown() < (int)((m_pos[3]-m_pos[1])/f->gheight))
			{
				ev->intercepted = true;
				return;
			}

			m_scroll[1]--;
			if(m_scroll[1] < 0)
				m_scroll[1] = 0;

			ev->intercepted = true;
			return;
		}

		// down button?
		if(py->mouse.x >= m_pos[2]-square() && py->mouse.y >= m_pos[3]-square() && py->mouse.x <= m_pos[2] && py->mouse.y <= m_pos[3])
		{
			m_scroll[1]++;
			if(m_scroll[1]+rowsshown() > m_options.size())
				m_scroll[1] = m_options.size() - rowsshown();

			ev->intercepted = true;
			return;
		}

		ev->intercepted = true;	// intercept mouse event
	}
}

