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
#include "constructionview.h"
#include "../../../platform.h"
#include "../viewportw.h"
#include "../../../../game/gui/gviewport.h"
#include "../../../sim/building.h"
#include "../../../sim/buildingtype.h"
#include "../../../sim/road.h"
#include "../../../sim/crpipe.h"
#include "../../../sim/powl.h"
#include "../../../sim/player.h"

ConstructionView::ConstructionView(Widget* parent, const char* n, void (*reframef)(Widget* thisw), void (*movefunc)(), void (*cancelfunc)(), void (*proceedfunc)(), void (*estimatefunc)())
{
	m_parent = parent;
	m_type = WIDGET_CONSTRUCTIONVIEW;
	m_name = n;
	reframefunc = reframef;
	m_ldown = false;
	this->movefunc = movefunc;
	this->cancelfunc = cancelfunc;
	this->proceedfunc = proceedfunc;
	this->estimatefunc = estimatefunc;

#if 0
	Image left_outer_topleftcorner;
	Image left_outer;

	Image right_outer_toprightcorner;
	Image right_outer;

	Image middle_outer_top;
	Image middle_outer_bottom;

	ViewportW viewport;

	Image white_bg;
#endif

#if 0
	restext = Text(this, "res ticker", RichText("asdadasdasads"), MAINFONT16, NULL, true, 1, 1, 1, 1);
	leftinnerdiagblur = Image(this, "gui/frames/innerdiagblur32x24halfwht.png", NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	rightinnerdiagblur = Image(this, "gui/frames/innerdiagblur32x24halfwht.png", NULL, 1, 1, 1, 1,		1, 0, 0, 1);
	innerbottom = Image(this, "gui/frames/innerbottom3x3.png", NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	lefthlineblur = Image(this, "gui/frames/innerhlineblur30x3.png", NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	righthlineblur = Image(this, "gui/frames/innerhlineblur30x3.png", NULL, 1, 1, 1, 1,		1, 0, 0, 1);
	whitebg = Image(this, "gui/backg/white.jpg", NULL, 1, 1, 1, 1,		0, 0, 1, 1);
#endif

	viewport = ViewportW(this, "viewport", NULL, &DrawViewport, &ViewportLDown, &ViewportLUp, &ViewportMousemove, &ViewportRDown, &ViewportRUp, ViewportMousewheel, VIEWPORT_ENTVIEW);

	left_outer_topleftcorner = Image(this, "gui/frames/outertopleft64x64.png", true, NULL, 1, 1, 1, 1,		1, 0, 0, 1);
	right_outer_toprightcorner = Image(this, "gui/frames/outertopleft64x64.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	middle_outer_top = Image(this, "gui/frames/outertop2x64.png", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	middle_outer_bottom = Image(this, "gui/frames/outertop2x64.png", true, NULL, 1, 1, 1, 1,		0, 1, 1, 0);

	white_bg = Image(this, "gui/backg/white.jpg", true, NULL, 1, 1, 1, 1,		0, 0, 1, 1);
	bg_touch = TouchListener(this, NULL, NULL, NULL, NULL, -1);

	//RichText inititle("Construction Manager");
	//titletext = Text(this, "title", inititle, MAINFONT16, NULL);
	//titletext = Text(this, "title", RichText("Construction Manager"), MAINFONT16, NULL, true, 0.8f, 0.7f, 0.1f, 1.0f);
	//titletext = Text(this, "title", RichText("Construction Manager"), MAINFONT16, NULL);
	titletext = Text(this, "title", RichText("Construction Manager"), MAINFONT32, NULL, false, 0.8f, 0.7f, 0.1f, 1.0f);

#if 0
	Button move_button;
	Button cancel_button;
	Button proceed_button;
#endif

	move_button = Button(this, "", "gui/move.png", RichText("Choose another spot"), RichText(""), MAINFONT16, BUTTON_LEFTIMAGE, NULL, movefunc, NULL, NULL, NULL, NULL, -1);
	cancel_button = Button(this, "", "gui/cancel.png", RichText("Cancel construction"), RichText(""), MAINFONT16, BUTTON_LEFTIMAGE, NULL, cancelfunc, NULL, NULL, NULL, NULL, -1);
	proceed_button = Button(this, "", "gui/accept.png", RichText("Proceed with plan"), RichText(""), MAINFONT16, BUTTON_LEFTIMAGE, NULL, proceedfunc, NULL, NULL, NULL, NULL, -1);
	estimate_button = Button(this, "", "gui/icons/dollars.png", RichText("Estimate cost"), RichText(""), MAINFONT16, BUTTON_LEFTIMAGE, NULL, estimatefunc, NULL, NULL, NULL, NULL, -1);

	reframe();
}

ConstructionView::ReqRow::ReqRow()
{
}

void ConstructionView::regen(Selection* sel)
{
	int* conmat = NULL;
	int qty = -1;
	int* maxcost = NULL;

	Player* py = &g_player[g_curP];

	if(sel->buildings.size() > 0)
	{
		int bi = *sel->buildings.begin();
		Building* b = &g_building[bi];
		BuildingT* t = &g_bltype[b->type];

		//RichText newtitle;
		//newtitle.m_part.push_back( RichTextP(t->name) );
		//newtitle.m_part.push_back( RichTextP(" Construction Manager") );

		//titletext.m_text = RichText(t->name);
		//titletext.m_text.m_part.push_back( RichTextP(" Construction Manager") );
		//titletext.m_text = newtitle;

		conmat = t->conmat;
		py->bptype = b->type;
		maxcost = b->maxcost;
	}
#if 0
	else if(sel->roads.size() > 0)
	{
		py->bptype = BUILDING_ROAD;
		conmat = g_roadcost;
		qty = sel->roads.size();
		Vec2i xz = *sel->roads.begin();
		RoadTile* road = RoadAt(xz.x, xz.y);
		maxcost = road->maxcost;
	}
	else if(sel->powls.size() > 0)
	{
		py->bptype = BUILDING_POWL;
		conmat = g_powlcost;
		qty = sel->powls.size();
		Vec2i xz = *sel->powls.begin();
		PowlTile* powl = PowlAt(xz.x, xz.y);
		maxcost = powl->maxcost;
	}
	else if(sel->crpipes.size() > 0)
	{
		py->bptype = BUILDING_CRPIPE;
		qty = sel->crpipes.size();
		conmat = g_crpipecost;
		Vec2i xz = *sel->crpipes.begin();
		CrPipeTile* crpipe = CrPipeAt(xz.x, xz.y);
		maxcost = crpipe->maxcost;
	}
#endif

	reqrows.clear();

	if(conmat)
	{
		float color[] = {0.1f, 0.7f, 0.8f, 1.0f};
		int i=0;
		int font = MAINFONT16;

		for(; i<RESOURCES; i++)
		{
			if(i == RES_LABOUR)
				continue;

			if(conmat[i] <= 0)
				continue;

			ReqRow reqrow;

			reqrow.restype = i;

			RichText textline1rt;
			char textline1cstr[256];
			if(qty < 0)
			{
				sprintf(textline1cstr, "Requisition %d", conmat[i]);
			}
			else if(qty >= 0)
			{
				sprintf(textline1cstr, "Requisition %dx%d", qty, conmat[i]);
			}
			textline1rt.m_part.push_back( RichTextP(textline1cstr) );
			textline1rt.m_part.push_back( RichTextP(RICHTEXT_ICON, g_resource[i].icon) );
			textline1rt.m_part.push_back( RichTextP(g_resource[i].name.c_str()) );
			//reqrow.textline = Text(this, "textline1", textline1rt, MAINFONT16, NULL);
			reqrow.textline = Text(this, "textline1", textline1rt, font, NULL, false, color[0], color[1], color[2], color[3]);

			RichText textline2rt;
			textline2rt.m_part.push_back( RichTextP("Max. est. cost:") );
			reqrow.textline2 = Text(this, "textline2", textline2rt, font, NULL, false, color[0], color[1], color[2], color[3]);

			RichText dollarsignrt;
			dollarsignrt.m_part.push_back( RichTextP(RICHTEXT_ICON, g_resource[RES_FUNDS].icon) );
			reqrow.dollarsign = Text(this, "dollarsign", dollarsignrt, font, NULL, false, color[0], color[1], color[2], color[3]);

			char edittext[256];
			if(maxcost)
			{
				sprintf(edittext, "%d", maxcost[i]);
			}
			else
			{
				sprintf(edittext, "0");
			}
			RichText editrt;
			editrt.m_part.push_back( RichTextP(edittext) );
			reqrow.editbox = EditBox(this, "editbox", editrt, font, NULL, false, 6, NULL, NULL, -1);

			RichText perunitrt;
			char perunitcstr[256];
			if(qty < 0)
			{
				sprintf(perunitcstr, "");
			}
			else if(qty >= 0)
			{
				sprintf(perunitcstr, "/tile segment");
			}
			perunitrt.m_part.push_back( RichTextP(perunitcstr) );
			reqrow.perunittext = Text(this, "perunittext", perunitrt, font, NULL, false, color[0], color[1], color[2], color[3]);

			reqrows.push_back(reqrow);
		}

		i = RES_LABOUR;

		while(true)
		{
			if(conmat[i] <= 0)
				break;

			ReqRow reqrow;

			reqrow.restype = i;

			RichText textline1rt;
			char textline1cstr[256];
			if(qty < 0)
			{
				sprintf(textline1cstr, "Requisition %d", conmat[i]);
			}
			else if(qty >= 0)
			{
				sprintf(textline1cstr, "Requisition %dx%d", qty, conmat[i]);
			}
			textline1rt.m_part.push_back( RichTextP(textline1cstr) );
			textline1rt.m_part.push_back( RichTextP(RICHTEXT_ICON, g_resource[i].icon) );
			textline1rt.m_part.push_back( RichTextP(g_resource[i].name.c_str()) );
			//reqrow.textline = Text(this, "textline1", textline1rt, MAINFONT16, NULL);
			reqrow.textline = Text(this, "textline1", textline1rt, font, NULL, false, color[0], color[1], color[2], color[3]);

			RichText textline2rt;
			textline2rt.m_part.push_back( RichTextP("Construction wage:") );
			reqrow.textline2 = Text(this, "textline2", textline2rt, font, NULL, false, color[0], color[1], color[2], color[3]);

			RichText dollarsignrt;
			dollarsignrt.m_part.push_back( RichTextP(RICHTEXT_ICON, g_resource[RES_FUNDS].icon) );
			reqrow.dollarsign = Text(this, "dollarsign", dollarsignrt, font, NULL, false, color[0], color[1], color[2], color[3]);

			char edittext[256];
			if(maxcost)
			{
				sprintf(edittext, "%d", maxcost[i]);
			}
			else
			{
				sprintf(edittext, "0");
			}
			RichText editrt;
			editrt.m_part.push_back( RichTextP(edittext) );
			reqrow.editbox = EditBox(this, "editbox", editrt, font, NULL, false, 6, NULL, NULL, -1);

			RichText perunitrt;
			char perunitcstr2[256];
			sprintf(perunitcstr2, "labour");
			perunitrt.m_part.push_back( RichTextP("/") );
			perunitrt.m_part.push_back( RichTextP(RICHTEXT_ICON, g_resource[i].icon) );
			perunitrt.m_part.push_back( RichTextP(perunitcstr2) );
			reqrow.perunittext = Text(this, "perunittext", perunitrt, font, NULL, false, color[0], color[1], color[2], color[3]);

			reqrows.push_back(reqrow);

			break;
		}
	}

	reframe();
}

void ConstructionView::reframe()	//resized or moved
{
	if(reframefunc)
		reframefunc(this);

	Player* py = &g_player[g_curP];

	viewport.m_pos[0] = m_pos[0] + 10;
	viewport.m_pos[1] = m_pos[1] + 10;
	viewport.m_pos[2] = (m_pos[0]+m_pos[2])/2;
	viewport.m_pos[3] = (m_pos[1]+m_pos[3])/2;

	white_bg.m_pos[0] = m_pos[0];
	white_bg.m_pos[1] = m_pos[1];
	white_bg.m_pos[2] = m_pos[2];
	white_bg.m_pos[3] = m_pos[3];

	bg_touch.m_pos[0] = 0;
	bg_touch.m_pos[1] = 0;
	bg_touch.m_pos[2] = py->width;
	bg_touch.m_pos[3] = py->height;

	titletext.m_pos[0] = m_pos[0] + 5;
	titletext.m_pos[1] = m_pos[1] + 5;
	titletext.m_pos[2] = m_pos[2];
	titletext.m_pos[3] = m_pos[3];

#if 0
	class ReqRow
	{
	public:
		int restype;
		Text textline;
		Text textline2;
		Text dollarsign;
		EditBox editbox;
		Text perunittext;
	};

	std::list<ReqRow> reqrows;
#endif

	int rowi = 0;
	int gheight = g_font[MAINFONT16].gheight;
	for(auto i=reqrows.begin(); i!=reqrows.end(); i++)
	{
		int line1y = rowi * gheight*3 + (m_pos[1]+m_pos[3])/2.0f + 10;
		int line2y = line1y + gheight;

		Text* textline = &i->textline;
		textline->m_pos[0] = m_pos[0] + 10;
		textline->m_pos[1] = line1y;
		textline->m_pos[2] = m_pos[2] - 10;
		textline->m_pos[3] = line1y + gheight*2;

		Text* textline2 = &i->textline2;
		textline2->m_pos[0] = m_pos[0] + 10;
		textline2->m_pos[1] = line2y;
		textline2->m_pos[2] = m_pos[2] - 10;
		textline2->m_pos[3] = line2y + gheight*2;

		Text* dollarsign = &i->dollarsign;
		dollarsign->m_pos[0] = m_pos[0] + 10 + 200;
		dollarsign->m_pos[1] = line2y;
		dollarsign->m_pos[2] = m_pos[2] - 10;
		dollarsign->m_pos[3] = line2y + gheight*2;

		float endx = EndX(&dollarsign->m_text, dollarsign->m_text.texlen(), dollarsign->m_font, dollarsign->m_pos[0], dollarsign->m_pos[1]);

		EditBox* editbox = &i->editbox;
		editbox->m_pos[0] = endx + 2;
		editbox->m_pos[1] = line2y;
		editbox->m_pos[2] = endx + 2 + 64;
		editbox->m_pos[3] = line2y + gheight;

		endx += 2 + 64;

		Text* perunittext = &i->perunittext;
		perunittext->m_pos[0] = endx + 2;
		perunittext->m_pos[1] = line2y;
		perunittext->m_pos[2] = m_pos[2] - 10;
		perunittext->m_pos[3] = line2y + gheight*2;

		rowi++;
	}

#if 0
	Button move_button;
	Button cancel_button;
	Button proceed_button;
#endif

	float midx = (m_pos[0]+m_pos[2])/2.0f;

	float button_height = 20;
	float button_space = 10;

	move_button.m_pos[0] = midx + 5;
	move_button.m_pos[1] = m_pos[1] + 50 + button_height*0 + button_space*0;
	move_button.m_pos[2] = midx + 5 + 180;
	move_button.m_pos[3] = m_pos[1] + 50 + button_height*1 + button_space*0;

	cancel_button.m_pos[0] = midx + 5;
	cancel_button.m_pos[1] = m_pos[1] + 50 + button_height*1 + button_space*1;
	cancel_button.m_pos[2] = midx + 5 + 180;
	cancel_button.m_pos[3] = m_pos[1] + 50 + button_height*2 + button_space*1;

	proceed_button.m_pos[0] = midx + 5;
	proceed_button.m_pos[1] = m_pos[1] + 50 + button_height*2 + button_space*2;
	proceed_button.m_pos[2] = midx + 5 + 180;
	proceed_button.m_pos[3] = m_pos[1] + 50 + button_height*3 + button_space*2;

	estimate_button.m_pos[0] = midx + 5;
	estimate_button.m_pos[1] = m_pos[1] + 50 + button_height*3 + button_space*3;
	estimate_button.m_pos[2] = midx + 5 + 180;
	estimate_button.m_pos[3] = m_pos[1] + 50 + button_height*4 + button_space*3;

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->reframe();
}

void ConstructionView::draw()
{
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	middle_outer_top.draw();
	white_bg.draw();
	titletext.draw();
	viewport.draw();

	for(auto i=reqrows.begin(); i!=reqrows.end(); i++)
	{
		i->textline.draw();
		i->textline2.draw();
		i->dollarsign.draw();
		i->editbox.draw();
		i->perunittext.draw();
	}

	move_button.draw();
	cancel_button.draw();
	proceed_button.draw();
	estimate_button.draw();

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

void ConstructionView::drawover()
{
	for(auto i=reqrows.begin(); i!=reqrows.end(); i++)
	{
		i->textline.drawover();
		i->textline2.drawover();
		i->dollarsign.drawover();
		i->editbox.drawover();
		i->perunittext.drawover();
	}

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->drawover();
}

void ConstructionView::inev(InEv* ev)
{
	for(auto i=reqrows.begin(); i!=reqrows.end(); i++)
		i->editbox.inev(ev);

	move_button.inev(ev);
	cancel_button.inev(ev);
	proceed_button.inev(ev);
	estimate_button.inev(ev);

	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->inev(ev);

	//bg_touch.inev(ev);
}

void ConstructionView::frameupd()
{
	for(auto i=reqrows.begin(); i!=reqrows.end(); i++)
		i->editbox.frameupd();

	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->frameupd();
}
