#include "../render/model.h"
#include "buildingtype.h"

BuildingT g_bltype[BUILDING_TYPES];

BuildingT::BuildingT()
{
}

void DefB(int type, const char* name, Vec2i size, bool hugterr, const char* modelrelative, Vec3f scale, Vec3f translate, const char* cmodelrelative,  Vec3f cscale, Vec3f ctranslate, int foundation, int reqdeposit)
{
	BuildingT* t = &g_bltype[type];
	t->widthx = size.x;
	t->widthz = size.y;
	sprintf(t->name, name);
	QueueModel(&t->model, modelrelative, scale, translate);
	QueueModel(&t->cmodel, cmodelrelative, cscale, ctranslate);
	t->foundation = foundation;
	t->hugterr = hugterr;

	Zero(t->input);
	Zero(t->output);
	Zero(t->conmat);

	t->reqdeposit = reqdeposit;
}

void BConMat(int type, int res, int amt)
{
	BuildingT* t = &g_bltype[type];
	t->conmat[res] = amt;
}

void BInput(int type, int res, int amt)
{
	BuildingT* t = &g_bltype[type];
	t->input[res] = amt;
}

void BOutput(int type, int res, int amt)
{
	BuildingT* t = &g_bltype[type];
	t->output[res] = amt;
}

void BEmitter(int type, int emitterindex, int ptype, Vec3f offset)
{
	BuildingT* t = &g_bltype[type];
	EmitterPlace* e = &t->emitterpl[emitterindex];
	*e = EmitterPlace(ptype, offset);
}
