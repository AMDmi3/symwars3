#ifndef LISTBOX_H
#define LISTBOX_H

#include "../widget.h"

class ListBox : public Widget
{
public:

	int m_mousedown[2];

	ListBox(Widget* parent, const char* n, int f, void (*reframef)(Widget* thisw), void (*change)());

	void draw();
	void inev(InEv* ev);
	int rowsshown();
	int square();
	void erase(int which);

	float topratio()
	{
		return m_scroll[1] / (float)(m_options.size());
	}

	float bottomratio()
	{
		return (m_scroll[1]+rowsshown()) / (float)(m_options.size());
	}

	float scrollspace();
};

#endif
