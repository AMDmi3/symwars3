#ifndef VIEWPORTW_H
#define VIEWPORTW_H

#include "../widget.h"

class ViewportW : public Widget
{
public:
	void (*drawfunc)(int p, int x, int y, int w, int h);
	bool (*ldownfunc)(int p, int relx, int rely, int w, int h);
	bool (*lupfunc)(int p, int relx, int rely, int w, int h);
	bool (*mousemovefunc)(int p, int relx, int rely, int w, int h);
	bool (*rdownfunc)(int p, int relx, int rely, int w, int h);
	bool (*rupfunc)(int p, int relx, int rely, int w, int h);
	bool (*mousewfunc)(int p, int d);
	bool (*mdownfunc)(int p, int relx, int rely, int w, int h);
	bool (*mupfunc)(int p, int relx, int rely, int w, int h);

	ViewportW();

	ViewportW(Widget* parent, const char* n, void (*reframef)(Widget* thisw),
			  void (*drawf)(int p, int x, int y, int w, int h),
			  bool (*ldownf)(int p, int relx, int rely, int w, int h),
			  bool (*lupf)(int p, int relx, int rely, int w, int h),
			  bool (*mousemovef)(int p, int relx, int rely, int w, int h),
			  bool (*rdownf)(int p, int relx, int rely, int w, int h),
			  bool (*rupf)(int p, int relx, int rely, int w, int h),
			  bool (*mousewf)(int p, int d),
			  int parm);
	void inev(InEv* ev);
	void draw();
};

#endif
