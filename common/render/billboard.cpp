#include "billboard.h"
#include "../platform.h"
#include "../math/3dmath.h"
#include "../texture.h"
#include "particle.h"
#include "shader.h"
#include "../utils.h"
#include "../math/vec3f.h"
#include "../math/camera.h"
#include "../sim/player.h"

BillboardT g_billbT[BILLBOARD_TYPES];
Billboard g_billb[BILLBOARDS];
unsigned int g_muzzle[4];

void Effects()
{
	QueueTexture(&g_muzzle[0], "effects/muzzle0.png", true, true);
	QueueTexture(&g_muzzle[1], "effects/muzzle1.png", true, true);
	QueueTexture(&g_muzzle[2], "effects/muzzle2.png", true, true);
	QueueTexture(&g_muzzle[3], "effects/muzzle3.png", true, true);
}

int NewBillbT()
{
	for(int i=0; i<BILLBOARD_TYPES; i++)
		if(!g_billbT[i].on)
			return i;

	return -1;
}

void DefBillb(int i, unsigned int glyph)
{
#if 0
	//BillboardT t;
	int i = NewBillbT();
	if(i < 0)
		return -1;
#endif

	BillboardT* t = &g_billbT[i];
	t->on = true;

#if 0
	char rawtex[64];
	StripPathExtension(tex, rawtex);
	strcpy(t->name, rawtex);
	char texpath[128];
	sprintf(texpath, "billboards/%s", rawtex);
	FindTextureExtension(texpath);
	//CreateTexture(t.tex, texpath);
	QueueTexture(&t->tex, texpath, true, true);
#elif 0
	QueueTexture(&t->tex, tex, true, true);
#else
	t->glyph = glyph;
#endif
	//g_billbT.push_back(t);
	//return g_billbT.size() - 1;
	//return i;
}

int NewBillboard()
{
	for(int i=0; i<BILLBOARDS; i++)
		if(!g_billb[i].on)
			return i;

	return -1;
}

void PlaceBillboard(int type, Vec3f pos, float size, int particle)
{
	int i = NewBillboard();
	if(i < 0)
		return;

	Billboard* b = &g_billb[i];
	b->on = true;
	b->type = type;
	b->pos = pos;
	b->size = size;
	b->particle = particle;
}

void SortBillboards()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	Vec3f pos = c->m_pos;
	Vec3f dir = Normalize( c->m_view - c->m_pos );

	for(int i=0; i<BILLBOARDS; i++)
	{
		if(!g_billb[i].on)
			continue;

		//g_billb[i].dist = Magnitude2(pos - g_billb[i].pos);
		g_billb[i].dist = Dot(dir, g_billb[i].pos);
	}

	Billboard temp;
	int leftoff = 0;
	bool backtracking = false;

	for(int i=1; i<BILLBOARDS; i++)
	{
		//if(!g_billb[i].on)
		//	continue;

		if(i > 0)
		{
			if(g_billb[i].dist > g_billb[i-1].dist)
			{
				if(!backtracking)
				{
					leftoff = i;
					backtracking = true;
				}
				temp = g_billb[i];
				g_billb[i] = g_billb[i-1];
				g_billb[i-1] = temp;
				i-=2;
			}
			else
			{
				if(backtracking)
				{
					backtracking = false;
					i = leftoff;
				}
			}
		}
		else
			backtracking = false;
	}
}

void DrawBillboards()
{
	Billboard* billb;
	BillboardT* t;
	float size;

	Player* py = &g_player[g_curP];
	Camera* cam = &py->camera;
	
	//Vec3f vertical = cam->up2();
	Vec3f vertical = cam->m_up;
	Vec3f horizontal = cam->m_strafe;
	Vec3f a, b, c, d;
	Vec3f vert, horiz;

	Particle* part;
	ParticleT* pT;

	Shader* s = &g_shader[SHADER_BILLBOARD];

	//glDisable(GL_CULL_FACE);

	for(int i=0; i<BILLBOARDS; i++)
	{
		billb = &g_billb[i];
		if(!billb->on)
			continue;

		t = &g_billbT[billb->type];
		Font* f = &g_font[MAINFONT16];
		Glyph* g = &f->glyph[t->glyph];

		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, g_texture[t->tex].texname);
		glBindTexture(GL_TEXTURE_2D, g_texture[f->texindex].texname);
		glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

		if(billb->particle >= 0)
		{
			part = &g_particle[billb->particle];
			pT = &g_particleT[part->type];
			size = pT->minsize + pT->sizevariation*(1.0f - part->life);
			glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, part->life);
		}
		else
		{
			size = billb->size;
			glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 1);
		}

		vert = vertical*size*g->texsize[1];
		horiz = horizontal*size*g->texsize[0];
		//Vec3f off = vertical*size*g->offset[1] + horizontal*size*g->offset[0];
		Vec3f off = vertical*size*g->offset[1];	// + horizontal*size*g->offset[0];

#if 0
	if(g_rtextp->m_type == RICHTEXT_ICON)
	{
		Icon* icon = &g_icon[g_rtextp->m_icon];
		float hscale = f->gheight / (float)icon->m_height;

		UseIconTex(g_rtextp->m_icon);
		glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

		int left = x;
		int right = left + (float)icon->m_width * hscale;
		int top = y;
		int bottom = top + f->gheight;
		DrawGlyph(left, top, right, bottom, 0, 0, 1, 1);

		UseFontTex();
		glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], currcolor[0], currcolor[1], currcolor[2], currcolor[3]);

		//g_log<<"color[3] = "<<currcolor[3]<<std::endl;
	}
	else if(g_rtextp->m_type == RICHTEXT_TEXT)
	{
		unsigned int k = g_rtextp->m_text.m_data[pi];
		Glyph* g = &f->glyph[k];

		int left = x + g->offset[0];
		int right = left + g->texsize[0];
		int top = y + g->offset[1];
		int bottom = top + g->texsize[1];
		DrawGlyph(left, top, right, bottom, g->texcoord[0], g->texcoord[1], g->texcoord[2], g->texcoord[3]);
	}
#endif

#if 0
		a = billb->pos - horiz + vert + off;
		b = billb->pos + horiz + vert + off;
		c = billb->pos + horiz - vert + off;
		d = billb->pos - horiz - vert + off;
#else
		a = billb->pos - horiz + vert*2 + off;
		b = billb->pos + horiz + vert*2 + off;
		c = billb->pos + horiz + off;
		d = billb->pos - horiz + off;
#endif

		const float vertices[] =
		{
			//posx, posy posz   texx, texy
			a.x, a.y, a.z,          g->texcoord[0], g->texcoord[1],
			b.x, b.y, b.z,          g->texcoord[2], g->texcoord[1],
			c.x, c.y, c.z,          g->texcoord[2], g->texcoord[3],

			c.x, c.y, c.z,          g->texcoord[2], g->texcoord[3],
			d.x, d.y, d.z,          g->texcoord[0], g->texcoord[3],
			a.x, a.y, a.z,          g->texcoord[0], g->texcoord[1]
		};

		//glVertexPointer(3, GL_FLOAT, sizeof(float)*5, &vertices[0]);
		//glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, &vertices[3]);

		glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[0]);
		glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[3]);
		//glVertexAttribPointer(s->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, va->normals);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		//g_log<<"draw"<<std::endl;
	}

	//glEnable(GL_CULL_FACE);

	/*
	CEntity* e;
	CPlayer* p;
	CHold* h;
	CItemType* iT;
	size = 8.0f;
	vert = vertical*size;
	horiz = horizontal*size;
	Vec3f muzz;
	Camera* cam;
	Vec3f offset;

	for(int i=0; i<ENTITIES; i++)
	{
	e = &g_entity[i];

	if(!e->on)
	continue;

	if(e->controller < 0)
	continue;

	p = &g_player[e->controller];

	if(p->equipped < 0)
	continue;

	h = &p->items[p->equipped];
	iT = &g_itemType[h->type];

	if((e->frame[BODY_UPPER] < ANIM_SHOTSHOULDER_S || e->frame[BODY_UPPER] > ANIM_SHOTSHOULDER_S+4))
	continue;

	glBindTexture(GL_TEXTURE_2D, g_muzzle[rand()%4]);

	cam = &e->camera;

	if(p == &g_player[g_localP] && g_viewmode == FIRSTPERSON)
	muzz = Rotate(iT->front, -cam->Pitch(), 1, 0, 0);
	else
	muzz = RotateAround(iT->front, Vec3f(0, MID_HEIGHT_OFFSET, 0), -cam->Pitch(), 1, 0, 0);

	muzz = cam->m_pos + Rotate(muzz, cam->Yaw(), 0, 1, 0);

	a = muzz - horiz + vert;
	b = muzz - horiz - vert;
	c = muzz + horiz - vert;
	d = muzz + horiz + vert;

	float vertices[] =
	{
	//posx, posy posz   texx, texy
	a.x, a.y, a.z,          1, 0,
	b.x, b.y, b.z,          1, 1,
	c.x, c.y, c.z,          0, 1,

	c.x, c.y, c.z,          0, 1,
	d.x, d.y, d.z,          0, 0,
	a.x, a.y, a.z,          1, 0
	};

	glVertexPointer(3, GL_FLOAT, sizeof(float)*5, &vertices[0]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, &vertices[3]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	}*/
}
