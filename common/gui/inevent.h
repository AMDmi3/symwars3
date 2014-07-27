#ifndef INEV_H
#define INEV_H

#include "../platform.h"

#define INEV_MOUSEMOVE		0
#define INEV_MOUSEDOWN		1
#define INEV_MOUSEUP		2
#define INEV_KEYDOWN		3
#define INEV_KEYUP			4
#define INEV_CHARIN			5
#define INEV_MOUSEWHEEL		6
#define INEV_TEXTED			7
#define INEV_TEXTIN			8

#define MOUSE_LEFT			0
#define MOUSE_MIDDLE		1
#define MOUSE_RIGHT			2

class InEv
{
public:
	int type;
	int x;
	int y;
	unsigned int key;
	int scancode;
	int amount;
	bool intercepted;
	std::string text;	//UTF8
	int cursor;	//cursor pos in composition
	int sellen;	//selection length

	InEv();
};


#endif
