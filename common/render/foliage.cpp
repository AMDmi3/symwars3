#include "../texture.h"
#include "foliage.h"
#include "water.h"
#include "shader.h"
#include "../math/camera.h"
#include "model.h"
#include "../math/frustum.h"
#include "vertexarray.h"
#include "heightmap.h"
#include "../utils.h"
#include "../window.h"
#include "../sim/player.h"
#include "../render/shadow.h"

FoliageT g_foliageT[FOLIAGE_TYPES];
Foliage g_foliage[FOLIAGES];
Matrix g_folmodmat[FOLIAGES];
int g_folonsw[FOLIAGES];

Foliage::Foliage()
{
	on = false;
}

void Foliage::reinstance()
{
	int i = this - g_foliage;
	g_folonsw[i] = on ? 1 : 0;
	Matrix* m = &g_folmodmat[i];

	if(on)
	{
		float pitch = 0;
		m->reset();
		float radians[] = {(float)DEGTORAD(pitch), (float)DEGTORAD(yaw), 0};
		m->translation((const float*)&pos);
		Matrix rotation;
		rotation.rotrad(radians);
		m->postmult(rotation);
	}
	else
	{
		memset(m->m_matrix, 0, sizeof(float)*16);
	}
}

void DefF(int type, const char* modelrelative, Vec3f scale, Vec3f translate, Vec3i size)
{
	FoliageT* t = &g_foliageT[type];
	//QueueTexture(&t->texindex, texrelative, true);
	QueueModel(&t->model, modelrelative, scale, translate);
	t->size = size;
}

int NewFoliage()
{
	for(int i=0; i<FOLIAGES; i++)
	{
		if(!g_foliage[i].on)
			return i;
	}

	return -1;
}

#if 0
void PlaceFoliage()
{
	if(g_scT < 0)
		return;

	int i = NewFoliage();

	if(i < 0)
		return;

	Foliage* s = &g_foliage[i];
	s->on = true;
	s->pos = g_vMouse;
	s->type = g_scT;
	s->yaw = DEGTORAD((rand()%360));

	if(s->pos.y <= WATER_LEVEL)
		s->on = false;
}
#endif

void DrawFoliage(Vec3f zoompos, Vec3f vertical, Vec3f horizontal)
{
	//return;

	Vec3f a, b, c, d;
	Vec3f vert, horiz;

	Shader* s = &g_shader[g_curS];

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	//glUniform1f(s->m_slot[SSLOT_MIND], MIN_DISTANCE);
	//glUniform1f(s->m_slot[SSLOT_MAXD], MAX_DISTANCE / py->zoom);

	FoliageT* t = &g_foliageT[FOLIAGE_TREE1];
	Vec3i* size = &t->size;
	Model* m = &g_model[t->model];
	VertexArray* va = &m->m_va[0];
	m->usetex();
	Matrix im;

	Player* py = &g_player[g_curP];
	Camera* cam = &py->camera;

	Vec3f viewdir = Normalize(cam->m_view - zoompos);
	Vec3f horizontal2 = horizontal;

	for(int i=0; i<FOLIAGES; i++)
	{
		Foliage* f = &g_foliage[i];

		if(!f->on)
			continue;

		Vec3f vmin(f->pos.x - t->size.x/2, f->pos.y, f->pos.z - t->size.x/2);
		Vec3f vmax(f->pos.x + t->size.x/2, f->pos.y + t->size.y, f->pos.z + t->size.x/2);

		if(!g_frustum.boxin2(vmin.x, vmin.y, vmin.z, vmax.x, vmax.y, vmax.z))
			continue;

		glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, 0, g_folmodmat[i].m_matrix);

		Matrix modelview;
#ifdef SPECBUMPSHADOW
   		 modelview.set(g_camview.m_matrix);
#endif
    	modelview.postmult(g_folmodmat[i]);
		glUniformMatrix4fv(s->m_slot[SSLOT_MODELVIEW], 1, 0, modelview.m_matrix);

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
		mvp.postmult(g_folmodmat[i]);
#endif
		glUniformMatrix4fv(s->m_slot[SSLOT_MVP], 1, 0, mvp.m_matrix);

		Matrix modelviewinv;
		Transpose(modelview, modelview);
		Inverse2(modelview, modelviewinv);
		//Transpose(modelviewinv, modelviewinv);
		glUniformMatrix4fv(s->m_slot[SSLOT_NORMALMAT], 1, 0, modelviewinv.m_matrix);

		glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
		glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
		if(s->m_slot[SSLOT_NORMAL] != -1)
			glVertexAttribPointer(s->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);
		glDrawArrays(GL_TRIANGLES, 0, va->numverts);
	}
}

bool PlaceFoliage(int type, Vec3i ipos)
{
	int i = NewFoliage();

	if(i < 0)
		return false;

	Foliage* f = &g_foliage[i];
	f->on = true;
	f->type = type;
	f->pos = Vec3f(ipos.x, ipos.y, ipos.z);
	f->reinstance();

	return true;
}

void ClearFol(int minx, int minz, int maxx, int maxz)
{
	for(int i=0; i<FOLIAGES; i++)
	{
		Foliage* f = &g_foliage[i];

		if(!f->on)
			continue;

		if(f->pos.x >= minx && f->pos.x <= maxx && f->pos.z >= minz && f->pos.z <= maxz)
		{
			f->on = false;
			f->reinstance();
		}
	}
}

void FillForest()
{
#if 0
	for(int tx = 0; tx < g_hmap.m_widthx; tx++)
		for(int tz = 0; tz < g_hmap.m_widthz; tz++)
		{
			int x = tz*TILE_SIZE + TILE_SIZE/2;
			int z = tz*TILE_SIZE + TILE_SIZE/2;

			Vec3f norm = g_hmap.getnormal(tx, tz);

			float y = g_hmap.accheight2(x, z);

			if(y >= ELEV_SANDONLYMAXY && y <= ELEV_GRASSONLYMAXY && 1.0f - norm.y <= 0.3f)
			{
				Forest* f = ForestAt(tx, tz);
				f->on = true;
				f->remesh();
			}
		}
#endif
	//for(int condensation = 0; condensation < sqrt(g_hmap.m_widthx * g_hmap.m_widthz); condensation++)
	{

		int maxfoliage = FOLIAGES*g_hmap.m_widthx*g_hmap.m_widthz/MAX_MAP/MAX_MAP;
		maxfoliage = imin(FOLIAGES, maxfoliage);

		for(int i=0; i<maxfoliage; i++)
		{
			//break;
			for(int j=0; j<30; j++)
			{
				int x = (rand()%g_hmap.m_widthx)*TILE_SIZE + rand()%TILE_SIZE;
				int z = (rand()%g_hmap.m_widthz)*TILE_SIZE + rand()%TILE_SIZE;

				int tx = x / TILE_SIZE;
				int tz = z / TILE_SIZE;

				float y = g_hmap.accheight2(x, z);

				Vec3f norm = g_hmap.getnormal(x/TILE_SIZE, z/TILE_SIZE);

				float offequator = fabs( (float)g_hmap.m_widthz*TILE_SIZE/2.0f - z );

				if(y >= ELEV_SANDONLYMAXY && y <= ELEV_GRASSONLYMAXY && 1.0f - norm.y <= 0.3f)
				{
					//int type = rand()%10 == 1 ? UNIT_MECH : UNIT_LABOURER;
#if 0
					int type = UNIT_LABOURER;

					if(rand()%10 == 1)
						type = UNIT_MECH;
					if(rand()%10 == 1)
						type = UNIT_GEPARDAA;

					PlaceUnit(type, Vec3i(x, y, z), -1, -1);
#endif
#if 1

					int type = FOLIAGE_TREE1;

					PlaceFoliage(type, Vec3i(x, y, z));
#endif
					break;
				}
			}
		}

		//CondenseForest(0, 0, g_hmap.m_widthx-1, g_hmap.m_widthz-1);
	}
}

void FreeFoliage()
{
	for(int i=0; i<FOLIAGES; i++)
	{
		g_foliage[i].on = false;
		g_foliage[i].reinstance();
	}
}
