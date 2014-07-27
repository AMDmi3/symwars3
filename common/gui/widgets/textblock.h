#ifndef TEXTBLOCK_H
#define TEXTBLOCK_H

#include "../widget.h"

class TextBlock : public Widget
{
public:

	TextBlock(Widget* parent, const char* n, const RichText t, int f, void (*reframef)(Widget* thisw), float r=1, float g=1, float b=1, float a=1) : Widget()
	{
		m_parent = parent;
		m_type = WIDGET_TEXTBLOCK;
		m_name = n;
		m_text = t;
		m_font = f;
		reframefunc = reframef;
		m_ldown = false;
		m_rgba[0] = r;
		m_rgba[1] = g;
		m_rgba[2] = b;
		m_rgba[3] = a;
		reframe();
	}

	void draw();
	//bool lbuttonup(bool moved);
	//bool lbuttondown();
	//bool mousemove();
	void changevalue(const char* newv);
	int square();
};

#endif
