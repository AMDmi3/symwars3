#ifndef IMAGE_H
#define IMAGE_H

#include "../widget.h"

class Image : public Widget
{
public:
	Image();
	Image(Widget* parent, const char* nm, const char* filepath, bool clamp, void (*reframef)(Widget* thisw), float r=1, float g=1, float b=1, float a=1, float texleft=0, float textop=0, float texright=1, float texbottom=1);
	Image(Widget* parent, const char* filepath, bool clamp, void (*reframef)(Widget* thisw), float r=1, float g=1, float b=1, float a=1, float texleft=0, float textop=0, float texright=1, float texbottom=1);

	void draw();
	void reframe();
};

#endif
