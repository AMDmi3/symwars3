#include "../platform.h"
#include "../render/shader.h"
#include "draw2d.h"

void DrawImage(unsigned int tex, float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(g_shader[SHADER_ORTHO].m_slot[SSLOT_TEXTURE0], 0);

	float vertices[] =
	{
		//posx, posy    texx, texy
		left, top,          texleft, textop,
		right, top,         texright, textop,
		right, bottom,     texright, texbottom,

		right, bottom,      texright, texbottom,
		left, bottom,       texleft, texbottom,
		left, top,          texleft, textop
	};

    //glVertexPointer(2, GL_FLOAT, sizeof(float) * 4, &vertices[0]);
    //glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 4, &vertices[2]);
	glVertexAttribPointer(g_shader[g_curS].m_slot[SSLOT_POSITION], 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, &vertices[0]);
	glVertexAttribPointer(g_shader[g_curS].m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, &vertices[2]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawSquare(float r, float g, float b, float a, float left, float top, float right, float bottom)
{
	//return;
	/*
	glDisable(GL_TEXTURE_2D);
	glColor4f(r, g, b, a);

	glBegin(GL_QUADS);
	glVertex2f(left, top);
	glVertex2f(left, bottom);
	glVertex2f(right, bottom);
	glVertex2f(right, top);
	glEnd();

	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);*/

	glUniform4f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_COLOR], r, g, b, a);

	float vertices[] =
	{
		//posx, posy    texx, texy
		left, top,0,          0, 0,
		right, top,0,         1, 0,
		right, bottom,0,      1, 1,

		right, bottom,0,      1, 1,
		left, bottom,0,       0, 1,
		left, top,0,          0, 0
	};

	glVertexAttribPointer(g_shader[SHADER_COLOR2D].m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[0]);

	//glVertexPointer(3, GL_FLOAT, sizeof(float)*5, &vertices[0]);
	//glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, &vertices[3]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawLine(float r, float g, float b, float a, float x1, float y1, float x2, float y2)
{
	//return;
	/*
	glDisable(GL_TEXTURE_2D);
	glColor4f(r, g, b, a);

	glBegin(GL_QUADS);
	glVertex2f(left, top);
	glVertex2f(left, bottom);
	glVertex2f(right, bottom);
	glVertex2f(right, top);
	glEnd();

	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);*/

	glUniform4f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_COLOR], r, g, b, a);

	float vertices[] =
	{
		//posx, posy    texx, texy
		x1, y1,0,          0, 0,
		x2, y2,0,         1, 0,
	};

	glVertexAttribPointer(g_shader[SHADER_COLOR2D].m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[0]);

	//glVertexPointer(3, GL_FLOAT, sizeof(float)*5, &vertices[0]);
	//glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, &vertices[3]);

	glDrawArrays(GL_LINES, 0, 2);
}
