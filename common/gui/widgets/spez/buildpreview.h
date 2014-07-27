#ifndef BUILDPREVIEW_H
#define BUILDPREVIEW_H

#include "../../widget.h"
#include "../viewportw.h"

class BuildPreview : public Widget
{
public:
	BuildPreview(Widget* parent, const char* n, void (*reframef)(Widget* thisw));

#if 0
	Text restext;
	Image leftinnerdiagblur;
	Image rightinnerdiagblur;
	Image innerbottom;
	Image lefthlineblur;
	Image righthlineblur;
	Image whitebg;
#endif

	Image left_outer_topleftcorner;
	Image left_outer;

	Image right_outer_toprightcorner;
	Image right_outer;

	Image middle_outer_top;
	Image middle_outer_bottom;

	ViewportW viewport;

	Image white_bg;

	void draw();
	void drawover();
	void reframe();
	void inev(InEv* ev);
	void frameupd();
};

#endif
