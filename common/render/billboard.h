#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "../platform.h"
#include "../math/3dmath.h"
#include "../math/vec3f.h"

class BillboardT
{
public:
	bool on;
	char name[32];
	unsigned int tex;
	unsigned int glyph;

	BillboardT()
	{
		on = false;
	}
};

#define BILLB_BL				0
#define BILLB_LAB				1
#define BILLB_MER				2
#define BILLBOARD_TYPES			3
extern BillboardT g_billbT[BILLBOARD_TYPES];

class Billboard
{
public:
	bool on;
	int type;
	float size;
	Vec3f pos;
	float dist;
	int particle;

	Billboard()
	{
		on = false;
		particle = -1;
	}
};

#define BILLBOARDS  256
extern Billboard g_billb[BILLBOARDS];

extern unsigned int g_muzzle[4];

void Effects();
int NewBillboard();
int NewBillboard(char* tex);
void DefBillb(int i, unsigned int glyph);
void SortBillboards();
void DrawBillboards();
void PlaceBillboard(int type, Vec3f pos, float size, int particle=-1);

#endif
