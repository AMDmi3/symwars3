#ifndef EDITBOX_H
#define EDITBOX_H

#include "../widget.h"

class EditBox : public Widget
{
public:
	int m_highl[2];	// highlighted (selected) text
	UString m_compos;	//composition for unicode text
	void (*submitfunc)();
	void (*changefunc3)(unsigned int key, unsigned int scancode, bool down);

	EditBox();
	EditBox(Widget* parent, const char* n, const RichText t, int f, void (*reframef)(Widget* thisw), bool pw, int maxl, void (*change3)(unsigned int key, unsigned int scancode, bool down), void (*submitf)(), int parm);

	void draw();
	RichText drawvalue();
	void frameupd();
	void inev(InEv* ev);
	void placestr(const RichText* str);
	void changevalue(const char* str);
	bool delnext();
	bool delprev();
	void copyval();
	void pasteval();
	void selectall();
	void close();
	void gainfocus();
	void losefocus();
};

#endif
