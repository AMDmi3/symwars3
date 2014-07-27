#ifndef UNIT_H
#define UNIT_H

#include "../math/camera.h"
#include "../platform.h"
#include "../math/vec3i.h"
#include "../math/vec2i.h"
#include "resources.h"

#define BODY_LOWER	0
#define BODY_UPPER	1

class Demand;

class Unit
{
public:
	bool on;
	int type;
	int owner;

	/*
	The draw (floating-point) position vectory is used for drawing.
	*/
	Vec3f drawpos;

	/*
	The real position is stored in integers.
	*/
	Vec2i cmpos;
	Vec3f facing;
	Vec2f rotation;

	std::list<Vec2i> path;
	Vec2i goal;

	int target;
	int target2;
	bool targetu;
	bool underorder;
	int fuelstation;
	int belongings[RESOURCES];
	int hp;
	bool passive;
	Vec2i prevpos;
	int taskframe;
	bool pathblocked;
	int frameslookjobago;
	int supplier;
	int reqamt;
	int targtype;
	int home;
	int car;
	//std::vector<TransportJob> bids;

	float frame[2];

	Vec2i subgoal;

	unsigned char mode;
	int pathdelay;
	long long lastpath;

	bool threadwait;

	// used for debugging - don't save to file
	bool collided;

	unsigned char cdtype;	//conduit type for mode (going to construction)
	int driver;
	short framesleft;
	int transpwage;	//transport driver wage

	Unit();
	virtual ~Unit();
	void destroy();
	void fillcollider();
	void freecollider();
	void resetpath();
	bool hidden() const;
};

#define UNITS	(4096)

extern Unit g_unit[UNITS];

#define UMODE_NONE					0
#define UMODE_GOBLJOB				1
#define UMODE_BLJOB					2
#define UMODE_GOCSTJOB				3	//going to construction job
#define UMODE_CSTJOB				4
#define UMODE_GOCDJOB				5	//going to conduit (construction) job
#define UMODE_CDJOB					6	//conduit (construction) job
#define UMODE_GOSHOP				7
#define UMODE_SHOPPING				8
#define UMODE_GOREST				9
#define UMODE_RESTING				10
#define	UMODE_GOTRANSP				11	//going to transport job
#define UMODE_DRTRANSP				12	//driving transport
#define UMODE_GOSUP					13	//transporter going to supplier
#define UMODE_GODEMB				14	//transporter going to demander bl
#define UMODE_GOREFUEL				15
#define UMODE_REFUELING				16
#define UMODE_ATDEMB				17	//at demanber bl, unloading load
#define UMODE_ATSUP					18	//at supplier loading resources
#define UMODE_GODEMCD				19	//going to demander conduit
#define UMODE_ATDEMCD				20	//at demander conduit, unloading

#define TARG_NONE		0
#define TARG_BL			1	//building
#define TARG_U			2	//unit
#define TARG_CD			3	//conduit

void DrawUnits();
bool PlaceUnit(int type, Vec2i cmpos, int owner, int *reti);
void FreeUnits();
void UpdUnits();
void ResetPath(Unit* u);
void ResetGoal(Unit* u);
void ResetMode(Unit* u);
void ResetTarget(Unit* u);
void StartBel(Unit* u);

#endif
