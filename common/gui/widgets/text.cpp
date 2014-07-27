#include "../widget.h"
#include "barbutton.h"
#include "button.h"
#include "checkbox.h"
#include "editbox.h"
#include "dropdowns.h"
#include "image.h"
#include "insdraw.h"
#include "link.h"
#include "listbox.h"
#include "text.h"
#include "textarea.h"
#include "textblock.h"
#include "touchlistener.h"

void Text::draw()
{
	//glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
	//glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.8f, 0.8f, 0.8f, 1.0f);
	//float color[] = {0.8f, 0.8f, 0.8f, 1.0f};
	//DrawShadowedText(font, pos[0], pos[1], text.c_str(), color);

	//g_log<<"draw text "<<m_text.rawstr().c_str()<<" (shadow: "<<m_shadow<<")"<<std::endl;
	//g_log.flush();

	if(m_shadow)
	{

#ifdef DEBUG
	g_log<<"text "<<__FILE__<<" "<<__LINE__<<std::endl;
    g_log.flush();
#endif
#if 1
		DrawShadowedTextF(m_font, m_pos[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3], &m_text, m_rgba);
#else
		DrawShadowedText(m_font, m_pos[0], m_pos[1], &m_text, m_rgba);
#endif
    }
	else
	{
#ifdef DEBUG
	g_log<<"text "<<__FILE__<<" "<<__LINE__<<std::endl;
    g_log.flush();
#endif

		DrawLineF(m_font, m_pos[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3], &m_text, m_rgba);
	}
	//glColor4f(1, 1, 1, 1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
}

