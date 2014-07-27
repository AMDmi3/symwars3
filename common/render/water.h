#ifndef WATER_H
#define WATER_H

#define WATER_LEVEL	0

#define WATER_TEX_GRADIENT		0
#define WATER_TEX_DETAIL		1
#define WATER_TEX_SPECULAR		2
#define WATER_TEX_NORMAL		3
#define WATER_TEXS				4

extern unsigned int g_watertex[WATER_TEXS];
extern unsigned int g_water;

void DrawWater();
void DrawWater2();
void DrawWater3();
void AllocWater(int wx, int wz);
void FreeWater();

#endif
