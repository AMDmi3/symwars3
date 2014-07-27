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



InsDraw::InsDraw(Widget* parent, void (*inst)()) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_INSDRAW;
	clickfunc = inst;
	m_ldown = false;
}

void InsDraw::draw()
{
	if(clickfunc != NULL)
		clickfunc();
}

