#include "deposit.h"
#include "resources.h"
#include "../math/3dmath.h"
#include "../render/shader.h"
#include "../render/heightmap.h"
#include "../math/vec3f.h"
#include "../gui/icon.h"
#include "../math/hmapmath.h"
#include "../gui/gui.h"
#include "../math/vec4f.h"
#include "player.h"

Deposit g_deposit[DEPOSITS];

Deposit::Deposit()
{
	on = false;
	occupied = false;
}

void FreeDeposits()
{
	for(int i=0; i<DEPOSITS; i++)
	{
		g_deposit[i].on = false;
		g_deposit[i].occupied = false;
	}
}

void DrawDeposits(const Matrix projection, const Matrix viewmat)
{
	if(g_hmap.m_widthx <= 0 || g_hmap.m_widthz <= 0)
		return;

	Shader* s = &g_shader[g_curS];

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, g_texture[ g_tiletexs[TILE_SAND] ].texname);
	//glUniform1i(s->m_slot[SSLOT_SANDTEX], 0);

	Matrix mvp;
	mvp.set(projection.m_matrix);
	mvp.postmult(viewmat);

	Player* py = &g_player[g_curP];

	for(int i=0; i<DEPOSITS; i++)
	{
		Deposit* d = &g_deposit[i];

		if(!d->on)
			continue;

		if(d->occupied)
			continue;

		Vec3f pos = d->drawpos;
#if 0
		pos.x = d->tilepos.x*TILE_SIZE + TILE_SIZE/2.0f;
		pos.z = d->tilepos.y*TILE_SIZE + TILE_SIZE/2.0f;
		pos.y = Bilerp(&g_hmap, pos.x, pos.z);
#endif
		//Vec4f ScreenPos(Matrix* mvp, Vec3f vec, float width, float height)
		Vec4f spos = ScreenPos(&mvp, pos, py->currw, py->currh, true);

		Resource* res = &g_resource[d->restype];
		Icon* ic = &g_icon[res->icon];

		DrawImage(g_texture[ ic->m_tex ].texname, spos.x - 25, spos.y - 25, spos.x + 25, spos.y + 25);
		//DrawShadowedText(MAINFONT8, spos.x, spos.y, &dep->m_label);
		//DrawCenterShadText(MAINFONT8, spos.x, spos.y, &dep->m_label);
	}
}
