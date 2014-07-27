#ifndef BARBUTTON_H
#define BARBUTTON_H

#include "../widget.h"
#include "button.h"

class BarButton : public Button
{
public:
	float m_healthbar;

	BarButton(Widget* parent, unsigned int sprite, float bar, void (*reframef)(Widget* thisw), void (*click)(), void (*overf)(), void (*out)());

	void draw();
};

#endif
