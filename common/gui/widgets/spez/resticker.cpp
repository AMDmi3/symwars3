#include "../../widget.h"
#include "../barbutton.h"
#include "../button.h"
#include "../checkbox.h"
#include "../editbox.h"
#include "../dropdowns.h"
#include "../image.h"
#include "../insdraw.h"
#include "../link.h"
#include "../listbox.h"
#include "../text.h"
#include "../textarea.h"
#include "../textblock.h"
#include "../touchlistener.h"
#include "../frame.h"
#include "resticker.h"
#include "../../../platform.h"
#include "../../../sim/player.h"

ResTicker::ResTicker(Widget* parent, const char* n, void (*reframef)(Widget* thisw))
{
	m_parent = parent;
	m_type = WIDGET_RESTICKER;
	m_name = n;
	reframefunc = reframef;
	m_ldown = false;

#if 0
	Text restext;
	Image leftinnerdiagblur;
	Image rightinnerdiagblur;
	Image innerbottom;
	Image lefthlineblur;
	Image righthlineblur;
	Image whitebg;
#endif
	
	//restext = Text(this, "res ticker", RichText("asdadasdasads"), MAINFONT16, NULL, true, 1, 1, 1, 1);
	restext = Text(this, "res ticker", RichText("asdadasdasads"), MAINFONT16, NULL, false, 0.2, 0.7, 0.8, 1);
#if 0
	leftinnerdiagblur = Image(this, "gui/frames/innerdiagblur32x24halfwht.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	rightinnerdiagblur = Image(this, "gui/frames/innerdiagblur32x24halfwht.png", true, NULL, 1, 1, 1, 1,		1, 0, 0, 1);
	innerbottom = Image(this, "gui/frames/innerbottom3x3.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	lefthlineblur = Image(this, "gui/frames/innerhlineblur30x3.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	righthlineblur = Image(this, "gui/frames/innerhlineblur30x3.png", true, NULL, 1, 1, 1, 1,		1, 0, 0, 1);
#endif
	whitebg = Image(this, "gui/backg/white.jpg", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);

	reframe();
}

#define RESTICKER_HEIGHT	24

void ResTicker::reframe()	//resized or moved
{
	if(reframefunc)
		reframefunc(this);

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->reframe();

#if 0
	whitebg.m_pos[0] = m_pos[0]+32;
	whitebg.m_pos[1] = m_pos[1];
	whitebg.m_pos[2] = m_pos[2]-32;
	whitebg.m_pos[3] = m_pos[1]+RESTICKER_HEIGHT;
	
	restext.m_pos[0] = m_pos[0]+32;
	restext.m_pos[1] = m_pos[1];
	restext.m_pos[2] = m_pos[2]-32;
	restext.m_pos[3] = m_pos[1]+RESTICKER_HEIGHT-3;
#else
	whitebg.m_pos[0] = m_pos[0];
	whitebg.m_pos[1] = m_pos[1];
	whitebg.m_pos[2] = m_pos[2];
	whitebg.m_pos[3] = m_pos[1]+24;
	
	restext.m_pos[0] = m_pos[0];
	restext.m_pos[1] = m_pos[1];
	restext.m_pos[2] = m_pos[2];
	restext.m_pos[3] = m_pos[1]+24;
#endif

#if 0
	leftinnerdiagblur.m_pos[0] = m_pos[0];
	leftinnerdiagblur.m_pos[1] = m_pos[1];
	leftinnerdiagblur.m_pos[2] = m_pos[0]+32;
	leftinnerdiagblur.m_pos[3] = m_pos[1]+RESTICKER_HEIGHT;

	rightinnerdiagblur.m_pos[0] = m_pos[2]-32;
	rightinnerdiagblur.m_pos[1] = m_pos[1];
	rightinnerdiagblur.m_pos[2] = m_pos[2];
	rightinnerdiagblur.m_pos[3] = m_pos[1]+RESTICKER_HEIGHT;

	innerbottom.m_pos[0] = m_pos[0]+32;
	innerbottom.m_pos[1] = m_pos[1]+RESTICKER_HEIGHT-3;
	innerbottom.m_pos[2] = m_pos[2]-32;
	innerbottom.m_pos[3] = m_pos[1]+RESTICKER_HEIGHT;

	float centerw = (m_pos[0]+m_pos[2])/2;

	lefthlineblur.m_pos[0] = m_pos[0]+32;
	lefthlineblur.m_pos[1] = m_pos[1]+RESTICKER_HEIGHT-3;
	lefthlineblur.m_pos[2] = centerw;
	lefthlineblur.m_pos[3] = m_pos[1]+RESTICKER_HEIGHT;

	righthlineblur.m_pos[0] = centerw;
	righthlineblur.m_pos[1] = m_pos[1]+RESTICKER_HEIGHT-3;
	righthlineblur.m_pos[2] = m_pos[2]-32;
	righthlineblur.m_pos[3] = m_pos[1]+RESTICKER_HEIGHT;
#endif

}

void ResTicker::draw()
{
#if 0
	Text restext;
	Image leftinnerdiagblur;
	Image rightinnerdiagblur;
	Image innerbottom;
	Image lefthlineblur;
	Image righthlineblur;
	Image whitebg;
#endif

	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	whitebg.draw();

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->draw();


	//RichText rt = RichText(")A)JJF)@J)(J)(F$KJ(0jfjfjoi3jfwkjlekf");
	//DrawShadowedTextF(MAINFONT16, m_pos[0]+32, m_pos[1]+4, 0, 0, 50, 50, &rt);
	restext.draw();

#if 0
	leftinnerdiagblur.draw();
	rightinnerdiagblur.draw();
	lefthlineblur.draw();
	righthlineblur.draw();
	//innerbottom.draw();
#endif
}

void ResTicker::drawover()
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->drawover();
}

void ResTicker::inev(InEv* ev)
{
	Player* py = &g_player[g_curP];

	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->inev(ev);
}

void ResTicker::frameupd()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->frameupd();
}
