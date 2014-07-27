#ifndef VSCROLLBAR_H
#define VSCROLLBAR_H

#include "../widget.h"
#include "image.h"
#include "../cursor.h"

class VScroll : public Widget
{
public:

	struct ScrollEv
	{
		float delta;
		float newpos;
	};

	float m_domain;
	float m_barpos[4];
	float m_uppos[4];
	float m_downpos[4];
	int m_mousedown[2];
	bool m_ldownbar;

	VScroll();
	VScroll(Widget* parent, const char* n);

	void inev(InEv* ev);
	void draw();
	void drawover();
	void reframe();
};

#endif
