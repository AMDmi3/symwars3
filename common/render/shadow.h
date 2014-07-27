#ifndef SHADOW_H
#define SHADOW_H

#include "../math/vec3f.h"

#define DEPTH_SIZEX		1024	//2048	//512	//4096
#define DEPTH_SIZEY		1024	//2048	//512	//4096

extern unsigned int g_depth;
extern Vec3f g_lightpos;
extern Vec3f g_lighteye;
extern Vec3f g_lightup;
extern Vec3f g_lightoff;

class Matrix;

extern Matrix g_cammodelview;
extern Matrix g_camproj;
extern Matrix g_camview;
extern Matrix g_cammvp;

void InitShadows();
void RenderToShadowMap(Matrix projection, Matrix viewmat, Matrix modelmat, Vec3f focus, Vec3f lightpos, void (*drawscenedepthfunc)());
void RenderShadowedScene(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelview, void (*drawscenefunc)(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float lightpos[3], float lightdir[3]));
void UseShadow(int shader, Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float lightpos[3], float lightdir[3]);

void Transpose(Matrix mat, Matrix &transpMat);
bool Inverse2(Matrix mat, Matrix &invMat);

#if 0
extern void (*DrawSceneDepthFunc)();
extern void (*DrawSceneFunc)(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float lightpos[3], float lightdir[3]);
#endif

#endif
