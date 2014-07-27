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
#include "../../debug.h"

Image::Image()
{
	m_parent = NULL;
	m_type = WIDGET_IMAGE;
	m_tex = 0;
	m_pos[0] = 0;
	m_pos[1] = 0;
	m_pos[2] = 0;
	m_pos[3] = 0;
}

Image::Image(Widget* parent, const char* nm, const char* filepath, bool clamp, void (*reframef)(Widget* thisw), float r, float g, float b, float a, float texleft, float textop, float texright, float texbottom) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_IMAGE;
	m_name = nm;
	//CreateTexture(tex, filepath, true);
	CreateTexture(m_tex, filepath, clamp, false);
	//CreateTexture(tex, filepath, clamp);
	reframefunc = reframef;
	m_texc[0] = texleft;
	m_texc[1] = textop;
	m_texc[2] = texright;
	m_texc[3] = texbottom;
	m_ldown = false;
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
	m_pos[0] = 0;
	m_pos[1] = 0;
	m_pos[2] = 0;
	m_pos[3] = 0;
	reframe();
}

Image::Image(Widget* parent, const char* filepath, bool clamp, void (*reframef)(Widget* thisw), float r, float g, float b, float a, float texleft, float textop, float texright, float texbottom) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_IMAGE;
	//CreateTexture(tex, filepath, true);
	CheckGLError(__FILE__, __LINE__);
	CreateTexture(m_tex, filepath, clamp, false);
	CheckGLError(__FILE__, __LINE__);
	//CreateTexture(tex, filepath, clamp);
	reframefunc = reframef;
	m_texc[0] = texleft;
	m_texc[1] = textop;
	m_texc[2] = texright;
	m_texc[3] = texbottom;
	m_ldown = false;
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
	m_pos[0] = 0;
	m_pos[1] = 0;
	m_pos[2] = 0;
	m_pos[3] = 0;
	reframe();
}

void Image::reframe()	//resized or moved
{
	if(reframefunc)
		reframefunc(this);

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->reframe();
}

void Image::draw()
{
	//glColor4fv(rgba);
	glUniform4fv(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, m_rgba);
	DrawImage(g_texture[m_tex].texname, m_pos[0], m_pos[1], m_pos[2], m_pos[3], m_texc[0], m_texc[1], m_texc[2], m_texc[3]);
	//glColor4f(1,1,1,1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
}

