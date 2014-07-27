#include "widget.h"
#include "gui.h"
#include "font.h"
#include "../window.h"
#include "icon.h"

void Widget::reframe()	//resized or moved
{
	if(reframefunc)
		reframefunc(this);

	if(m_parent)
	{
		float parentframe[4];
		m_parent->subframe(parentframe);
		SubFrame(parentframe, m_pos, m_frame);
	}

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->reframe();
}

void CenterLabel(Widget* w)
{
	Font* f = &g_font[w->m_font];

	int texwidth = TextWidth(w->m_font, &w->m_label);

	w->m_tpos[0] = (w->m_pos[2]+w->m_pos[0])/2 - texwidth/2;
	w->m_tpos[1] = (w->m_pos[3]+w->m_pos[1])/2 - f->gheight/2;
}

Widget* Widget::get(const char* name)
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		if(stricmp((*i)->m_name.c_str(), name) == 0)
			return *i;

	return NULL;
}

void Widget::add(Widget* neww)
{
	if(!neww)
		OutOfMem(__FILE__, __LINE__);

	m_subwidg.push_back(neww);
}

void Widget::close()
{
	m_opened = false;

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->close();
}

void Widget::chcall(Widget* ch, int type, void* data)
{
}

void SubFrame(float *a, float *b, float *c)
{
	c[0] = fmax(a[0], b[0]);
	c[1] = fmax(a[1], b[1]);
	c[2] = fmin(a[2], b[2]);
	c[3] = fmin(a[3], b[3]);

	if(c[0] > c[2])
		c[0] = c[2];

	if(c[1] > c[3])
		c[1] = c[3];
}
