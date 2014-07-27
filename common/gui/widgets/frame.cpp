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
#include "frame.h"
#include "../../platform.h"

Frame::Frame(Widget* parent, const char* n, void (*reframef)(Widget* thisw))
{
	m_parent = parent;
	m_type = WIDGET_FRAME;
	m_name = n;
	reframefunc = reframef;
	m_ldown = false;
	reframe();
}

void Frame::draw()
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->draw();
}

void Frame::drawover()
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->drawover();
}

void Frame::frameupd()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->frameupd();
}

void Frame::inev(InEv* ev)
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->inev(ev);
}
