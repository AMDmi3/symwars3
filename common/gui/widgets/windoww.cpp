#include "../gui.h"
#include "../../texture.h"
#include "../../sim/player.h"
#include "windoww.h"
#include "../../debug.h"

WindowW::WindowW() : Widget()
{
	m_type = WIDGET_WINDOW;
	m_parent = NULL;
	m_opened = false;
}

void WindowW::chcall(Widget* ch, int type, void* data)
{
	if(ch == &vscroll)
	{

	}
	//else if(ch == &hscroll)
	{
	}
}

WindowW::WindowW(Widget* parent, const char* n, void (*reframef)(Widget* thisw)) : Widget()
{
	m_type = WIDGET_WINDOW;
	reframefunc = reframef;
	m_parent = parent;
	m_name = n;
	m_opened = false;
	m_scroll[0] = 0;
	m_scroll[1] = 0;

	const float alpha = 0.9f;

	CheckGLError(__FILE__, __LINE__);

	top_image = Image(this, "gui/frames/outertop2x64.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);

	CheckGLError(__FILE__, __LINE__);
	topleft_image = Image(this, "gui/frames/outertopleft64x64.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);

	CheckGLError(__FILE__, __LINE__);
	topright_image = Image(this, "gui/frames/outertopleft64x64.png", true, NULL, 1, 1, 1, alpha,		1, 0, 0, 1);

	CheckGLError(__FILE__, __LINE__);
	left_image = Image(this, "gui/frames/outerleft64x2.png", false, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);

	CheckGLError(__FILE__, __LINE__);
	right_image = Image(this, "gui/frames/outerleft64x2.png", false, NULL, 1, 1, 1, alpha,		1, 0, 0, 1);

	CheckGLError(__FILE__, __LINE__);
	bottom_image = Image(this, "gui/frames/outertop2x64.png", true, NULL, 1, 1, 1, alpha,		0, 1, 1, 0);

	CheckGLError(__FILE__, __LINE__);
	bottomleft_image = Image(this, "gui/frames/outertopleft64x64.png", true, NULL, 1, 1, 1, alpha,		0, 1, 1, 0);
	bottomright_image = Image(this, "gui/frames/outertopleft64x64.png", true, NULL, 1, 1, 1, alpha,		1, 1, 0, 0);
	bg_image = Image(this, "gui/backg/white.jpg", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	bg_logo_image = Image(this, "gui/centerp/pcsgray.png", true, NULL, 1, 1, 1, alpha/2.0f,		0, 0, 1, 1);

	inner_top_mid_image = Image(this, "gui/frames/innerbottom3x3.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_top_leftdiag_image = Image(this, "gui/frames/innerdiagblur32x24.png", true, NULL, 1, 1, 1, alpha,		0, 1, 1, 0);
	inner_top_rightdiag_image = Image(this, "gui/frames/innerdiagblur32x24.png", true, NULL, 1, 1, 1, alpha,		1, 1, 0, 0);
	inner_top_leftstrip_image = Image(this, "gui/frames/innerbottom3x3.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_top_rightstrip_image = Image(this, "gui/frames/innerbottom3x3.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_top_leftstripblur_image = Image(this, "gui/frames/innerhlineblur30x3.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_top_rightstripblur_image = Image(this, "gui/frames/innerhlineblur30x3.png", true, NULL, 1, 1, 1, alpha,		1, 0, 0, 1);
	inner_top_leftvblur_image = Image(this, "gui/frames/innervblur12x81.png", true, NULL, 1, 1, 1, alpha,		0, 1, 1, 0);
	inner_top_rightvblur_image = Image(this, "gui/frames/innervblur12x81.png", true, NULL, 1, 1, 1, alpha,		1, 1, 0, 0);

	inner_bottom_mid_image = Image(this, "gui/frames/innerbottom3x3.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_bottom_leftdiag_image = Image(this, "gui/frames/innerdiagblur32x24.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_bottom_rightdiag_image = Image(this, "gui/frames/innerdiagblur32x24.png", true, NULL, 1, 1, 1, alpha,		1, 0, 0, 1);
	inner_bottom_leftstrip_image = Image(this, "gui/frames/innerbottom3x3.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_bottom_rightstrip_image = Image(this, "gui/frames/innerbottom3x3.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_bottom_leftstripblur_image = Image(this, "gui/frames/innerhlineblur30x3.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_bottom_rightstripblur_image = Image(this, "gui/frames/innerhlineblur30x3.png", true, NULL, 1, 1, 1, alpha,		1, 0, 0, 1);
	inner_bottom_leftvblur_image = Image(this, "gui/frames/innervblur12x81.png", true, NULL, 1, 1, 1, alpha,		0, 0, 1, 1);
	inner_bottom_rightvblur_image = Image(this, "gui/frames/innervblur12x81.png", true, NULL, 1, 1, 1, alpha,		1, 0, 0, 1);

	vscroll = VScroll(this, "vscroll");

	//m_minsz[0] = 32*2 + 12*2;
	m_minsz[0] = 12*2 + 10;
	m_minsz[1] = 10;

	// only call this when window is created
	if(reframefunc)
		reframefunc(this);
	else //default size and position
	{
		// to do
	}

	reframe();
}

void WindowW::reframe()
{
	//if(reframefunc)
	//	reframefunc(this);

	bg_image.m_pos[0] = m_pos[0];
	bg_image.m_pos[1] = m_pos[1];
	bg_image.m_pos[2] = m_pos[2];
	bg_image.m_pos[3] = m_pos[3];

	float innerleft = m_pos[0] - 45;
	float innertop = m_pos[1] - 50;
	float innerright = m_pos[2] + 45;
	float innerbottom = m_pos[3] + 50;

	float innerfree = innerright - innerleft - 32*2 - 12*2;

	vscroll.m_pos[0] = innerright - 3 - 10;
	vscroll.m_pos[1] = innertop + 27;
	vscroll.m_pos[2] = innerright - 3;
	vscroll.m_pos[3] = innerbottom - 27 - 10;
	vscroll.reframe();

	inner_top_mid_image.m_pos[0] = innerleft + 12 + innerfree/6.0f + 32;
	inner_top_mid_image.m_pos[1] = innertop;
	inner_top_mid_image.m_pos[2] = innerright - 12 - innerfree/6.0f - 32;
	inner_top_mid_image.m_pos[3] = innertop + 3;

	float minsz = fmin((innerright-innerleft),(innerbottom-innertop));

	bg_logo_image.m_pos[0] = (innerleft+innerright)/2.0f - minsz/2.0f;
	bg_logo_image.m_pos[1] = (innertop+innerbottom)/2.0f - minsz/2.0f;
	bg_logo_image.m_pos[2] = (innerleft+innerright)/2.0f + minsz/2.0f;
	bg_logo_image.m_pos[3] = (innertop+innerbottom)/2.0f + minsz/2.0f;

	inner_top_leftdiag_image.m_pos[0] = inner_top_mid_image.m_pos[0] - 32;
	inner_top_leftdiag_image.m_pos[1] = innertop;
	inner_top_leftdiag_image.m_pos[2] = inner_top_mid_image.m_pos[0];
	inner_top_leftdiag_image.m_pos[3] = innertop + 24;

	inner_top_rightdiag_image.m_pos[0] = inner_top_mid_image.m_pos[2];
	inner_top_rightdiag_image.m_pos[1] = innertop;
	inner_top_rightdiag_image.m_pos[2] = inner_top_mid_image.m_pos[2] + 32;
	inner_top_rightdiag_image.m_pos[3] = innertop + 24;

	inner_top_leftstrip_image.m_pos[0] = innerleft;
	inner_top_leftstrip_image.m_pos[1] = innertop + 24;
	inner_top_leftstrip_image.m_pos[2] = inner_top_leftdiag_image.m_pos[0];
	inner_top_leftstrip_image.m_pos[3] = innertop + 24 + 3;

	inner_top_rightstrip_image.m_pos[0] = inner_top_rightdiag_image.m_pos[2];
	inner_top_rightstrip_image.m_pos[1] = innertop + 24;
	inner_top_rightstrip_image.m_pos[2] = innerright;
	inner_top_rightstrip_image.m_pos[3] = innertop + 24 + 3;

	inner_top_leftstripblur_image.m_pos[0] = inner_top_leftdiag_image.m_pos[0];
	inner_top_leftstripblur_image.m_pos[1] = innertop + 24;
	inner_top_leftstripblur_image.m_pos[2] = inner_top_leftdiag_image.m_pos[2] + 12;
	inner_top_leftstripblur_image.m_pos[3] = innertop + 24 + 3;

	inner_top_rightstripblur_image.m_pos[0] = inner_top_rightdiag_image.m_pos[0] - 12;
	inner_top_rightstripblur_image.m_pos[1] = innertop + 24;
	inner_top_rightstripblur_image.m_pos[2] = inner_top_rightdiag_image.m_pos[2];
	inner_top_rightstripblur_image.m_pos[3] = innertop + 24 + 3;

	inner_top_leftvblur_image.m_pos[0] = innerleft;
	inner_top_leftvblur_image.m_pos[1] = innertop + 24;
	inner_top_leftvblur_image.m_pos[2] = innerleft + 12;
	inner_top_leftvblur_image.m_pos[3] = innerbottom/3.0f + innertop*2.0f/3.0f;

	inner_top_rightvblur_image.m_pos[0] = innerright - 12;
	inner_top_rightvblur_image.m_pos[1] = innertop + 24;
	inner_top_rightvblur_image.m_pos[2] = innerright;
	inner_top_rightvblur_image.m_pos[3] = innerbottom/3.0f + innertop*2.0f/3.0f;

	inner_bottom_mid_image.m_pos[0] = innerleft + 12 + innerfree/6.0f + 32;
	inner_bottom_mid_image.m_pos[1] = innerbottom - 3;
	inner_bottom_mid_image.m_pos[2] = innerright - 12 - innerfree/6.0f - 32;
	inner_bottom_mid_image.m_pos[3] = innerbottom;

	inner_bottom_leftdiag_image.m_pos[0] = inner_bottom_mid_image.m_pos[0] - 32;
	inner_bottom_leftdiag_image.m_pos[1] = innerbottom - 24;
	inner_bottom_leftdiag_image.m_pos[2] = inner_bottom_mid_image.m_pos[0];
	inner_bottom_leftdiag_image.m_pos[3] = innerbottom;

	inner_bottom_rightdiag_image.m_pos[0] = inner_bottom_mid_image.m_pos[2];
	inner_bottom_rightdiag_image.m_pos[1] = innerbottom - 24;
	inner_bottom_rightdiag_image.m_pos[2] = inner_bottom_mid_image.m_pos[2] + 32;
	inner_bottom_rightdiag_image.m_pos[3] = innerbottom;

	inner_bottom_leftstrip_image.m_pos[0] = innerleft;
	inner_bottom_leftstrip_image.m_pos[1] = innerbottom - 24 - 3;
	inner_bottom_leftstrip_image.m_pos[2] = inner_bottom_leftdiag_image.m_pos[0];
	inner_bottom_leftstrip_image.m_pos[3] = innerbottom - 24;

	inner_bottom_rightstrip_image.m_pos[0] = inner_bottom_rightdiag_image.m_pos[2];
	inner_bottom_rightstrip_image.m_pos[1] = innerbottom - 24 - 3;
	inner_bottom_rightstrip_image.m_pos[2] = innerright;
	inner_bottom_rightstrip_image.m_pos[3] = innerbottom - 24;

	inner_bottom_leftstripblur_image.m_pos[0] = inner_bottom_leftdiag_image.m_pos[0];
	inner_bottom_leftstripblur_image.m_pos[1] = innerbottom - 24 - 3;
	inner_bottom_leftstripblur_image.m_pos[2] = inner_bottom_leftdiag_image.m_pos[2] + 12;
	inner_bottom_leftstripblur_image.m_pos[3] = innerbottom - 24;

	inner_bottom_rightstripblur_image.m_pos[0] = inner_bottom_rightdiag_image.m_pos[0] - 12;
	inner_bottom_rightstripblur_image.m_pos[1] = innerbottom - 24 - 3;
	inner_bottom_rightstripblur_image.m_pos[2] = inner_bottom_rightdiag_image.m_pos[2];
	inner_bottom_rightstripblur_image.m_pos[3] = innerbottom - 24;

	inner_bottom_leftvblur_image.m_pos[0] = innerleft;
	inner_bottom_leftvblur_image.m_pos[1] = innerbottom*2.0f/3.0f + innertop/3.0f;
	inner_bottom_leftvblur_image.m_pos[2] = innerleft + 12;
	inner_bottom_leftvblur_image.m_pos[3] = innerbottom - 24;

	inner_bottom_rightvblur_image.m_pos[0] = innerright - 12;
	inner_bottom_rightvblur_image.m_pos[1] = innerbottom*2.0f/3.0f + innertop/3.0f;
	inner_bottom_rightvblur_image.m_pos[2] = innerright;
	inner_bottom_rightvblur_image.m_pos[3] = innerbottom - 24;

	left_image.m_pos[0] = m_pos[0] - 64;
	left_image.m_pos[1] = m_pos[1];
	left_image.m_pos[2] = m_pos[0];
	left_image.m_pos[3] = m_pos[3];
	left_image.m_texc[3] = (left_image.m_pos[3]-left_image.m_pos[1])/2.0f;

	right_image.m_pos[0] = m_pos[2];
	right_image.m_pos[1] = m_pos[1];
	right_image.m_pos[2] = m_pos[2] + 64;
	right_image.m_pos[3] = m_pos[3];
	right_image.m_texc[3] = (right_image.m_pos[3]-right_image.m_pos[1])/2.0f;

	bottomleft_image.m_pos[0] = m_pos[0] - 64;
	bottomleft_image.m_pos[1] = m_pos[3];
	bottomleft_image.m_pos[2] = m_pos[0];
	bottomleft_image.m_pos[3] = m_pos[3] + 64;

	bottomright_image.m_pos[0] = m_pos[2];
	bottomright_image.m_pos[1] = m_pos[3];
	bottomright_image.m_pos[2] = m_pos[2] + 64;
	bottomright_image.m_pos[3] = m_pos[3] + 64;

	topleft_image.m_pos[0] = m_pos[0] - 64;
	topleft_image.m_pos[1] = m_pos[1] - 64;
	topleft_image.m_pos[2] = m_pos[0];
	topleft_image.m_pos[3] = m_pos[1];

	topright_image.m_pos[0] = m_pos[2];
	topright_image.m_pos[1] = m_pos[1] - 64;
	topright_image.m_pos[2] = m_pos[2] + 64;
	topright_image.m_pos[3] = m_pos[1];

	top_image.m_pos[0] = m_pos[0];
	top_image.m_pos[1] = m_pos[1] - 64;
	top_image.m_pos[2] = m_pos[2];
	top_image.m_pos[3] = m_pos[1];

	bottom_image.m_pos[0] = m_pos[0];
	bottom_image.m_pos[1] = m_pos[3];
	bottom_image.m_pos[2] = m_pos[2];
	bottom_image.m_pos[3] = m_pos[3] + 64;

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->reframe();
}

void WindowW::draw()
{
	if(!m_opened)
		return;

	top_image.draw();
	topleft_image.draw();
	topright_image.draw();
	left_image.draw();
	right_image.draw();
	bottom_image.draw();
	bottomleft_image.draw();
	bottomright_image.draw();
	bg_image.draw();
	bg_logo_image.draw();

	inner_top_mid_image.draw();
	inner_top_leftdiag_image.draw();
	inner_top_rightdiag_image.draw();
	inner_top_leftstrip_image.draw();
	inner_top_rightstrip_image.draw();
	inner_top_leftstripblur_image.draw();
	inner_top_rightstripblur_image.draw();
	inner_top_leftvblur_image.draw();
	inner_top_rightvblur_image.draw();

	inner_bottom_mid_image.draw();
	inner_bottom_leftdiag_image.draw();
	inner_bottom_rightdiag_image.draw();
	inner_bottom_leftstrip_image.draw();
	inner_bottom_rightstrip_image.draw();
	inner_bottom_leftstripblur_image.draw();
	inner_bottom_rightstripblur_image.draw();
	inner_bottom_leftvblur_image.draw();
	inner_bottom_rightvblur_image.draw();

	vscroll.draw();

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->draw();
}

void WindowW::drawover()
{
	if(!m_opened)
		return;

	vscroll.drawover();

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->drawover();
}

void WindowW::inev(InEv* ev)
{
	if(!m_opened)
		return;

	for(auto w=m_subwidg.rbegin(); w!=m_subwidg.rend(); w++)
		(*w)->inev(ev);

	Player* py = &g_player[g_curP];

	vscroll.inev(ev);

	if(m_ldown)
	{
		if(ev->type == INEV_MOUSEMOVE ||
				( (ev->type == INEV_MOUSEDOWN || ev->type == INEV_MOUSEUP) && ev->key == MOUSE_LEFT) )
			ev->intercepted = true;

		if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_LEFT)
			m_ldown = false;

		if(ev->type == INEV_MOUSEMOVE)
		{
			int dx = py->mouse.x - m_mousedown[0];
			int dy = py->mouse.y - m_mousedown[1];
			m_mousedown[0] = py->mouse.x;
			m_mousedown[1] = py->mouse.y;

			if(py->curst == CU_MOVE)
			{
				m_pos[0] += dx;
				m_pos[1] += dy;
				m_pos[2] += dx;
				m_pos[3] += dy;

				if(m_pos[0]-64 < 0)
				{
					m_pos[2] -= m_pos[0] - 64;
					m_pos[0] = 64;
				}
				if(m_pos[2]+64 > py->width)
				{
					m_pos[0] -= m_pos[2] + 64 - py->width;
					m_pos[2] = py->width - 64;
				}
				if(m_pos[1]-64 < 0)
				{
					m_pos[3] -= m_pos[1] - 64;
					m_pos[1] = 64;
				}
				if(m_pos[3]+64 > py->height)
				{
					m_pos[1] -= m_pos[3] + 64 - py->height;
					m_pos[3] = py->height - 64;
				}

				reframe();
			}
			else if(py->curst == CU_RESZT)
			{
				int newh = m_pos[3]-m_pos[1]-dy;
				if(newh < m_minsz[1]) newh = m_minsz[1];
				m_pos[1] = m_pos[3] - newh;
				if(m_pos[1]-64 < 0) m_pos[1] = 64;
				if(m_pos[1]+64 > py->height) m_pos[1] = py->height-64;
				reframe();
			}
			else if(py->curst == CU_RESZB)
			{
				int newh = m_pos[3]-m_pos[1]+dy;
				if(newh < m_minsz[1]) newh = m_minsz[1];
				m_pos[3] = m_pos[1] + newh;
				if(m_pos[3]-64 < 0) m_pos[3] = 64;
				if(m_pos[3]+64 > py->height) m_pos[3] = py->height-64;
				reframe();
			}
			else if(py->curst == CU_RESZL)
			{
				int neww = m_pos[2]-m_pos[0]-dx;
				if(neww < m_minsz[0]) neww = m_minsz[0];
				m_pos[0] = m_pos[2] - neww;
				if(m_pos[0]-64 < 0) m_pos[0] = 64;
				if(m_pos[0]+64 > py->width) m_pos[0] = py->width-64;
				reframe();
			}
			else if(py->curst == CU_RESZR)
			{
				int neww = m_pos[2]-m_pos[0]+dx;
				if(neww < m_minsz[0]) neww = m_minsz[0];
				m_pos[2] = m_pos[0] + neww;
				if(m_pos[2]-64 < 0) m_pos[2] = 64;
				if(m_pos[2]+64 > py->width) m_pos[2] = py->width-64;
				reframe();
			}
			else if(py->curst == CU_RESZTL)
			{
				int newh = m_pos[3]-m_pos[1]-dy;
				if(newh < m_minsz[1]) newh = m_minsz[1];
				m_pos[1] = m_pos[3] - newh;
				if(m_pos[1]-64 < 0) m_pos[1] = 64;
				if(m_pos[1]+64 > py->height) m_pos[1] = py->height-64;

				int neww = m_pos[2]-m_pos[0]-dx;
				if(neww < m_minsz[0]) neww = m_minsz[0];
				m_pos[0] = m_pos[2] - neww;
				if(m_pos[0]-64 < 0) m_pos[0] = 64;
				if(m_pos[0]+64 > py->width) m_pos[0] = py->width-64;

				reframe();
			}
			else if(py->curst == CU_RESZTR)
			{
				int newh = m_pos[3]-m_pos[1]-dy;
				if(newh < m_minsz[1]) newh = m_minsz[1];
				m_pos[1] = m_pos[3] - newh;
				if(m_pos[1]-64 < 0) m_pos[1] = 64;
				if(m_pos[1]+64 > py->height) m_pos[1] = py->height-64;

				int neww = m_pos[2]-m_pos[0]+dx;
				if(neww < m_minsz[0]) neww = m_minsz[0];
				m_pos[2] = m_pos[0] + neww;
				if(m_pos[2]-64 < 0) m_pos[2] = 64;
				if(m_pos[2]+64 > py->width) m_pos[2] = py->width-64;

				reframe();
			}
			else if(py->curst == CU_RESZBL)
			{
				int newh = m_pos[3]-m_pos[1]+dy;
				if(newh < m_minsz[1]) newh = m_minsz[1];
				m_pos[3] = m_pos[1] + newh;
				if(m_pos[3]-64 < 0) m_pos[3] = 64;
				if(m_pos[3]+64 > py->height) m_pos[3] = py->height-64;

				int neww = m_pos[2]-m_pos[0]-dx;
				if(neww < m_minsz[0]) neww = m_minsz[0];
				m_pos[0] = m_pos[2] - neww;
				if(m_pos[0]-64 < 0) m_pos[0] = 64;
				if(m_pos[0]+64 > py->width) m_pos[0] = py->width-64;

				reframe();
			}
			else if(py->curst == CU_RESZBR)
			{
				int newh = m_pos[3]-m_pos[1]+dy;
				if(newh < m_minsz[1]) newh = m_minsz[1];
				m_pos[3] = m_pos[1] + newh;
				if(m_pos[3]-64 < 0) m_pos[3] = 64;
				if(m_pos[3]+64 > py->height) m_pos[3] = py->height-64;

				int neww = m_pos[2]-m_pos[0]+dx;
				if(neww < m_minsz[0]) neww = m_minsz[0];
				m_pos[2] = m_pos[0] + neww;
				if(m_pos[2]-64 < 0) m_pos[2] = 64;
				if(m_pos[2]+64 > py->width) m_pos[2] = py->width-64;

				reframe();
			}
		}
	}

	if(m_over && ev->type == INEV_MOUSEDOWN && !ev->intercepted)
	{
		if(ev->key == MOUSE_LEFT)
		{
			m_mousedown[0] = py->mouse.x;
			m_mousedown[1] = py->mouse.y;
			m_ldown = true;
			ev->intercepted = true;
		}
	}

	if(ev->type == INEV_MOUSEMOVE)
	{
		if(m_ldown)
		{
			ev->intercepted = true;
			return;
		}

		if(!ev->intercepted &&
				py->mouse.x >= m_pos[0]-64 &&
				py->mouse.y >= m_pos[1]-64 &&
				py->mouse.x <= m_pos[2]+64 &&
				py->mouse.y <= m_pos[3]+64)
		{
			m_over = true;

			if(py->mousekeys[MOUSE_MIDDLE])
				return;

			ev->intercepted = true;

			if(py->curst == CU_DRAG)
				return;

			if(py->mouse.x <= m_pos[0]-32)
			{
				if(py->mouse.y <= m_pos[1]-32)
					py->curst = CU_RESZTL;
				else if(py->mouse.y >= m_pos[3]+32)
					py->curst = CU_RESZBL;
				else
					py->curst = CU_RESZL;
			}
			else if(py->mouse.x >= m_pos[2]+32)
			{
				if(py->mouse.y <= m_pos[1]-32)
					py->curst = CU_RESZTR;
				else if(py->mouse.y >= m_pos[3]+32)
					py->curst = CU_RESZBR;
				else
					py->curst = CU_RESZR;
			}
			else if(py->mouse.x >= m_pos[0]-32 &&
					py->mouse.x <= m_pos[2]+32)
			{
				if(py->mouse.y <= m_pos[1]-32)
					py->curst = CU_RESZT;
				else if(py->mouse.y >= m_pos[3]+32)
					py->curst = CU_RESZB;
				else if(py->mouse.x <= m_pos[0] ||
						py->mouse.y <= m_pos[1] ||
						py->mouse.x >= m_pos[2] ||
						py->mouse.y >= m_pos[3])
					py->curst = CU_MOVE;
				else
					py->curst = CU_DEFAULT;
			}
		}
		else
		{
			if(!ev->intercepted)
			{
				if(m_over)
				{
					py->curst = CU_DEFAULT;
				}
			}
			else
			{
				// to do: this will be replaced by code in other
				//widgets that will set the cursor
				py->curst = CU_DEFAULT;
			}

			m_over = false;
		}
	}
}

void WindowW::subframe(float* fr)
{
	memcpy(fr, m_pos, sizeof(float)*4);
}
