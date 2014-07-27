#ifndef CONSTRUCTIONVIEW_H
#define CONSTRUCTIONVIEW_H

#include "../../../platform.h"
#include "../button.h"
#include "../image.h"
#include "../text.h"
#include "../editbox.h"
#include "../touchlistener.h"
#include "../../widget.h"
#include "../viewportw.h"
#include "../../../sim/selection.h"

class ConstructionView : public Widget
{
public:
	ConstructionView(Widget* parent, const char* n, void (*reframef)(Widget* thisw), void (*movefunc)(), void (*cancelfunc)(), void (*proceedfunc)(), void (*estimatefunc)());

	Image left_outer_topleftcorner;
	Image left_outer;

	Image right_outer_toprightcorner;
	Image right_outer;

	Image middle_outer_top;
	Image middle_outer_bottom;

	ViewportW viewport;

	Image white_bg;
	TouchListener bg_touch;

	Text titletext;

	void (*movefunc)();
	void (*cancelfunc)();
	void (*proceedfunc)();
	void (*estimatefunc)();

	class ReqRow
	{
	public:
		ReqRow();

		int restype;
		Text textline;
		Text textline2;
		Text dollarsign;
		EditBox editbox;
		Text perunittext;
	};

	std::list<ReqRow> reqrows;

	Button move_button;
	Button cancel_button;
	Button proceed_button;
	Button estimate_button;

	void draw();
	void drawover();
	void reframe();
	void inev(InEv* ev);
	void frameupd();
	void regen(Selection* sel);
};

#endif
