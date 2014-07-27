#include "order.h"
#include "../render/shader.h"
#include "selection.h"
#include "unit.h"
#include "unittype.h"
#include "../render/heightmap.h"
#include "sim.h"
#include "../window.h"
#include "../texture.h"
#include "../utils.h"
#include "../math/matrix.h"
#include "../math/hmapmath.h"
#include "../sound/sound.h"
#include "player.h"
#include "../render/shadow.h"

std::list<OrderMarker> g_order;

void DrawOrders(Matrix* projection, Matrix* modelmat, Matrix* viewmat)
{
	UseS(SHADER_BILLBOARD);
	//glBegin(GL_QUADS);

	OrderMarker* o;
	Vec3f p;
	float r;
	float a;

	Shader* s = &g_shader[g_curS];

	glUniformMatrix4fv(s->m_slot[SSLOT_PROJECTION], 1, 0, projection->m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, 0, modelmat->m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_VIEWMAT], 1, 0, viewmat->m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_MVP], 1, 0, g_cammvp.m_matrix);
	glUniform4f(s->m_slot[SSLOT_COLOR], 0, 1, 0, 1);
	//glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
	//glEnableVertexAttribArray(s->m_slot[SSLOT_TEXCOORD0]);
	//glEnableVertexAttribArray(s->m_slot[SSLOT_NORMAL]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_circle ].texname);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	auto oitr = g_order.begin();

	while(oitr != g_order.end())
	{
		o = &*oitr;
		p = o->pos;
		r = o->radius;
		a = 1.0f - (float)(GetTickCount() - o->tick)/(float)ORDER_EXPIRE;

		/*
		glColor4f(1, 1, 1, a);

		0, 0);		glVertex3f(p.x - r, p.y + 1, p.z - r);
		0, 1);		glVertex3f(p.x - r, p.y + 1, p.z + r);
		1, 1);		glVertex3f(p.x + r, p.y + 1, p.z + r);
		1, 0);		glVertex3f(p.x + r, p.y + 1, p.z - r);
		*/

		glUniform4f(s->m_slot[SSLOT_COLOR], 0, 1, 0, a);

#if 0
		float y1 = Bilerp(&g_hmap, p.x + r, p.z - r);
		float y2 = Bilerp(&g_hmap, p.x + r, p.z + r);
		float y3 = Bilerp(&g_hmap, p.x - r, p.z + r);
		float y4 = Bilerp(&g_hmap, p.x - r, p.z - r);
#elif 1
		float y1 = g_hmap.accheight(p.x + r, p.z - r);
		float y2 = g_hmap.accheight(p.x + r, p.z + r);
		float y3 = g_hmap.accheight(p.x - r, p.z + r);
		float y4 = g_hmap.accheight(p.x - r, p.z - r);
#else
		float y1 = p.y;
		float y2 = p.y;
		float y3 = p.y;
		float y4 = p.y;
#endif

		float vertices[] =
		{
			//posx, posy posz   texx, texy
			p.x + r, y1 + TILE_SIZE/20, p.z - r,          1, 0,
			p.x + r, y2	+ TILE_SIZE/20, p.z + r,          1, 1,
			p.x - r, y3 + TILE_SIZE/20, p.z + r,          0, 1,

			p.x - r, y3 + TILE_SIZE/20, p.z + r,          0, 1,
			p.x - r, y4 + TILE_SIZE/20, p.z - r,          0, 0,
			p.x + r, y1 + TILE_SIZE/20, p.z - r,          1, 0
		};

		//glVertexPointer(3, GL_FLOAT, sizeof(float)*5, &vertices[0]);
		//glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, &vertices[3]);

		glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[0]);
		glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[3]);
		//glVertexAttribPointer(s->m_slot[SLOT::NORMAL], 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, va->normals);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		if(GetTickCount() - o->tick > ORDER_EXPIRE)
		{
			oitr = g_order.erase(oitr);
			continue;
		}

		oitr++;
	}

	//glEnd();
	//glColor4f(1, 1, 1, 1);
	//glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	EndS();
}

void Order(int mousex, int mousey, int viewwidth, int viewheight, Vec3f campos, Vec3f camfocus, Vec3f camviewdir, Vec3f camside, Vec3f camup2)
{
#if 0
	InfoMessage("asd", "ord");
#endif

	Vec3f line[2];

	Vec3f ray = ScreenPerspRay(mousex, mousey, viewwidth, viewheight, campos, camside, camup2, camviewdir, FIELD_OF_VIEW);

	line[0] = campos;
	line[1] = campos + (ray * 100000.0f);

	Vec3f mapgoal;

#if 0
	if(!GetMapIntersection(&g_hmap, line, &mapgoal))
#else
	if(!FastMapIntersect(&g_hmap, line, &mapgoal))
#endif
		return;

#if 0
	InfoMessage("asd", "found surf");
#endif

	//std::vector<int> selection = py->sel;
	//int selecttype = g_selectType;

	//py->sel.clear();

	//SelectOne();

	//std::vector<int> targets = py->sel;
	//int targettype = g_selectType;

	//py->sel = selection;
	//g_selectType = selecttype;

	//char msg[128];
	//sprintf(msg, "s.size()=%d, stype=%d", (int)py->sel.size(), g_selectType);
	//Chat(msg);

	Vec3f vmin = Vec3f(0,0,0);
	Vec3f vmax = Vec3f(g_hmap.m_widthx*TILE_SIZE, 0, g_hmap.m_widthz*TILE_SIZE);
	Vec3f center(0,0,0);

#if 0
	char msg[256];
	sprintf(msg, "mapmaxmin:(%f,%f)->(%f,%f)", vmin.x, vmin.z, vmax.x, vmax.z);
	InfoMessage("asd", msg);
#endif

	Player* py = &g_player[g_curP];

	//if(targets.size() <= 0 || (targettype != SELECT_UNIT && targettype != SELECT_BUILDING))
	{
#if 0
		int i;
#endif
		Unit* u;
		UnitT* t;
		Vec3f order = mapgoal;

#if 0
		g_order.push_back(OrderMarker(order, GetTickCount(), 100));
#endif
		//order.x = Clip(order.x, 0, g_hmap.m_widthX*TILE_SIZE);
		//order.z = Clip(order.z, 0, g_hmap.m_widthZ*TILE_SIZE);
		Vec3f p;

		auto selitr = py->sel.units.begin();
		int counted = 0;

		while(selitr != py->sel.units.end())
		{
			u = &g_unit[*selitr];

#if 0
			if(e->hp <= 0.0f)
			{
				py->sel.erase( selitr );
				continue;
			}
#endif

			p = Vec3f(u->cmpos.x, 0, u->cmpos.y);
			//e->target = -1;
			//e->underOrder = true;

			if(p.x < vmin.x)
				vmin.x = p.x;
			if(p.z < vmin.z)
				vmin.z = p.z;
			if(p.x > vmax.x)
				vmax.x = p.x;
			if(p.z > vmax.z)
				vmax.z = p.z;

			//center = center + p;
			center = (center * counted + p) / (float)(counted+1);

			counted ++;
			selitr++;
		}

		//center = center / (float)py->sel.size();
		//Vec3f half = (std::min + std::max) / 2.0f;

#if 0
		char msg[256];
		sprintf(msg, "minmax:(%f,%f)->(%f,%f),order:(%f,%f)", vmin.x, vmin.z, vmax.x, vmax.z, order.x, order.z);
		InfoMessage("asd", msg);
#endif

		// All units to one goal
		//if(fabs(center.x - order.x) < half.x && fabs(center.z - order.z) < half.z)
		if(order.x <= vmax.x && order.x >= vmin.x && order.z <= vmax.z && order.z >= vmin.z)
		{
#if 0
			InfoMessage("asd", "typ 1");
#endif

			int radius = 0;
			selitr = py->sel.units.begin();

			// Get the biggest unit width/radius
			while(selitr != py->sel.units.end())
			{
				u = &g_unit[*selitr];
				u->resetpath();
				u->goal = Vec2i(order.x, order.z);
				u->underorder = true;
				u->target = -1;
				u->pathdelay = 0;
				u->lastpath = g_simframe;
				//e->underorder = true;
				t = &g_utype[u->type];
				if(t->size.x > radius)
					radius = t->size.x;
				//e->pathblocked = false;
				selitr++;
			}

			g_order.push_back(OrderMarker(order, GetTickCount(), radius*0.5f));

			Sound_Order();
		}
		// Formation goal
		else
		{
#if 0
			InfoMessage("asd", "typ 2");
#endif

			Vec3f offset;

			selitr = py->sel.units.begin();

			while(selitr != py->sel.units.end())
			{
				u = &g_unit[*selitr];
				p = Vec3f(u->cmpos.x, 0, u->cmpos.y);
				offset = p - center;
				Vec3f goal = order + offset;
				u->resetpath();
				u->goal = Vec2i(goal.x, goal.z);
				u->underorder = true;
				u->pathdelay = 0;
				u->lastpath = g_simframe;
				u->target = -1;
				t = &g_utype[u->type];
				int radius = t->size.x;
				//u->goal.x = Clip(u->goal.x, 0 + radius, g_hmap.m_widthX*TILE_SIZE - radius);
				//u->goal.z = Clip(u->goal.z, 0 + radius, g_hmap.m_widthZ*TILE_SIZE - radius);
				//u->pathblocked = false;
				g_order.push_back(OrderMarker(goal, GetTickCount(), radius*0.5f));
				selitr++;
			}

			Sound_Order();
		}

		//AckSnd();
	}
#if 0
	else if(targets.size() > 0 && targettype == SELECT_UNIT)
	{
		int targi = targets[0];
		CUnit* targu = &g_unit[targi];
		Vec3f p = targu->camera.Position();

		if(targu->owner == g_localP)
			return;

		MakeWar(g_localP, targu->owner);

		for(int j=0; j<py->sel.size(); j++)
		{
			int i = py->sel[j];
			CUnit* u = &g_unit[i];
			u->goal = p;
			u->underOrder = true;
			u->targetU = true;
			u->target = targi;
		}

		AckSnd();
	}
	else if(targets.size() > 0 && targettype == SELECT_BUILDING)
	{
		int targi = targets[0];
		CBuilding* targb = &g_building[targi];
		Vec3f p = targb->pos;

		if(targb->owner == g_localP)
			return;

		MakeWar(g_localP, targb->owner);

		for(int j=0; j<py->sel.size(); j++)
		{
			int i = py->sel[j];
			CUnit* u = &g_unit[i];
			u->goal = p;
			u->underOrder = true;
			u->targetU = false;
			u->target = targi;
		}

		AckSnd();
	}
#endif
}
