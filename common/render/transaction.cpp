#include "../math/matrix.h"
#include "transaction.h"
#include "../gui/font.h"
#include "../math/vec4f.h"
#include "../window.h"
#include "../utils.h"
#include "../sim/player.h"

std::list<Transaction> g_transx;

void DrawTransactions(Matrix projmodlview)
{
	//return;

	Player* py = &g_player[g_curP];

	Vec3f* pos;
	Vec4f screenpos;
	int size = g_font[MAINFONT8].gheight;
	float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	auto triter = g_transx.begin();

	while(triter != g_transx.end())
	{
		pos = &triter->drawpos;

		screenpos.x = pos->x;
		screenpos.y = pos->y;
		screenpos.z = pos->z;
#if	1 //if ortho projection
		screenpos.w = 1;
#endif

		screenpos.transform(projmodlview);
		screenpos = screenpos / screenpos.w;
		screenpos.x = (screenpos.x * 0.5f + 0.5f) * py->width;
		screenpos.y = (-screenpos.y * 0.5f + 0.5f) * py->height;

		int x1 = screenpos.x - triter->halfwidth;
		int y1 = screenpos.y;
		color[3] = triter->life * 0.9f;

		DrawShadowedText(MAINFONT8, x1, y1, &triter->rtext, color);
		//DrawBoxShadText(MAINFONT8, x1, y1, py->width, py->height, &triter->rtext, color, 0, -1);
		//DrawCenterShadText(MAINFONT8, x1, y1, &triter->rtext, color, -1);

		triter->drawpos.y += TRANSACTION_RISE * g_drawfrinterval;
		triter->life -= TRANSACTION_DECAY * g_drawfrinterval;

		if(triter->life <= 0.0f)
		{
			triter = g_transx.erase( triter );
			continue;
		}

		triter ++;
	}
}

void NewTransx(Vec3f pos, const RichText* rtext)
{
	Transaction t;
	t.life = 1;
	t.drawpos = pos;
	t.rtext = *rtext;
	t.halfwidth = TextWidth(MAINFONT8, rtext) / 2.0f;
	g_transx.push_back(t);

#if 0
	auto titer = g_transx.rbegin();

	g_log<<"raw str = "<<titer->rtext.rawstr()<<std::endl;
	g_log.flush();
#endif
}
