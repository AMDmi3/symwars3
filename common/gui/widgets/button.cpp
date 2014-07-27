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
#include "../icon.h"
#include "../../ustring.h"
#include "../gui.h"
#include "../../sim/player.h"
#include "../../debug.h"

Button::Button() : Widget()
{
	m_parent = NULL;
	m_type = WIDGET_BUTTON;
	//m_text = RichText("");
	m_font = MAINFONT8;
	//float length = 0;
	//for(int i=0; i<strlen(t); i++)
	//    length += g_font[f].glyph[t[i]].w;
	//m_tpos[0] = (left+right)/2.0f - length/2.0f;
	//m_tpos[1] = (top+bottom)/2.0f - g_font[f].gheight/2.0f;
	m_style = BUTTON_LINEBASED;
	m_over = false;
	m_ldown = false;
	CreateTexture(m_tex, "gui/transp.png", true, false);
	CreateTexture(m_bgtex, "gui/corrodbutton.png", true, false);
	CreateTexture(m_bgovertex, "gui/corrodbuttonover.png", true, false);
	reframefunc = NULL;
	clickfunc = NULL;
	overfunc = NULL;
	clickfunc2 = NULL;
	overfunc2 = NULL;
	outfunc = NULL;
	m_param = -1;
	//reframe();
}

Button::Button(Widget* parent, const char* name, const char* filepath, const RichText label, const RichText tooltip, int f, int style, void (*reframef)(Widget* thisw), void (*click)(), void (*click2)(int p), void (*overf)(), void (*overf2)(int p), void (*out)(), int parm) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_BUTTON;
	m_name = name;
	m_style = style;
	m_text = tooltip;
	m_label = label;
	m_font = f;
	float length = 0;
	//for(int i=0; i<strlen(t); i++)
	//    length += g_font[f].glyph[t[i]].origsize[0];
	length = EndX(&m_text, m_text.rawlen(), m_font, 0, 0);
	//m_tpos[0] = (left+right)/2.0f - length/2.0f;
	//m_tpos[1] = (top+bottom)/2.0f - g_font[f].gheight/2.0f;
	m_over = false;
	m_ldown = false;
	CreateTexture(m_tex, filepath, true, false);

	if(style == BUTTON_CORRODE)
	{
		CreateTexture(m_bgtex, "gui/corrodbutton.png", true, false);
		CreateTexture(m_bgovertex, "gui/corrodbuttonover.png", true, false);
	}

	reframefunc = reframef;
	clickfunc = click;
	clickfunc2 = click2;
	overfunc = overf;
	overfunc2 = overf2;
	outfunc = out;
	m_param = parm;
	reframe();
}

void Button::reframe()
{
	if(reframefunc)
		reframefunc(this);
}

void Button::inev(InEv* ev)
{
	Player* py = &g_player[g_curP];

	if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		//mousemove();

		if(m_over && m_ldown)
		{
			if(clickfunc != NULL)
				clickfunc();

			if(clickfunc2 != NULL)
				clickfunc2(m_param);

			m_over = false;
			m_ldown = false;

			ev->intercepted = true;
			return;	// intercept mouse event
		}

		if(m_ldown)
		{
			m_ldown = false;
			ev->intercepted = true;
			return;
		}

		m_over = false;
	}
	else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		//mousemove();

		if(m_over)
		{
			m_ldown = true;
			ev->intercepted = true;
			return;	// intercept mouse event
		}
	}
	else if(ev->type == INEV_MOUSEMOVE)
	{
		if(py->mouse.x >= m_pos[0] && py->mouse.x <= m_pos[2] && py->mouse.y >= m_pos[1] && py->mouse.y <= m_pos[3])
		{
		}
		else
		{
			if(m_over && outfunc != NULL)
				outfunc();

			m_over = false;
		}

		if(!ev->intercepted)
		{
			if(py->mouse.x >= m_pos[0] && py->mouse.x <= m_pos[2] && py->mouse.y >= m_pos[1] && py->mouse.y <= m_pos[3])
			{
				if(overfunc != NULL)
					overfunc();
				if(overfunc2 != NULL)
					overfunc2(m_param);

				m_over = true;

				ev->intercepted = true;
				return;
			}
		}
	}
}

void Button::draw()
{
	if(m_style == BUTTON_CORRODE)
	{
		if(m_over)
			DrawImage(g_texture[m_bgovertex].texname, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);
		else
			DrawImage(g_texture[m_bgtex].texname, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

		DrawImage(g_texture[m_tex].texname, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

		DrawShadowedText(m_font, m_tpos[0], m_tpos[1], &m_label);
	}
	else if(m_style == BUTTON_LEFTIMAGE)
	{
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

		DrawSquare(midcolor[0], midcolor[1], midcolor[2], midcolor[3], m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], m_pos[2], m_pos[1], m_pos[2], m_pos[3]-1);
		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], m_pos[0], m_pos[1], m_pos[2]-1, m_pos[1]);

		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], m_pos[0]+1, m_pos[3], m_pos[2], m_pos[3]);
		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], m_pos[2], m_pos[1]+1, m_pos[2], m_pos[3]);

		EndS();
		CheckGLError(__FILE__, __LINE__);
		Ortho(py->currw, py->currh, 1, 1, 1, 1);

		float w = m_pos[2]-m_pos[0]-2;
		float h = m_pos[3]-m_pos[1]-2;
		float minsz = fmin(w, h);

		DrawImage(g_texture[m_tex].texname, m_pos[0]+1, m_pos[1]+1, m_pos[0]+minsz, m_pos[1]+minsz);

		float gheight = g_font[m_font].gheight;
		float texttop = m_pos[1] + h/2.0f - gheight/2.0f;
		float textleft = m_pos[0]+minsz+1;

		DrawShadowedText(m_font, textleft, texttop, &m_label);
	}

	//if(_stricmp(m_name.c_str(), "choose texture") == 0)
	//	g_log<<"draw choose texture"<<std::endl;
}

void Button::drawover()
{
	if(m_over)
	{
		Player* py = &g_player[g_curP];
		//DrawShadowedText(m_font, m_tpos[0], m_tpos[1], m_text.c_str());
		DrawShadowedText(m_font, py->mouse.x, py->mouse.y-g_font[m_font].gheight, &m_text);
	}
}

