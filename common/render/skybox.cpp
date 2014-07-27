#include "../platform.h"
#include "skybox.h"
#include "../math/3dmath.h"
#include "shader.h"
#include "../texture.h"
#include "../math/matrix.h"
#include "shader.h"
#include "../math/vec3f.h"
#include "../utils.h"
#include "../debug.h"
#include "../path/pathdebug.h"
#include "shadow.h"

unsigned int g_front;
unsigned int g_left;
unsigned int g_right;
unsigned int g_bottom;
unsigned int g_top;
unsigned int g_back;

void LoadSkyBox(const char* basename)
{
	char front[MAX_PATH+1];
	char left[MAX_PATH+1];
	char right[MAX_PATH+1];
	char bottom[MAX_PATH+1];
	char top[MAX_PATH+1];
	char back[MAX_PATH+1];

	sprintf(front, "%sft^", basename);
	sprintf(left, "%slf^", basename);
	sprintf(right, "%srt^", basename);
	sprintf(bottom, "%sdn^", basename);
	sprintf(top, "%sup^", basename);
	sprintf(back, "%sbk^", basename);

	FindTextureExtension(front);
	FindTextureExtension(left);
	FindTextureExtension(right);
	FindTextureExtension(bottom);
	FindTextureExtension(top);
	FindTextureExtension(back);

	QueueTexture(&g_front, front, true, true);
	QueueTexture(&g_left, left, true, true);
	QueueTexture(&g_right, right, true, true);
	QueueTexture(&g_bottom, bottom, true, true);
	QueueTexture(&g_top, top, true, true);
	QueueTexture(&g_back, back, true, true);
}

void DrawSkyBox(Vec3f pos)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	Shader* s = &g_shader[g_curS];

	//g_log<<"mm"<<s->m_slot[SSLOT_MODELMAT]<<std::endl;
#if 1
	Matrix modelmat;
	modelmat.translation((const float*)&pos);
	glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, 0, modelmat.m_matrix);
#endif

	Matrix mvp;
#if 0
	mvp.set(modelview.m_matrix);
	mvp.postmult(g_camproj);
#elif 0
	mvp.set(g_camproj.m_matrix);
	mvp.postmult(modelview);
#else
	mvp.set(g_camproj.m_matrix);
	mvp.postmult(g_camview);
	mvp.postmult(modelmat);
#endif
	glUniformMatrix4fv(s->m_slot[SSLOT_MVP], 1, 0, mvp.m_matrix);

	CheckGLError(__FILE__, __LINE__);

	// right
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_right ].texname);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	CheckGLError(__FILE__, __LINE__);


	/*
	Note: I just realized these don't actually correspond to the sides
	they say they do. It was because I changed it in a previous project
	because somebody needed them to correspond to Quake's skybox texture mapping.
	*/

//#undef SKYBOX_SIZE
//#define SKYBOX_SIZE 2000.0f

	float vright[] =
	{
		SKY_TEX_1, SKY_TEX_0, SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_1, SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_1,-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_0, SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_1,-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_0, -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE
	};
//typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);

	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vright[2]);

	CheckGLError(__FILE__, __LINE__);

	glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vright[0]);

	//g_log<<"tci"<<s->m_slot[SSLOT_TEXCOORD0]<<std::endl;

	CheckGLError(__FILE__, __LINE__);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	CheckGLError(__FILE__, __LINE__);

#if 1
	// left
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_left ].texname);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	float vleft[] =
	{
		SKY_TEX_1, SKY_TEX_1, -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_1, SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_0, SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_1, -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_0, SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_0, -SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE
	};

	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vleft[2]);
	glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vleft[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	CheckGLError(__FILE__, __LINE__);


	// front
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_front ].texname);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	float vfront[] =
	{
		SKY_TEX_1, SKY_TEX_0, SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_1, SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_1, SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_0, SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_1, SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_0, SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE
	};

	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vfront[2]);
	glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vfront[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// back
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_back ].texname);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	float vback[] =
	{
		SKY_TEX_1, SKY_TEX_1, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_1, -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_0, -SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_1, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_0, -SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_0, -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE
	};

	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vback[2]);
	glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vback[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// bottom
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_bottom ].texname);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	float vbottom[] =
	{
		SKY_TEX_1, SKY_TEX_1, SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_1, -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_0, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_1, SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TEX_0, SKY_TEX_0, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TEX_1, SKY_TEX_0, SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE
	};

	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vbottom[2]);
	glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vbottom[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// top
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_top ].texname);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	float vtop[] =
	{
		SKY_TOP_0, SKY_TOP_1, -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TOP_0, SKY_TOP_0, -SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TOP_1, SKY_TOP_0, SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TOP_0, SKY_TOP_1, -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
		SKY_TOP_1, SKY_TOP_0, SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
		SKY_TOP_1, SKY_TOP_1, SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE
	};

	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vtop[2]);
	glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vtop[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//DrawGrid();
#endif

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

#if 0
void DrawSkyBox2(CVector3 pos)
{
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(pos.x, pos.y + SKYBOX_SIZE/2, pos.z);

	glBindTexture(GL_TEXTURE_2D, g_front);
	glBegin(GL_QUADS);
	SKY_TEX_0, SKY_TEX_0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TEX_0, SKY_TEX_1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TEX_1, SKY_TEX_1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TEX_1, SKY_TEX_0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_back);
	glBegin(GL_QUADS);
	SKY_TEX_1, SKY_TEX_0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TEX_0, SKY_TEX_0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TEX_0, SKY_TEX_1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TEX_1, SKY_TEX_1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_right);
	glBegin(GL_QUADS);
	SKY_TEX_0, SKY_TEX_0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TEX_0, SKY_TEX_1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TEX_1, SKY_TEX_1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TEX_1, SKY_TEX_0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_left);
	glBegin(GL_QUADS);
	SKY_TEX_1, SKY_TEX_0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TEX_0, SKY_TEX_0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TEX_0, SKY_TEX_1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TEX_1, SKY_TEX_1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_bottom);
	glBegin(GL_QUADS);
	SKY_TEX_1, SKY_TEX_0);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TEX_0, SKY_TEX_0);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TEX_0, SKY_TEX_1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TEX_1, SKY_TEX_1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_top);
	glBegin(GL_QUADS);
	SKY_TOP_1, SKY_TOP_1);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	SKY_TOP_1, SKY_TOP_0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TOP_0, SKY_TOP_0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	SKY_TOP_0, SKY_TOP_1);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	glEnd();

	/*
	glBindTexture(GL_TEXTURE_2D, g_front);
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_back);
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_right);
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_left);
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_bottom);
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);	glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);	glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, g_top);
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);	glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);	glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
	glEnd();
	*/

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}
#endif
