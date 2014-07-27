#include "../platform.h"
#include "../math/vec3f.h"

class OrderMarker
{
public:
	Vec3f pos;
	long tick;
	float radius;

	OrderMarker(Vec3f p, long t, float r)
	{
		pos = p;
		tick = t;
		radius = r;
	}
};

extern std::list<OrderMarker> g_order;

#define ORDER_EXPIRE		2000

void DrawOrders(Matrix* projection, Matrix* modelmat, Matrix* viewmat);
void Order(int mousex, int mousey, int viewwidth, int viewheight, Vec3f campos, Vec3f camfocus, Vec3f camviewdir, Vec3f camside, Vec3f camup2);
