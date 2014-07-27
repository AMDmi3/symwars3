#ifndef TOUCHLISTENER_H
#define TOUCHLISTENER_H

#include "../widget.h"

class TouchListener : public Widget
{
public:
	TouchListener();
	TouchListener(Widget* parent, void (*reframef)(Widget* thisw), void (*click2)(int p), void (*overf)(int p), void (*out)(), int parm);

	void inev(InEv* ev);
};

#endif
