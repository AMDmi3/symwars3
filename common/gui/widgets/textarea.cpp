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
#include "../icon.h"
#include "../../sim/player.h"

TextArea::TextArea(Widget* parent, const char* n, const RichText t, int f, void (*reframef)(Widget* thisw), float r, float g, float b, float a, void (*change)()) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_TEXTAREA;
	m_name = n;
	m_value = t;
	m_font = f;
	reframefunc = reframef;
	m_ldown = false;
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
	m_highl[0] = 0;
	m_highl[1] = 0;
	changefunc = change;
	m_scroll[1] = 0;
	m_highl[0] = 0;
	m_highl[1] = 0;
	m_caret = m_value.texlen();
	CreateTexture(m_frametex, "gui/frame.jpg", true, false);
	CreateTexture(m_filledtex, "gui/filled.jpg", true, false);
	CreateTexture(m_uptex, "gui/up.jpg", true, false);
	//CreateTexture(m_downtex, "gui/down.jpg", true, false);
	reframe();
	m_lines = CountLines(&m_value, f, m_pos[0], m_pos[1], m_pos[2]-m_pos[0]-square(), m_pos[3]-m_pos[1]);
}

void TextArea::draw()
{
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	DrawImage(m_frametex, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

	DrawImage(g_texture[m_frametex].texname, m_pos[2]-square(), m_pos[1], m_pos[2], m_pos[3]);
	DrawImage(g_texture[m_uptex].texname, m_pos[2]-square(), m_pos[1], m_pos[2], m_pos[1]+square());
	DrawImage(g_texture[m_downtex].texname, m_pos[2]-square(), m_pos[3]-square(), m_pos[2], m_pos[3]);
	DrawImage(g_texture[m_filledtex].texname, m_pos[2]-square(), m_pos[1]+square()+scrollspace()*topratio(), m_pos[2], m_pos[1]+square()+scrollspace()*bottomratio());

	float width = m_pos[2] - m_pos[0] - square();
	float height = m_pos[3] - m_pos[1];

	//DrawBoxShadText(m_font, m_pos[0], m_pos[1], width, height, m_value.c_str(), m_rgba, m_scroll[1], m_opened ? m_caret : -1);

	DrawShadowedTextF(m_font, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3], &m_value, NULL, m_opened ? m_caret : -1);

	HighlightF(m_font, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3], &m_value, m_highl[0], m_highl[1]);
}

int TextArea::rowsshown()
{
	int rows = (m_pos[3]-m_pos[1])/g_font[m_font].gheight;

	return rows;
}

int TextArea::square()
{
	return g_font[m_font].gheight;
}

float TextArea::scrollspace()
{
	return (m_pos[3]-m_pos[1]-square()*2);
}

void TextArea::inev(InEv* ev)
{
	Player* py = &g_player[g_curP];

	if(ev->type == INEV_MOUSEMOVE && !ev->intercepted)
	{
		if(m_ldown)
		{
			int newcaret = MatchGlyphF(&m_value, m_font, py->mouse.x, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

			if(newcaret > m_caret)
			{
				m_highl[0] = m_caret;
				m_highl[1] = newcaret;
				//g_log<<"hihgl "<<m_highl[0]<<"->"<<m_highl[1]<<std::endl;
				//g_log.flush();
			}
			else
			{
				m_highl[0] = newcaret;
				m_highl[1] = m_caret;
				//g_log<<"hihgl "<<m_highl[0]<<"->"<<m_highl[1]<<std::endl;
				//g_log.flush();
			}

			ev->intercepted = true;
			return;
		}

		if(py->mouse.x >= m_pos[0] && py->mouse.x <= m_pos[2] && py->mouse.y >= m_pos[1] && py->mouse.y <= m_pos[3])
		{
			m_over = true;

			ev->intercepted = true;
			return;
		}
		else
		{
			m_over = false;

			return;
		}
	}
	else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		if(m_over)
		{
			m_ldown = true;
			ev->intercepted = true;
			return;
		}
	}
	else if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		if(m_over && m_ldown)
		{
			m_ldown = false;
			ev->intercepted = true;
			gainfocus();
			return;
		}

		m_ldown = false;

		if(m_opened)
		{
			m_opened = false;
			return;
		}
	}
	else if(ev->type == INEV_KEYDOWN && !ev->intercepted)
	{
		if(!m_opened)
			return;

		//int len = m_value.length();
		int len = m_value.texlen();

		if(m_caret > len)
			m_caret = len;

		if(ev->key == SDLK_LEFT)
		{
			if(m_caret <= 0)
			{
				ev->intercepted = true;
				return;
			}

			m_caret --;
		}
		else if(ev->key == SDLK_RIGHT)
		{
			int len = m_value.texlen();

			if(m_caret >= len)
			{
				ev->intercepted = true;
				return;
			}

			m_caret ++;
		}
		else if(ev->key == SDLK_DELETE)
		{
			len = m_value.texlen();

			if((m_highl[1] <= 0 || m_highl[0] == m_highl[1]) && m_caret >= len || len <= 0)
			{
				ev->intercepted = true;
				return;
			}

			delnext();

			if(!m_passw)
				m_value = ParseTags(m_value, &m_caret);
		}
#if 0
		else if(ev->key == 190 && !py->keys[SDLK_SHIFT])
		{
			//placechar('.');
		}
#endif

		if(changefunc != NULL)
			changefunc();

		if(changefunc2 != NULL)
			changefunc2(m_param);

		ev->intercepted = true;
	}
	else if(ev->type == INEV_KEYUP && !ev->intercepted)
	{
		if(!m_opened)
			return;

		ev->intercepted = true;
	}
	else if(ev->type == INEV_CHARIN && !ev->intercepted)
	{
		if(!m_opened)
			return;

		int len = m_value.texlen();

		if(m_caret > len)
			m_caret = len;

		if(ev->key == SDLK_BACKSPACE)
		{
			len = m_value.texlen();

			if((m_highl[1] <= 0 || m_highl[0] == m_highl[1]) && m_caret >= len || len <= 0)
			{
				ev->intercepted = true;
				return;
			}

			delprev();

			if(!m_passw)
				m_value = ParseTags(m_value, &m_caret);
		}/*
		 else if(k == SDLK_DELETE)
		 {
		 len = m_value.texlen();

		 if((m_highl[1] <= 0 || m_highl[0] == m_highl[1]) && m_caret >= len || len <= 0)
		 return true;

		 delnext();

		 if(!m_passw)
		 m_value = ParseTags(m_value, &m_caret);
		 }*/
		else if(ev->key == SDLK_LSHIFT || ev->key == SDLK_RSHIFT)
		{
			ev->intercepted = true;
			return;
		}
		else if(ev->key == SDLK_CAPSLOCK)
		{
			ev->intercepted = true;
			return;
		}
		else if(ev->key == SDLK_SPACE)
		{
			placechar(' ');
		}
		else if(ev->key == SDLK_RETURN)
		{
			placechar('\n');
		}
		//else if(k >= 'A' && k <= 'Z' || k >= 'a' && k <= 'z')
		//else if(k == 190 || k == 188)
		//else if((k >= '!' && k <= '@') || (k >= '[' && k <= '`') || (k >= '{' && k <= '~') || (k >= '0' || k <= '9'))
		else
		{

#ifdef PASTE_DEBUG
			g_log<<"charin "<<(char)k<<" ("<<k<<")"<<std::endl;
			g_log.flush();
#endif

			//if(k == 'C' && py->keys[SDLK_CONTROL])
			if(ev->key == 3)	//copy
			{
				copyval();
			}
			//else if(k == 'V' && py->keys[SDLK_CONTROL])
			else if(ev->key == 22)	//paste
			{
				pasteval();
			}
			//else if(k == 'A' && py->keys[SDLK_CONTROL])
			else if(ev->key == 1)	//select all
			{
				selectall();
			}
			else
				placechar(ev->key);
		}

		if(changefunc != NULL)
			changefunc();

		if(changefunc2 != NULL)
			changefunc2(m_param);

		ev->intercepted = true;
	}
}

void TextArea::changevalue(const char* newv)
{
	m_value = newv;
	if(m_caret > strlen(newv))
		m_caret = strlen(newv);
	m_lines = CountLines(&m_value, MAINFONT8, m_pos[0], m_pos[1], m_pos[2]-m_pos[0]-square(), m_pos[3]-m_pos[1]);
}

void TextArea::placestr(const char* str)
{
	int len = m_value.texlen();

	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		len -= m_highl[1] - m_highl[0];
	}

	int addlen = strlen(str);
	if(addlen + len >= m_maxlen)
		addlen = m_maxlen - len;

	char* addstr = new char[addlen+1];

	if(!addstr)
		OutOfMem(__FILE__, __LINE__);

	if(addlen > 0)
	{
		for(int i=0; i<addlen; i++)
			addstr[i] = str[i];
		addstr[addlen] = '\0';
	}
	else
	{
		delete [] addstr;
		return;
	}

	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		RichText before = m_value.substr(0, m_highl[0]);
		RichText after = m_value.substr(m_highl[1]-1, m_value.texlen()-m_highl[1]);
		m_value = before + addstr + after;

		m_caret = m_highl[0] + addlen;
		m_highl[0] = m_highl[1] = 0;
	}
	else
	{
		if(len >= m_maxlen)
		{
			delete [] addstr;
			return;
		}

		RichText before = m_value.substr(0, m_caret);
		RichText after = m_value.substr(m_caret, m_value.texlen()-m_caret);
		m_value = before + addstr + after;
		m_caret += addlen;
	}

	//RichText val = drawvalue();
	int endx = EndX(&m_value, m_caret, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

	if(endx >= m_pos[2])
		m_scroll[0] -= endx - m_pos[2] + 1;

	delete [] addstr;
}

bool TextArea::delnext()
{
	int len = m_value.texlen();

	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		RichText before = m_value.substr(0, m_highl[0]);
		RichText after = m_value.substr(m_highl[1], len-m_highl[1]);
		m_value = before + after;

		m_caret = m_highl[0];
		m_highl[0] = m_highl[1] = 0;
	}
	else if(m_caret >= len || len <= 0)
		return true;
	else
	{
		RichText before = m_value.substr(0, m_caret);
		RichText after = m_value.substr(m_caret+1, len-m_caret);
		m_value = before + after;
	}

	//RichText val = drawvalue();
	int endx = EndX(&m_value, m_caret, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

	if(endx <= m_pos[0])
		m_scroll[0] += m_pos[0] - endx + 1;
	else if(endx >= m_pos[2])
		m_scroll[0] -= endx - m_pos[2] + 1;

	return true;
}

bool TextArea::delprev()
{
	int len = m_value.texlen();

	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		RichText before = m_value.substr(0, m_highl[0]);
		RichText after = m_value.substr(m_highl[1], len-m_highl[1]);
		m_value = before + after;

		m_caret = m_highl[0];
		m_highl[0] = m_highl[1] = 0;
	}
	else if(m_caret <= 0 || len <= 0)
		return true;
	else
	{
		RichText before = m_value.substr(0, m_caret-1);
		RichText after = m_value.substr(m_caret, len-m_caret);
		m_value = before + after;

		m_caret--;
	}

	//RichText val = drawvalue();
	int endx = EndX(&m_value, m_caret, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

	if(endx <= m_pos[0])
		m_scroll[0] += m_pos[0] - endx + 1;
	else if(endx >= m_pos[2])
		m_scroll[0] -= endx - m_pos[2] + 1;

	return true;
}

//#define PASTE_DEBUG

void TextArea::copyval()
{
#ifdef PASTE_DEBUG
	g_log<<"copy vkc"<<std::endl;
	g_log.flush();
#endif

#ifdef PLATFORM_WIN
	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		RichText highl = m_value.substr(m_highl[0], m_highl[1]-m_highl[0]);
		std::string rawhighl = highl.rawstr();
		const size_t len = strlen(rawhighl.c_str())+1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		memcpy(GlobalLock(hMem), rawhighl.c_str(), len);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}
	else
	{
		const char* output = "";
		const size_t len = strlen(output)+1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		memcpy(GlobalLock(hMem), output, len);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}

	//return true;
#endif //PLATFORM_WIN
}

void TextArea::pasteval()
{
#ifdef PLATFORM_WIN
#ifdef PASTE_DEBUG
	g_log<<"paste"<<std::endl;
#endif
	OpenClipboard(NULL);

#ifdef PASTE_DEBUG
	g_log<<"paste1"<<std::endl;
#endif
	HANDLE clip0 = GetClipboardData(CF_TEXT);

#ifdef PASTE_DEBUG
	g_log<<"paste2"<<std::endl;
#endif
	//HANDLE h = GlobalLock(clip0);
	//placestr((char*)clip0);
	char* str = (char*)GlobalLock(clip0);
#ifdef PASTE_DEBUG
	g_log<<"paste3"<<std::endl;
	g_log<<str<<std::endl;
#endif

	placestr(str);

#ifdef PASTE_DEBUG
	g_log<<"place str ";
	g_log<<str<<std::endl;
	g_log.flush();
	g_log.flush();
#endif

	if(!m_passw)
		m_value = ParseTags(m_value, &m_caret);

	GlobalUnlock(clip0);
	CloseClipboard();

	//return true;
#endif //PLATFORM_WIN
}

void TextArea::placechar(unsigned int k)
{
	//int len = m_value.length();

	//if(m_type == WIDGET_EDITBOX && len >= m_maxlen)
	//	return;

	//char addchar = k;

	//std::string before = m_value.substr(0, m_caret);
	//std::string after = m_value.substr(m_caret, len-m_caret);
	//m_value = before + addchar + after;

	RichText newval;

	int currplace = 0;
	bool changed = false;
	for(auto i=m_value.m_part.begin(); i!=m_value.m_part.end(); i++)
	{
		if(currplace + i->texlen() >= m_caret && !changed)
		{
			changed = true;

			if(i->m_type == RICHTEXT_TEXT)
			{
				if(i->m_text.m_length <= 1)
					continue;

				RichTextP chpart;

				chpart.m_type = RICHTEXT_TEXT;

				int subplace = m_caret - currplace;

				if(subplace > 0)
				{
					chpart.m_text = chpart.m_text + i->m_text.substr(0, subplace);
				}

				chpart.m_text = chpart.m_text + UString(k);

				if(i->m_text.m_length - subplace > 0)
				{
					chpart.m_text = chpart.m_text + i->m_text.substr(subplace, i->m_text.m_length-subplace);
				}

				chpart.m_text = i->m_text.substr(0, i->m_text.m_length-1);

				newval = newval + RichText(chpart);
			}
			else if(i->m_type == RICHTEXT_ICON)
			{
				Icon* icon = &g_icon[i->m_icon];

				int subplace = m_caret - currplace;

				if(subplace <= 0)
				{
					newval = newval + RichText(RichTextP(UString(k)));
					newval = newval + RichText(*i);
				}
				else
				{
					newval = newval + RichText(*i);
					newval = newval + RichText(RichTextP(UString(k)));
				}
			}

		}
		else
		{
			newval = newval + RichText(*i);
			currplace += i->texlen();
		}
	}

	m_value = newval;

	m_caret ++;
}

void TextArea::selectall()
{
	m_highl[0] = 0;
	m_highl[1] = m_value.texlen()+1;
	m_caret = -1;

	//RichText val = drawvalue();
	int endx = EndX(&m_value, m_value.texlen(), m_font, m_pos[0]+m_scroll[0], m_pos[1]);

	if(endx <= m_pos[2])
		m_scroll[0] += m_pos[2] - endx - 1;

	if(m_scroll[0] >= 0)
		m_scroll[0] = 0;

	//return true;
}

void TextArea::close()
{
	losefocus();

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->close();
}


void TextArea::gainfocus()
{
	if(!m_opened)
	{
		Player* py = &g_player[g_curP];

		if(py->kbfocus > 0)
		{
			SDL_StopTextInput();
			py->kbfocus--;
		}

		m_opened = true;
		SDL_StartTextInput();
		SDL_Rect r;
		r.x = m_pos[0];
		r.y = m_pos[3];
		r.w = py->width - m_pos[0];
		r.h = py->height - m_pos[3];
		SDL_SetTextInputRect(&r);
		py->kbfocus++;
	}
}

void TextArea::losefocus()
{
	if(m_opened)
	{
		Player* py = &g_player[g_curP];

		if(py->kbfocus > 0)
		{
			SDL_StopTextInput();
			py->kbfocus--;
		}

		m_opened = false;
	}
}
