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
#include "../../sim/player.h"
#include "../gui.h"
#include "../../sys/unicode.h"
#include "../../debug.h"

EditBox::EditBox() : Widget()
{
	m_parent = NULL;
	m_type = WIDGET_EDITBOX;
	m_name = "";
	m_font = MAINFONT8;
	m_value = "";
	m_caret = m_value.texlen();
	m_opened = false;
	m_passw = false;
	m_maxlen = 0;
	reframefunc = NULL;
	submitfunc = NULL;
	changefunc3 = NULL;
	m_scroll[0] = 0;
	m_highl[0] = 0;
	m_highl[1] = 0;
	CreateTexture(m_frametex, "gui/frame.jpg", true, false);
	m_param = -1;
	changefunc2 = NULL;
	//reframe();
}

EditBox::EditBox(Widget* parent, const char* n, const RichText t, int f, void (*reframef)(Widget* thisw), bool pw, int maxl, void (*change3)(unsigned int key, unsigned int scancode, bool down), void (*submitf)(), int parm) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_EDITBOX;
	m_name = n;
	m_font = f;
	m_value = t;
	m_caret = m_value.texlen();
	m_opened = false;
	m_passw = pw;
	m_maxlen = maxl;
	reframefunc = reframef;
	submitfunc = submitf;
	changefunc3 = change3;
	m_scroll[0] = 0;
	m_highl[0] = 0;
	m_highl[1] = 0;
	CreateTexture(m_frametex, "gui/frame.jpg", true, false);
	m_param = parm;
	changefunc2 = NULL;
	reframe();
}

RichText EditBox::drawvalue()
{
	/*
	std::string val = m_value;

	if(m_passw)
	{
		val = "";
		for(int i=0; i<m_value.length(); i++)
			val.append("*");
	}

	return val;*/

	if(!m_passw)
		return m_value;

	return m_value.pwver();
}

void EditBox::draw()
{
	//glColor4f(1, 1, 1, 1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	DrawImage(g_texture[m_frametex].texname, m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

	if(m_over)
		//glColor4f(1, 1, 1, 1);
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	else
		//glColor4f(0.8f, 0.8f, 0.8f, 1);
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.8f, 0.8f, 0.8f, 1);

	RichText val = drawvalue();

	//if(m_opened)
	//	g_log<<"op m_caret="<<m_caret<<std::endl;

	DrawShadowedTextF(m_font, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3], &val, NULL, m_opened ? m_caret : -1);

	//glColor4f(1, 1, 1, 1);
	//glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	HighlightF(m_font, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3], &val, m_highl[0], m_highl[1]);
}

//#define MOUSESC_DEBUG

void EditBox::frameupd()
{
#ifdef MOUSESC_DEBUG
	g_log<<"editbox frameup"<<std::endl;
	g_log.flush();
#endif

	Player* py = &g_player[g_curP];

	if(m_ldown)
	{
		bool movedcar = false;

#ifdef MOUSESC_DEBUG
		g_log<<"ldown frameup"<<std::endl;
		g_log.flush();
#endif

		if(py->mouse.x >= m_pos[2]-5)
		{
			m_scroll[0] -= fmax(1, g_font[m_font].gheight/4.0f);

			RichText val = drawvalue();
			int vallen = val.texlen();

			int endx = EndX(&val, vallen, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

			if(endx < m_pos[2])
				m_scroll[0] += m_pos[2] - endx;

			if(m_scroll[0] > 0.0f)
				m_scroll[0] = 0.0f;

			movedcar = true;
		}
		else if(py->mouse.x <= m_pos[0]+5)
		{
			m_scroll[0] += fmax(1, g_font[m_font].gheight/4.0f);

			if(m_scroll[0] > 0.0f)
				m_scroll[0] = 0.0f;

			movedcar = true;
		}

		if(movedcar)
		{
			RichText val = drawvalue();
			int newcaret = MatchGlyphF(&val, m_font, py->mouse.x, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

			if(newcaret > m_caret)
			{
				m_highl[0] = m_caret;
				m_highl[1] = newcaret;
			}
			else if(newcaret < m_caret)
			{
				m_highl[0] = newcaret;
				m_highl[1] = m_caret;
			}
		}
	}
}

void EditBox::inev(InEv* ev)
{
//#ifdef MOUSESC_DEBUG
	//g_log<<"editbox mousemove"<<std::endl;
	//g_log.flush();
//#endif

	Player* py = &g_player[g_curP];

	if(ev->type == INEV_MOUSEMOVE)
	{
		if(!ev->intercepted)
		{
			if(m_ldown)
			{
				RichText val = drawvalue();
				int newcaret = MatchGlyphF(&val, m_font, py->mouse.x, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

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

				py->mouseoveraction = true;

				ev->intercepted = true;
				return;
			}
			else
			{
				m_over = false;

				return;
			}
		}
	}
	else if(ev->type == INEV_MOUSEDOWN && ev->key == MOUSE_LEFT)
	{
		if(m_opened)
		{
			m_opened = false;
			m_highl[0] = m_highl[1] = 0;
		}

		if(!ev->intercepted)
		{
			if(m_over)
			{
				m_ldown = true;

				RichText val = drawvalue();

				//m_highl[1] = MatchGlyphF(m_value.c_str(), m_font, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3]);
				//m_highl[0] = m_highl[1];
				//m_caret = m_highl[1];
				m_caret = MatchGlyphF(&val, m_font, py->mouse.x, m_pos[0]+m_scroll[0], m_pos[1], m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

				m_highl[0] = 0;
				m_highl[1] = 0;

				ev->intercepted = true;
				return;
			}
		}
	}
	else if(ev->type == INEV_MOUSEUP && ev->key == MOUSE_LEFT && !ev->intercepted)
	{
		//if(m_over && m_ldown)
		if(m_ldown)
		{
			m_ldown = false;

			if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
			{
				m_caret = -1;
			}

			ev->intercepted = true;
			gainfocus();

			return;
		}

		m_ldown = false;

		if(m_opened)
		{
			ev->intercepted = true;
			return;
		}
	}
	else if(ev->type == INEV_KEYDOWN && !ev->intercepted)
	{
		if(!m_opened)
			return;

		int len = m_value.texlen();

		if(m_caret > len)
			m_caret = len;

		if(ev->key == SDLK_F1)
			return;

		if(ev->key == SDLK_LEFT)
		{
			if(m_highl[0] > 0 && m_highl[0] != m_highl[1])
			{
				m_caret = m_highl[0];
				m_highl[0] = m_highl[1] = 0;
			}
			else if(m_caret <= 0)
			{
				ev->intercepted = true;
				return;
			}
			else
				m_caret --;

			RichText val = drawvalue();
			int endx = EndX(&val, m_caret, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

			//g_log<<"left endx = "<<endx<<"/"<<m_pos[0]<<std::endl;
			//g_log.flush();

			if(endx <= m_pos[0])
				m_scroll[0] += m_pos[0] - endx + 1;
		}
		else if(ev->key == SDLK_RIGHT)
		{
			int len = m_value.texlen();

			if(m_highl[0] > 0 && m_highl[0] != m_highl[1])
			{
				m_caret = m_highl[1];
				m_highl[0] = m_highl[1] = 0;
			}
			else if(m_caret >= len)
			{
				ev->intercepted = true;
				return;
			}
			else
				m_caret ++;

			RichText val = drawvalue();
			int endx = EndX(&val, m_caret, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

			if(endx >= m_pos[2])
				m_scroll[0] -= endx - m_pos[2] + 1;
		}
		else if(ev->key == SDLK_DELETE)
		{
			len = m_value.texlen();

			//g_log<<"vk del"<<std::endl;
			//g_log.flush();

			if((m_highl[1] <= 0 || m_highl[0] == m_highl[1]) && m_caret >= len || len <= 0)
			{
				ev->intercepted = true;
				return;
			}

			delnext();

			if(!m_passw)
				m_value = ParseTags(m_value, &m_caret);
		}
		if(ev->key == SDLK_BACKSPACE)
		{
			len = m_value.texlen();

			if((m_highl[1] <= 0 || m_highl[0] == m_highl[1]) && len <= 0)
			{
				ev->intercepted = true;
				return;
			}

			delprev();

			if(!m_passw)
				m_value = ParseTags(m_value, &m_caret);
		}/*
		 else if(ev->key == SDLK_DELETE)
		 {
		 len = m_value.texlen();

		 g_log<<"vk del"<<std::endl;
		 g_log.flush();

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
		else if(ev->key == SDLK_RETURN || ev->key == SDLK_RETURN2)
		{
			ev->intercepted = true;
			if(submitfunc)
				submitfunc();
			return;
		}
#if 0
		else if(ev->key == 190 && !py->keys[SDLK_SHIFT])
		{
			//placechar('.');
		}
#endif

		if(changefunc2 != NULL)
			changefunc2(m_param);

		if(changefunc3 != NULL)
			changefunc3(ev->key, ev->scancode, true);

		ev->intercepted = true;
	}
	else if(ev->type == INEV_KEYUP && !ev->intercepted)
	{
		if(!m_opened)
			return;
		
		if(changefunc3 != NULL)
			changefunc3(ev->key, ev->scancode, false);

		ev->intercepted = true;
	}
	else if(ev->type == INEV_TEXTIN && !ev->intercepted)
	{
		if(!m_opened)
			return;

		ev->intercepted = true;

		int len = m_value.texlen();

		if(m_caret > len)
			m_caret = len;

		//g_log<<"vk "<<ev->key<<std::endl;
		//g_log.flush();


#if 0
		if(ev->key == SDLK_SPACE)
		{
			placechar(' ');
		}
		else
#endif

#ifdef PASTE_DEBUG
			g_log<<"charin "<<(char)ev->key<<" ("<<ev->key<<")"<<std::endl;
		g_log.flush();
#endif

#if 0
		//if(ev->key == 'C' && py->keys[SDLK_CONTROL])
		if(ev->key == 3)	//copy
		{
			copyval();
		}
		//else if(ev->key == 'V' && py->keys[SDLK_CONTROL])
		else if(ev->key == 22)	//paste
		{
			pasteval();
		}
		//else if(ev->key == 'A' && py->keys[SDLK_CONTROL])
		else if(ev->key == 1)	//select all
		{
			selectall();
		}
		else
#endif
			unsigned int* ustr = ToUTF32((const unsigned char*)ev->text.c_str(), ev->text.length());
		//RichText addstr(RichTextP(UString(ustr)));	//Why does MSVS2012 not accept this?
		RichText addstr = RichText(RichTextP(UString(ustr)));
		unsigned int first = ustr[0];
		delete [] ustr;

		placestr(&addstr);

		if(changefunc != NULL)
			changefunc();

		if(changefunc2 != NULL)
			changefunc2(m_param);
		
		if(changefunc3 != NULL)
			changefunc3(first, 0, true);

		ev->intercepted = true;
	}
}

void EditBox::placestr(const RichText* str)
{
	int len = m_value.texlen();

	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		len -= m_highl[1] - m_highl[0];
	}

	int addlen = str->texlen();
	if(addlen + len >= m_maxlen)
		addlen = m_maxlen - len;

	RichText addstr = str->substr(0, addlen);

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
			return;
		}

		RichText before = m_value.substr(0, m_caret);
		RichText after = m_value.substr(m_caret, m_value.texlen()-m_caret);
		m_value = before + addstr + after;
		m_caret += addlen;

		LogRich(&m_value);
	}

	RichText val = drawvalue();
	int endx = EndX(&val, m_caret, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

	if(endx >= m_pos[2])
		m_scroll[0] -= endx - m_pos[2] + 1;
}

void EditBox::changevalue(const char* str)
{
	int len = m_value.texlen();

	if(len >= m_maxlen)
		return;

	int setlen = strlen(str);
	if(setlen >= m_maxlen)
		setlen = m_maxlen;

	char* setstr = new char[setlen+1];

	if(!setstr)
		OutOfMem(__FILE__, __LINE__);

	for(int i=0; i<setlen; i++)
		setstr[i] = str[i];
	setstr[setlen] = '\0';

	m_value = setstr;
	m_highl[0] = m_highl[1] = 0;
	m_caret = 0;

	delete [] setstr;
}

bool EditBox::delnext()
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

	RichText val = drawvalue();
	int endx = EndX(&val, m_caret, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

	if(endx <= m_pos[0])
		m_scroll[0] += m_pos[0] - endx + 1;
	else if(endx >= m_pos[2])
		m_scroll[0] -= endx - m_pos[2] + 1;

	return true;
}

bool EditBox::delprev()
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

		//g_log<<"before newval="<<before.rawstr()<<" texlen="<<before.texlen()<<std::endl;
		//g_log<<"after="<<after.rawstr()<<" texlen="<<after.texlen()<<std::endl;
		//g_log<<"ba newval="<<m_value.rawstr()<<" texlen="<<(before + after).texlen()<<std::endl;
		//g_log<<"newval="<<m_value.rawstr()<<" texlen="<<m_value.texlen()<<std::endl;

		m_caret--;
	}

	RichText val = drawvalue();
	int endx = EndX(&val, m_caret, m_font, m_pos[0]+m_scroll[0], m_pos[1]);

	if(endx <= m_pos[0])
		m_scroll[0] += m_pos[0] - endx + 1;
	else if(endx >= m_pos[2])
		m_scroll[0] -= endx - m_pos[2] + 1;

	return true;
}

//#define PASTE_DEBUG

void EditBox::copyval()
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
#endif // PLATFORM_WIN

	//return true;
}

void EditBox::pasteval()
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

	//placestr(str);

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
#endif // PLATFORM_WIN
}

void EditBox::selectall()
{
	m_highl[0] = 0;
	m_highl[1] = m_value.texlen()+1;
	m_caret = -1;

	RichText val = drawvalue();
	int endx = EndX(&val, m_value.texlen(), m_font, m_pos[0]+m_scroll[0], m_pos[1]);

	if(endx <= m_pos[2])
		m_scroll[0] += m_pos[2] - endx - 1;

	if(m_scroll[0] >= 0)
		m_scroll[0] = 0;

	//return true;
}

void EditBox::close()
{
	losefocus();

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->close();
}


void EditBox::gainfocus()
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

void EditBox::losefocus()
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
