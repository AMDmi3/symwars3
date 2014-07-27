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
#include "../gui.h"
#include "../../sim/player.h"
#include "../../debug.h"

BarButton::BarButton(Widget* parent, unsigned int sprite, float bar, void (*reframef)(Widget* thisw), void (*click)(), void (*overf)(), void (*out)()) : Button()
{
	m_parent = parent;
	m_type = WIDGET_BARBUTTON;
	m_over = false;
	m_ldown = false;
	m_tex = sprite;
	CreateTexture(m_bgtex, "gui/buttonbg.png", true, false);
	CreateTexture(m_bgovertex, "gui/buttonbgover.png", true, false);
	reframefunc = reframef;
	m_healthbar = bar;
	clickfunc = click;
	overfunc = overf;
	outfunc = out;
	reframe();
}

void BarButton::draw()
{
	if(m_over)
		DrawImage(g_texture[m_bgovertex].texname, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);
	else
		DrawImage(g_texture[m_bgtex].texname, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

	DrawImage(g_texture[m_tex].texname, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

	Player* py = &g_player[g_curP];

	EndS();
	UseS(SHADER_COLOR2D);
	glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_WIDTH], (float)py->currw);
	glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_HEIGHT], (float)py->currh);
	DrawSquare(1, 0, 0, 1, m_pos[0], m_pos[3]-5, m_pos[2], m_pos[3]);
	float bar = (m_pos[2] - m_pos[0]) * m_healthbar;
	DrawSquare(0, 1, 0, 1, m_pos[0], m_pos[3]-5, m_pos[0]+bar, m_pos[3]);

	EndS();
	CheckGLError(__FILE__, __LINE__);
	Ortho(py->currw, py->currh, 1, 1, 1, 1);
}

