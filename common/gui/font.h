#ifndef FONT_H
#define FONT_H

#include "../platform.h"

#define MAX_CHARS	256
#define CODE_POINTS	110000

class Glyph
{
public:
	int pixel[2];
	int texsize[2];
	int offset[2];
	int origsize[2];
	float texcoord[4];

	Glyph()
	{
		pixel[0] = 0;
		pixel[1] = 0;
		texsize[0] = 0;
		texsize[1] = 0;
		offset[0] = 0;
		offset[1] = 0;
		origsize[0] = 0;
		origsize[1] = 0;
		texcoord[0] = 0;
		texcoord[1] = 0;
		texcoord[2] = 0;
		texcoord[3] = 0;
	}
};

class Font
{
public:
	unsigned int texindex;
	float width;    //image width
	float height;   //image height
	float gheight;  //glyph height
	Glyph glyph[CODE_POINTS];

	Font()
	{
		width = 0;
		height = 0;
		gheight = 0;
	}
};

#define FONT_EUROSTILE32	0
#define FONT_MSUIGOTHIC16	1
#define FONT_SMALLFONTS10	2
#define FONT_GULIM32		3
#define FONT_EUROSTILE16	4
#define FONT_CALIBRILIGHT16	5
#define FONTS				6
extern Font g_font[FONTS];

//#define MAINFONT8 (FONT_SMALLFONTS10)
#define MAINFONT8 (FONT_CALIBRILIGHT16)
//#define MAINFONT16 (FONT_MSUIGOTHIC16)
//#define MAINFONT16 (FONT_EUROSTILE16)
#define MAINFONT16 (FONT_CALIBRILIGHT16)
//#define MAINFONT32 (FONT_EUROSTILE32)
#define MAINFONT32 (FONT_GULIM32)

class RichText;

void LoadFonts();
void UseFontTex();
void UseIconTex(int ico);
void DrawGlyph(float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom);
void DrawGlyphF(float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom);
void HighlGlyphF(float left, float top, float right, float bottom);
void DrawLine(int fnt, float startx, float starty, const RichText* text, const float* color=NULL, int caret=-1);
void DrawLineF(int fnt, float startx, float starty,  float framex1, float framey1, float framex2, float framey2, const RichText* text, const float* color=NULL, int caret=-1);
void DrawShadowedText(int fnt, float startx, float starty, const RichText* text, const float* color=NULL, int caret=-1);
void DrawShadowedTextF(int fnt, float startx, float starty, float framex1, float framey1, float framex2, float framey2, const RichText* text, const float* color=NULL, int caret=-1);
void DrawCenterShadText(int fnt, float startx, float starty, const RichText* text, const float* color=NULL, int caret=-1);
void DrawBoxShadText(int fnt, float startx, float starty, float width, float height, const RichText* text, const float* color, int ln, int caret);
void DrawBoxShadTextF(int fnt, float startx, float starty, float width, float height, const RichText* text, const float* color, int ln, int caret, float framex1, float framey1, float framex2, float framey2);
int CountLines(const RichText* text, int fnt, float startx, float starty, float width, float height);
int EndX(const RichText* text, int lastc, int fnt, float startx, float starty);
int MatchGlyphF(const RichText* text, int fnt, int matchx, float startx, float starty, float framex1, float framey1, float framex2, float framey2);
void HighlightF(int fnt, float startx, float starty, float framex1, float framey1, float framex2, float framey2, const RichText* text, int highlstarti, int highlendi);
void NextLineBreak();
void AdvanceGlyph();
int TextWidth(int fnt, const RichText* text);

#endif
