#include "gui.h"
#include "../texture.h"
#include "../sim/player.h"

void ViewLayer::draw()
{
	if(!m_opened)
		return;

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->draw();
}

void ViewLayer::drawover()
{
	if(!m_opened)
		return;

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->drawover();
}

void ViewLayer::inev(InEv* ev)
{
	if(!m_opened)
		return;

	for(auto w=m_subwidg.rbegin(); w!=m_subwidg.rend(); w++)
		(*w)->inev(ev);
}

void ViewLayer::reframe()
{
	Player* py = &g_player[g_curP];

	m_pos[0] = 0;
	m_pos[1] = 0;
	m_pos[2] = py->width-1;
	m_pos[3] = py->height-1;

	if(reframefunc)
		reframefunc(this);

	for(auto w=m_subwidg.begin(); w!=m_subwidg.end(); w++)
		(*w)->reframe();
}
