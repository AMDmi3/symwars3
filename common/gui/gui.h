#ifndef GUI_H
#define GUI_H

#include "../platform.h"
#include "../math/3dmath.h"
#include "viewlayer.h"
#include "widgets/image.h"
#include "widgets/barbutton.h"
#include "widgets/button.h"
#include "widgets/checkbox.h"
#include "widgets/dropdowns.h"
#include "widgets/editbox.h"
#include "widgets/insdraw.h"
#include "widgets/link.h"
#include "widgets/listbox.h"
#include "widgets/text.h"
#include "widgets/textarea.h"
#include "widgets/textblock.h"
#include "widgets/touchlistener.h"
#include "widgets/frame.h"
#include "widgets/viewportw.h"

class GUI : public Widget
{
public:
	std::list<ViewLayer> view;
	void (*keyupfunc[SDL_NUM_SCANCODES])();
	void (*keydownfunc[SDL_NUM_SCANCODES])();
	void (*anykeyupfunc)(int k);
	void (*anykeydownfunc)(int k);
	void (*mousemovefunc)();
	void (*lbuttondownfunc)();
	void (*lbuttonupfunc)();
	void (*rbuttondownfunc)();
	void (*rbuttonupfunc)();
	void (*mbuttondownfunc)();
	void (*mbuttonupfunc)();
	void (*mousewheelfunc)(int delta);
	Widget* activewidg;

	GUI() : Widget()
	{
		m_type = WIDGET_GUI;

		for(int i=0; i<256; i++)
		{
			keyupfunc[i] = NULL;
			keydownfunc[i] = NULL;
		}

		mousemovefunc = NULL;
	}

	void assignmousewheel(void (*wheel)(int delta))
	{
		mousewheelfunc = wheel;
	}

	void assignlbutton(void (*down)(), void (*up)())
	{
		lbuttondownfunc = down;
		lbuttonupfunc = up;
	}

	void assignrbutton(void (*down)(), void (*up)())
	{
		rbuttondownfunc = down;
		rbuttonupfunc = up;
	}

	void assignmbutton(void (*down)(), void (*up)())
	{
		mbuttondownfunc = down;
		mbuttonupfunc = up;
	}

	void assignmousemove(void (*mouse)())
	{
		mousemovefunc = mouse;
	}

	void assignkey(int i, void (*down)(), void (*up)())
	{
		keydownfunc[i] = down;
		keyupfunc[i] = up;
	}

	void assignanykey(void (*down)(int k), void (*up)(int k))
	{
		anykeydownfunc = down;
		anykeyupfunc = up;
	}

	void draw();
	void inev(InEv* ev);
	void closeall();
	void close(const char* name);
	void open(const char* name);
	void reframe();
};

bool MousePosition();
void CenterMouse();
void Status(const char* status, bool logthis=false);
void Ortho(int width, int height, float r, float g, float b, float a);

#endif
