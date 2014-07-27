#ifndef TEXT_H
#define TEXT_H

#include "../widget.h"

class Text : public Widget
{
public:
	Text()
	{
		m_parent = NULL;
		m_type = WIDGET_TEXT;
		m_name = "";
		//m_text = "";
		m_font = MAINFONT8;
		reframefunc = NULL;
		m_ldown = false;
		m_rgba[0] = 1;
		m_rgba[1] = 1;
		m_rgba[2] = 1;
		m_rgba[3] = 1;
		m_shadow = false;
	}

	Text(Widget* parent, const char* n, const RichText& t, int f, void (*reframef)(Widget* thisw), bool shdw=true, float r=0.8f, float g=0.8f, float b=0.8f, float a=1) : Widget()
	{
		m_parent = parent;
		m_type = WIDGET_TEXT;
		m_name = n;
		//g_log<<"t.rawstr "<<t.rawstr()<<std::endl;
		//g_log.flush();

#ifdef USTR_DEBUG
		g_log<<"Text(Widget* parent, const char* n, const RichText t, int f, void (*reframef)(Widget* thisw), bool shdw=true, float r=0.8f, float g=0.8f, float b=0.8f, float a=1) : Widget()"<<std::endl;
		g_log.flush();
#endif

		m_text = t;

#ifdef USTR_DEBUG
		g_log<<"m_text end assign"<<std::endl;
		g_log.flush();
#endif

		m_font = f;
		reframefunc = reframef;
		m_ldown = false;
		//rgba[0] = rgba[1] = rgba[2] = 0.8f;
		//rgba[3] = 1.0f;
		m_rgba[0] = r;
		m_rgba[1] = g;
		m_rgba[2] = b;
		m_rgba[3] = a;
		m_shadow = shdw;
		reframe();
	}

	void draw();
};

#endif
