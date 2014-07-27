#ifndef FRAME_H
#define FRAME_H

#include "../widget.h"

class Frame : public Widget
{
public:
	Frame(Widget* parent, const char* n, void (*reframef)(Widget* thisw));

	void draw();
	void drawover();
	void inev(InEv* ev);
	void frameupd();
};

#endif
