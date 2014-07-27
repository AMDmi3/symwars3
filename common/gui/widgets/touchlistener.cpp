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

TouchListener::TouchListener() : Widget()
{
	m_parent = NULL;
	m_type = WIDGET_TOUCHLISTENER;
	m_over = false;
	m_ldown = false;
	reframefunc = NULL;
	clickfunc = NULL;
	overfunc = NULL;
	clickfunc2 = NULL;
	overfunc2 = NULL;
	outfunc = NULL;
	m_param = -1;
}

TouchListener::TouchListener(Widget* parent, void (*reframef)(Widget* thisw), void (*click2)(int p), void (*overf2)(int p), void (*out)(), int parm) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_TOUCHLISTENER;
	m_over = false;
	m_ldown = false;
	reframefunc = reframef;
	clickfunc = NULL;
	overfunc = NULL;
	clickfunc2 = click2;
	overfunc2 = overf2;
	outfunc = out;
	m_param = parm;
	reframe();
}

void TouchListener::inev(InEv* ev)
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
	else if(ev->type == INEV_MOUSEMOVE && !ev->intercepted)
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
		else
		{
			if(m_over && outfunc != NULL)
				outfunc();

			m_over = false;
		}
	}
}
