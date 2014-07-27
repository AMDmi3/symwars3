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
#include "bottompanel.h"
#include "../../../platform.h"
#include "../viewportw.h"
#include "../../../../game/gui/gviewport.h"
#include "../../../sim/player.h"

BottomPanel::BottomPanel(Widget* parent, const char* n, void (*reframef)(Widget* thisw))
{
	m_parent = parent;
	m_type = WIDGET_BOTTOMPANEL;
	m_name = n;
	reframefunc = reframef;
	m_ldown = false;

	left_outer_toprightcorner = Image(this, "gui/frames/outertopleft64x64.png", true, NULL, 1, 1, 1, 1,		1, 0, 0, 1);
	left_outer_top = Image(this, "gui/frames/outertop2x64.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	left_minimap = ViewportW(this, "minimap viewport", NULL, &DrawViewport, &ViewportLDown, &ViewportLUp, &ViewportMousemove, &ViewportRDown, &ViewportRUp, ViewportMousewheel, VIEWPORT_MINIMAP);

	right_outer_topleftcorner = Image(this, "gui/frames/outertopleft64x64.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	right_outer_top = Image(this, "gui/frames/outertop2x64.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);

	middle_outer_top = Image(this, "gui/frames/outertop2x12.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);

	white_bg = Image(this, "gui/backg/white.jpg", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);

	for(int i=0; i<9; i++)
	{
		//bottomright_button[i] = Button(this, "gui/transp.png", RichText(""), MAINFONT8, NULL, NULL, NULL, NULL, i);
		//bottomright_button[i] = Button(this, "gui/brbut/apartment1.png", RichText(""), MAINFONT8, NULL, NULL, NULL, NULL, i);
		bottomright_button_on[i] = false;
	}

	reframe();
}

void BottomPanel::reframe()	//resized or moved
{
	if(reframefunc)
		reframefunc(this);

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->reframe();

	left_outer_toprightcorner.m_pos[0] = m_pos[0] + MINIMAP_SIZE - 32;
	left_outer_toprightcorner.m_pos[1] = m_pos[1];
	left_outer_toprightcorner.m_pos[2] = m_pos[0] + MINIMAP_SIZE + 32;
	left_outer_toprightcorner.m_pos[3] = m_pos[1] + 64;

	left_outer_toprightcorner.m_texc[3] = (left_outer_toprightcorner.m_pos[3]-left_outer_toprightcorner.m_pos[1])/64.0f;

	left_outer_top.m_pos[0] = m_pos[0];
	left_outer_top.m_pos[1] = m_pos[1];
	left_outer_top.m_pos[2] = m_pos[0] + MINIMAP_SIZE - 32;
	left_outer_top.m_pos[3] = m_pos[1] + 64;

	left_outer_top.m_texc[2] = (left_outer_toprightcorner.m_pos[2]-left_outer_toprightcorner.m_pos[0])/2.0f;

	left_minimap.m_pos[0] = m_pos[0] + MINIMAP_OFF;
	left_minimap.m_pos[1] = m_pos[1] + 32 - MINIMAP_OFF;
	left_minimap.m_pos[2] = m_pos[0] + MINIMAP_SIZE + MINIMAP_OFF;
	left_minimap.m_pos[3] = m_pos[1] + 32 + MINIMAP_SIZE - MINIMAP_OFF;

#if 0
	Image right_outer_topleftcorner;
	Image right_outer_top;
#endif
	right_outer_topleftcorner.m_pos[0] = m_pos[2] - MINIMAP_SIZE - 32;
	right_outer_topleftcorner.m_pos[1] = m_pos[1];
	right_outer_topleftcorner.m_pos[2] = m_pos[2] - MINIMAP_SIZE + 32;
	right_outer_topleftcorner.m_pos[3] = m_pos[1] + 64;

	right_outer_topleftcorner.m_texc[3] = (left_outer_toprightcorner.m_pos[3]-left_outer_toprightcorner.m_pos[1])/64.0f;

	right_outer_top.m_pos[0] = m_pos[2] - MINIMAP_SIZE + 32;
	right_outer_top.m_pos[1] = m_pos[1];
	right_outer_top.m_pos[2] = m_pos[2];
	right_outer_top.m_pos[3] = m_pos[1] + 64;

	middle_outer_top.m_pos[0] = m_pos[0] + MINIMAP_SIZE + 32 - 15;
	middle_outer_top.m_pos[1] = m_pos[1] + 64 - 12;
	middle_outer_top.m_pos[2] = m_pos[2] - MINIMAP_SIZE - 32 + 15;
	middle_outer_top.m_pos[3] = m_pos[1] + 64;

	white_bg.m_pos[0] = m_pos[0];
	white_bg.m_pos[1] = m_pos[1] + 64;
	white_bg.m_pos[2] = m_pos[2];
	white_bg.m_pos[3] = m_pos[3];

	float bottomright_left = m_pos[2] - MINIMAP_SIZE - MINIMAP_OFF;
	float bottomright_top = m_pos[1] + 32 - MINIMAP_OFF;

	for(int y=0; y<3; y++)
	{
		for(int x=0; x<3; x++)
		{
			int i = y*3 + x;

			Button* b = &bottomright_button[i];

			b->m_pos[0] = bottomright_left + MINIMAP_SIZE/3 * x;
			b->m_pos[1] = bottomright_top + MINIMAP_SIZE/3 * y;
			b->m_pos[2] = bottomright_left + MINIMAP_SIZE/3 * (x+1);
			b->m_pos[3] = bottomright_top + MINIMAP_SIZE/3 * (y+1);

			CenterLabel(b);
		}
	}

#if 0
	leftinnerdiagblur.m_pos[0] = m_pos[0];
	leftinnerdiagblur.m_pos[1] = m_pos[1];
	leftinnerdiagblur.m_pos[2] = m_pos[0]+32;
	leftinnerdiagblur.m_pos[3] = m_pos[1]+24;

	rightinnerdiagblur.m_pos[0] = m_pos[2]-32;
	rightinnerdiagblur.m_pos[1] = m_pos[1];
	rightinnerdiagblur.m_pos[2] = m_pos[2];
	rightinnerdiagblur.m_pos[3] = m_pos[1]+24;

	restext.m_pos[0] = m_pos[0]+32;
	restext.m_pos[1] = m_pos[1];
	restext.m_pos[2] = m_pos[2]-32;
	restext.m_pos[3] = m_pos[1]+24-3;

#if 1
	whitebg.m_pos[0] = m_pos[0]+32;
	whitebg.m_pos[1] = m_pos[1];
	whitebg.m_pos[2] = m_pos[2]-32;
	whitebg.m_pos[3] = m_pos[1]+24;
#else
	whitebg.m_pos[0] = m_pos[0];
	whitebg.m_pos[1] = m_pos[1];
	whitebg.m_pos[2] = m_pos[2];
	whitebg.m_pos[3] = m_pos[1]+24;
#endif

	innerbottom.m_pos[0] = m_pos[0]+32;
	innerbottom.m_pos[1] = m_pos[1]+24-3;
	innerbottom.m_pos[2] = m_pos[2]-32;
	innerbottom.m_pos[3] = m_pos[1]+24;

	float centerw = (m_pos[0]+m_pos[2])/2;

	lefthlineblur.m_pos[0] = m_pos[0]+32;
	lefthlineblur.m_pos[1] = m_pos[1]+24-3;
	lefthlineblur.m_pos[2] = centerw;
	lefthlineblur.m_pos[3] = m_pos[1]+24;

	righthlineblur.m_pos[0] = centerw;
	righthlineblur.m_pos[1] = m_pos[1]+24-3;
	righthlineblur.m_pos[2] = m_pos[2]-32;
	righthlineblur.m_pos[3] = m_pos[1]+24;
#endif
}

void BottomPanel::draw()
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

#if 0
	whitebg.draw();
#endif

	left_outer_toprightcorner.draw();
	left_outer_top.draw();

	right_outer_topleftcorner.draw();
	right_outer_top.draw();

	middle_outer_top.draw();

	white_bg.draw();
	left_minimap.draw();


	for(int y=0; y<3; y++)
	{
		for(int x=0; x<3; x++)
		{
			int i = y*3 + x;

			if(!bottomright_button_on[i])
				continue;

			Button* b = &bottomright_button[i];

			b->draw();
		}
	}

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->draw();

#if 0
	restext.draw();

	//RichText rt = RichText(")A)JJF)@J)(J)(F$KJ(0jfjfjoi3jfwkjlekf");
	//DrawShadowedTextF(MAINFONT16, m_pos[0]+32, m_pos[1]+4, 0, 0, 50, 50, &rt);

	leftinnerdiagblur.draw();
	rightinnerdiagblur.draw();
	lefthlineblur.draw();
	righthlineblur.draw();
	//innerbottom.draw();
#endif
}

void BottomPanel::drawover()
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->drawover();
}

void BottomPanel::inev(InEv* ev)
{
	for(int i=0; i<9; i++)
		if(bottomright_button_on[i])
			bottomright_button[i].inev(ev);

	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->inev(ev);
}

void BottomPanel::frameupd()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->frameupd();
}
