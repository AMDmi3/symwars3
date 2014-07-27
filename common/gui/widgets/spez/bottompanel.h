#ifndef BOTTOMPANEL_H
#define BOTTOMPANEL_H

#include "../../widget.h"
#include "../viewportw.h"

#define MINIMAP_SIZE				100
#define BOTTOM_MID_PANEL_HEIGHT		200
#define MINIMAP_OFF					5

class BottomPanel : public Widget
{
public:
	BottomPanel(Widget* parent, const char* n, void (*reframef)(Widget* thisw));

	Image left_outer_toprightcorner;
	Image left_outer_top;
	ViewportW left_minimap;

	Image right_outer_topleftcorner;
	Image right_outer_top;

	Image middle_outer_top;

	Image white_bg;

	Button bottomright_button[9];
	bool bottomright_button_on[9];

	void draw();
	void drawover();
	void reframe();
	void inev(InEv* ev);
	void frameupd();
};

#endif
