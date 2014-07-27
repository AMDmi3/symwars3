#include "pathnode.h"
#include "collidertile.h"
#include "../math/vec2i.h"
#include "../math/3dmath.h"
#include "../sim/unit.h"
#include "../sim/unittype.h"
#include "../sim/building.h"
#include "../sim/buildingtype.h"
#include "../render/heightmap.h"
#include "../math/hmapmath.h"
#include "../phys/collision.h"
#include "../sim/road.h"
#include "../render/water.h"
#include "../utils.h"
#include "../render/shader.h"
#include "../sim/selection.h"
#include "../sim/sim.h"
#include "../phys/trace.h"
#include "binheap.h"
#include "jpspath.h"
#include "pathnode.h"
#include "../render/shader.h"
#include "../sim/player.h"

static std::vector<Vec3f> gridvecs;
Unit* g_pathunit = NULL;

void DrawSteps()
{
	Shader* s = &g_shader[g_curS];
	Player* py = &g_player[g_curP];

	if(py->sel.units.size() <= 0)
		return;

	int ui = *py->sel.units.begin();
	Unit* u = &g_unit[ui];

	std::vector<Vec3f> lines;

#if 1

	Vec2i npos = Vec2i( u->cmpos.x / PATHNODE_SIZE, u->cmpos.y / PATHNODE_SIZE );
	int nminx = imax(0, npos.x-50);
	int nminz = imax(0, npos.y-50);
	int nmaxx = imin(g_pathdim.x-1, npos.x+50);
	int nmaxz = imin(g_pathdim.y-1, npos.y+50);

	for(int x = nminx; x <= nmaxx; x ++)
		for(int z = nminz; z <= nmaxz; z++)
		{
			PathNode* n = PathNodeAt(x, z);

			if(!n->previous)
				continue;

			Vec2i nprevpos = PathNodePos(n->previous);

			Vec3f to;
			Vec3f from;

			to.x = x * PATHNODE_SIZE + PATHNODE_SIZE/2;
			to.z = z * PATHNODE_SIZE + PATHNODE_SIZE/2;
			to.y = g_hmap.accheight(to.x, to.z) + TILE_SIZE/20;

			from.x = nprevpos.x * PATHNODE_SIZE + PATHNODE_SIZE/2;
			from.z = nprevpos.y * PATHNODE_SIZE + PATHNODE_SIZE/2;
			from.y = g_hmap.accheight(from.x, from.z) + TILE_SIZE/20;

			lines.push_back(from);
			lines.push_back(to);
		}

#else
	int si = 0;
	auto siter = wt->openlist.begin();
	for(; siter != wt->openlist.end(); si++, siter++)
	{
		PathNode* tS = &*siter;
		PathNode* prevS = tS->previous;

		if(!prevS)
			continue;

		Vec3f fromvec;
		Vec3f tovec;

		fromvec.x = prevS->nx * PATHNODE_SIZE + PATHNODE_SIZE/2;
		fromvec.z = prevS->nz * PATHNODE_SIZE + PATHNODE_SIZE/2;
		fromvec.y = g_hmap.accheight(fromvec.x, fromvec.z) + TILE_SIZE/200;

		tovec.x = tS->nx * PATHNODE_SIZE + PATHNODE_SIZE/2;
		tovec.z = tS->nz * PATHNODE_SIZE + PATHNODE_SIZE/2;
		tovec.y = g_hmap.accheight(tovec.x, tovec.z) + TILE_SIZE/200;

		lines.push_back(fromvec);
		lines.push_back(tovec);
	}
#endif

	if(lines.size() <= 0)
		return;

	glUniform4f(s->m_slot[SSLOT_COLOR], 0.5f, 0.5f, 0, 1);
	//glBegin(GL_LINES);

	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &lines[0]);
	glDrawArrays(GL_LINES, 0, lines.size());
}

void DrawGrid()
{
	//return;
	Shader* s = &g_shader[g_curS];

#if 1
	if(gridvecs.size() > 0)
	{
		glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 1);
		//glBegin(GL_LINES);

		glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &gridvecs[0]);
		if(s->m_slot[SSLOT_TEXCOORD0] != -1)    glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, &gridvecs[0]);
		glDrawArrays(GL_LINES, 0, gridvecs.size());
	}
#endif

#if 1
	glUniform4f(s->m_slot[SSLOT_COLOR],  0.5f, 0, 0, 1);

	Player* py = &g_player[g_curP];

	if(py->sel.units.size() > 0)
	{
		int i = *py->sel.units.begin();
		Unit* u = &g_unit[i];

		bool roadVeh = false;
#if 1
		if(g_utype[u->type].roaded)
			roadVeh = true;
#endif

		UnitT* t = &g_utype[u->type];

		int ux = u->cmpos.x / PATHNODE_SIZE;
		int uz = u->cmpos.y / PATHNODE_SIZE;

		for(int x=imax(0, ux-50); x<imin(ux+50, g_pathdim.x); x++)
			for(int z=imax(0, uz-50); z<imin(uz+50, g_pathdim.y); z++)
			{
				ColliderTile* cell = ColliderTileAt(x, z);

				bool blocked = false;

				if(roadVeh && !(cell->flags & FLAG_HASROAD))
				{
					blocked = true;
				}

				bool foundother = false;

				for(short uiter = 0; uiter < 4; uiter++)
				{
					if(cell->units[uiter] < 0)
						continue;

					short uindex = cell->units[uiter];

					if(&g_unit[uindex] != u)
					{
						foundother = true;
						break;
					}
				}

				if(!foundother && !blocked)
					continue;

				std::vector<Vec3f> vecs;
				vecs.push_back(Vec3f(x*PATHNODE_SIZE, 1, z*PATHNODE_SIZE));
				vecs.push_back(Vec3f((x+1)*PATHNODE_SIZE, 1, (z+1)*PATHNODE_SIZE));
				vecs.push_back(Vec3f((x+1)*PATHNODE_SIZE, 1, z*PATHNODE_SIZE));
				vecs.push_back(Vec3f(x*PATHNODE_SIZE, 1, (z+1)*PATHNODE_SIZE));

				vecs[0].y = g_hmap.accheight(vecs[0].x, vecs[0].z) + TILE_SIZE/100;
				vecs[1].y = g_hmap.accheight(vecs[1].x, vecs[1].z) + TILE_SIZE/100;
				vecs[2].y = g_hmap.accheight(vecs[2].x, vecs[2].z) + TILE_SIZE/100;
				vecs[3].y = g_hmap.accheight(vecs[3].x, vecs[3].z) + TILE_SIZE/100;

				glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &vecs[0]);
				glDrawArrays(GL_LINES, 0, vecs.size());
			}
	}
#endif

	if(gridvecs.size() > 0)
		return;

#if 0
	for(int x=0; x<g_pathdim.x-1; x++)
	{
		for(int z=0; z<g_pathdim.y-1; z++)
		{
			int i = gridvecs.size();

			gridvecs.push_back(Vec3f(x*PATHNODE_SIZE, 0 + 1, z*PATHNODE_SIZE));
			gridvecs.push_back(Vec3f(x*PATHNODE_SIZE, 0 + 1, (z+1)*PATHNODE_SIZE));
			gridvecs[i+0].y = g_hmap.accheight(gridvecs[i+0].x, gridvecs[i+0].z) + TILE_SIZE/10;
			gridvecs[i+1].y = g_hmap.accheight(gridvecs[i+1].x, gridvecs[i+1].z) + TILE_SIZE/10;
			//glVertex3f(x*(MIN_RADIUS*2.0f), 0 + 1, 0);
			//glVertex3f(x*(MIN_RADIUS*2.0f), 0 + 1, g_map.m_widthZ*TILE_SIZE);
		}
	}

	for(int z=0; z<g_pathdim.y-1; z++)
	{
		for(int x=0; x<g_pathdim.x-1; x++)
		{
			int i = gridvecs.size();
			gridvecs.push_back(Vec3f(x*PATHNODE_SIZE, 0 + 1, z*PATHNODE_SIZE));
			gridvecs.push_back(Vec3f((x+1)*PATHNODE_SIZE, 0 + 1, z*PATHNODE_SIZE));
			gridvecs[i+0].y = g_hmap.accheight(gridvecs[i+0].x, gridvecs[i+0].z) + TILE_SIZE/10;
			gridvecs[i+1].y = g_hmap.accheight(gridvecs[i+1].x, gridvecs[i+1].z) + TILE_SIZE/10;
			//glVertex3f(0, 0 + 1, z*(MIN_RADIUS*2.0f));
			//glVertex3f(g_map.m_widthX*TILE_SIZE, 0 + 1, z*(MIN_RADIUS*2.0f));
		}
	}
#else
	for(int x=0; x<=g_hmap.m_widthx; x++)
	{
		for(int z=0; z<g_hmap.m_widthz; z++)
		{
			int i = gridvecs.size();
			int x1 = imin((x+0)*TILE_SIZE, g_hmap.m_widthx*TILE_SIZE-2);
			int x2 = imin((x+1)*TILE_SIZE, g_hmap.m_widthx*TILE_SIZE-2);
			int z1 = imin((z+0)*TILE_SIZE, g_hmap.m_widthz*TILE_SIZE-2);
			int z2 = imin((z+1)*TILE_SIZE, g_hmap.m_widthz*TILE_SIZE-2);
			gridvecs.push_back(Vec3f(x1, 0 + 1, z1));
			gridvecs.push_back(Vec3f(x1, 0 + 1, z2));
			gridvecs[i+0].y = g_hmap.accheight(gridvecs[i+0].x, gridvecs[i+0].z) + TILE_SIZE/10;
			gridvecs[i+1].y = g_hmap.accheight(gridvecs[i+1].x, gridvecs[i+1].z) + TILE_SIZE/10;
			//glVertex3f(x*(MIN_RADIUS*2.0f), 0 + 1, 0);
			//glVertex3f(x*(MIN_RADIUS*2.0f), 0 + 1, g_map.m_widthZ*TILE_SIZE);
		}
	}

	for(int z=0; z<=g_hmap.m_widthz; z++)
	{
		for(int x=0; x<g_hmap.m_widthx; x++)
		{
			int i = gridvecs.size();
			int x1 = imin((x+0)*TILE_SIZE, g_hmap.m_widthx*TILE_SIZE-2);
			int x2 = imin((x+1)*TILE_SIZE, g_hmap.m_widthx*TILE_SIZE-2);
			int z1 = imin((z+0)*TILE_SIZE, g_hmap.m_widthz*TILE_SIZE-2);
			int z2 = imin((z+1)*TILE_SIZE, g_hmap.m_widthz*TILE_SIZE-2);
			gridvecs.push_back(Vec3f(x1, 0 + 1, z1));
			gridvecs.push_back(Vec3f(x2, 0 + 1, z1));
			gridvecs[i+0].y = g_hmap.accheight(gridvecs[i+0].x, gridvecs[i+0].z) + TILE_SIZE/10;
			gridvecs[i+1].y = g_hmap.accheight(gridvecs[i+1].x, gridvecs[i+1].z) + TILE_SIZE/10;
			//glVertex3f(0, 0 + 1, z*(MIN_RADIUS*2.0f));
			//glVertex3f(g_map.m_widthX*TILE_SIZE, 0 + 1, z*(MIN_RADIUS*2.0f));
		}
	}
#endif

	//glEnd();
	//glColor4f(1, 1, 1, 1);
}

void DrawUnitSquares()
{
	Unit* u;
	UnitT* t;
	Vec3f p;
	Shader* s = &g_shader[g_curS];

	glUniform4f(s->m_slot[SSLOT_COLOR], 0.5f, 0, 0, 1);

	for(int i=0; i<UNITS; i++)
	{
		u = &g_unit[i];

		if(!u->on)
			continue;

		t = &g_utype[u->type];
		p = Vec3f(u->cmpos.x, u->drawpos.y + TILE_SIZE/100, u->cmpos.y);

#if 1
		if(u->collided)
			glUniform4f(s->m_slot[SSLOT_COLOR], 1.0f, 0, 0, 1);
		else
			glUniform4f(s->m_slot[SSLOT_COLOR], 0.2f, 0, 0, 1);
#endif

		/*
		 glVertex3f(p.x - r, 0 + 1, p.z - r);
		 glVertex3f(p.x - r, 0 + 1, p.z + r);
		 glVertex3f(p.x + r, 0 + 1, p.z + r);
		 glVertex3f(p.x + r, 0 + 1, p.z - r);
		 glVertex3f(p.x - r, 0 + 1, p.z - r);
		 */

		Vec3i vmin(u->cmpos.x - t->size.x/2, 0, u->cmpos.y - t->size.z/2);

		std::vector<Vec3f> vecs;
		vecs.push_back(Vec3f(vmin.x, 0 + 1, vmin.z));
		vecs.push_back(Vec3f(vmin.x, 0 + 1, vmin.z + t->size.z));
		vecs.push_back(Vec3f(vmin.x + t->size.x, 0 + 1, vmin.z + t->size.z));
		vecs.push_back(Vec3f(vmin.x + t->size.x, 0 + 1, vmin.z));
		vecs.push_back(Vec3f(vmin.x, 0 + 1, vmin.z));

		vecs[0].y = g_hmap.accheight(vecs[0].x, vecs[0].z) + TILE_SIZE/100;
		vecs[1].y = g_hmap.accheight(vecs[1].x, vecs[1].z) + TILE_SIZE/100;
		vecs[2].y = g_hmap.accheight(vecs[2].x, vecs[2].z) + TILE_SIZE/100;
		vecs[3].y = g_hmap.accheight(vecs[3].x, vecs[3].z) + TILE_SIZE/100;
		vecs[4].y = g_hmap.accheight(vecs[4].x, vecs[4].z) + TILE_SIZE/100;

		glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &vecs[0]);
		glDrawArrays(GL_LINE_STRIP, 0, vecs.size());
	}

}

void DrawPaths()
{
#if 1
	Player* py = &g_player[g_curP];
	Shader* s = &g_shader[g_curS];

	int i = 0;

	if(py->sel.units.size() <= 0)
		return;

	i = *py->sel.units.begin();
#else
	for(int i=0; i<UNITS; i++)
#endif
	{
		glUniform4f(s->m_slot[SSLOT_COLOR], 0, 1, 0, 1);

		Unit* u = &g_unit[i];

#if 0
		if(!u->on)
			continue;
#endif

		std::vector<Vec3f> vecs;

		Vec3f p;
		p.x = u->cmpos.x;
		p.z = u->cmpos.y;
		p.y = g_hmap.accheight(p.x, p.z) + TILE_SIZE/20;

		for(auto piter = u->path.begin(); piter != u->path.end(); piter++)
		{
			p.x = piter->x;
			p.z = piter->y;
			p.y = g_hmap.accheight(p.x, p.z) + TILE_SIZE/20;
			//glVertex3f(p->x, p->y + 5, p->z);
			vecs.push_back(p);
			//vecs.push_back(p+Vec3f(0,10,0));
		}

#if 0
		p.x = u->goal.x;
		p.z = u->goal.y;
		p.y = g_hmap.accheight(p.x, p.z) + TILE_SIZE/100;
#endif

		if(vecs.size() > 1)
		{
			glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &vecs[0]);
			glDrawArrays(GL_LINE_STRIP, 0, vecs.size());
		}
		else
		{
#if 0
			//vecs.push_back(u->camera.Position() + Vec3f(0,5,0));
			//vecs.push_back(u->goal + Vec3f(0,5,0));

			glUniform4f(s->m_slot[SSLOT_COLOR], 0.8f, 1, 0.8f, 1);

			glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &vecs[0]);
			glDrawArrays(GL_LINE_STRIP, 0, vecs.size());
#endif
		}
	}
}

#if 1
void DrawVelocities()
{
	Unit* u;
	Vec3f p;
	UnitT* t;
	Shader* s = &g_shader[g_curS];

	glUniform4f(s->m_slot[SSLOT_COLOR], 1, 0, 1, 1);

	for(int i=0; i<UNITS; i++)
	{
		u = &g_unit[i];

		if(!u->on)
			continue;

		t = &g_utype[u->type];

		std::vector<Vec3f> vecs;

		vecs.push_back(u->drawpos + Vec3f(0, TILE_SIZE/20, 0));
		Vec3f prevpos = Vec3f(u->prevpos.x, g_hmap.accheight(u->prevpos.x, u->prevpos.y), u->prevpos.y);
		vecs.push_back(u->drawpos + (u->drawpos - prevpos) * (10*t->cmspeed) + Vec3f(0, TILE_SIZE/20, 0));

		if(vecs.size() > 0)
		{
			glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &vecs[0]);
			glDrawArrays(GL_LINE_STRIP, 0, vecs.size());
		}
	}
}

#endif


void LogPathDebug()
{
	if(g_pathunit)
	{
		g_pathunit = NULL;
		return;
	}

	Player* py = &g_player[g_curP];

	if(py->sel.units.size() <= 0)
		return;

	int i = *py->sel.units.begin();

	g_pathunit = &g_unit[i];
}
