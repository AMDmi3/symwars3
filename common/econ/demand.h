#ifndef DEMAND_H
#define DEMAND_H

#include "../math/vec2i.h"
#include "../platform.h"
#include "../sim/resources.h"
#include "../sim/unittype.h"
#include "../sim/buildingtype.h"
#include "../sim/build.h"
#include "../sim/player.h"

#define RATIO_DENOM		100000
#define AVG_DIST		(TILE_SIZE*6)
#define CYCLE_FRAMES	(SIM_FRAME_RATE*60)	

class CostCompo
{
public:
	int margcost;	//marginal cost
	int fixcost;	//fixed cost
	int transpcost;	//transportation cost
	int ramt;

	CostCompo()
	{
		margcost = 0;
		fixcost = 0;
		transpcost = 0;
		ramt = 0;
	}
};

class Bid
{
public:
	int minbid;
	int maxbid;
	Vec2i tpos;
	Vec2i cmpos;
	int maxdist;
	int minutil;
	int marginpr;
	std::list<CostCompo> costcompo;	//cost composition for different sources

	Bid()
	{
		minbid = 0;
		maxbid = 0;
		maxdist = -1;
		minutil = -1;
		marginpr = -1;
	}
};

#define DEM_NODE		0	//	unknown node
#define DEM_RNODE		1	//	resource demand
#define DEM_BNODE		2	//	demand at building, for building
#define DEM_UNODE		3	//	demand for unit: worker, transport, or military
#define DEM_ROADNODE	4	
#define DEM_POWLNODE	5
#define DEM_CRPIPENODE	6

class DemNode
{
public:
	int demtype;
	DemNode* parent;
	Bid bid;	//reflects the maximum possible gouging price
	int profit;

	DemNode();
};

class DemsAtB;
class DemsAtU;

class RDemNode : public DemNode
{
public:
	int rtype;
	int ramt;
	int btype;
	int bi;
	int utype;
	int ui;
	int demui;
	DemsAtB* supbp;
	DemsAtU* supup;
	DemsAtU* opup;

	RDemNode() : DemNode()
	{
		demtype = DEM_RNODE;
		rtype = -1;
		ramt = 0;
		btype = -1;
		bi = -1;
		utype = -1;
		ui = -1;
		supbp = NULL;
		supup = NULL;
		demui = -1;
	}
};

class RoadDem : public DemNode
{
public:
	Vec2i tpos;
	std::list<RDemNode> condems;

	RoadDem() : DemNode()
	{
		demtype = DEM_ROADNODE;
	}
};

class PowlDem : public DemNode
{
public:
	Vec2i tpos;
	std::list<RDemNode> condems;

	PowlDem() : DemNode()
	{
		demtype = DEM_POWLNODE;
	}
};

class CrPipeDem : public DemNode
{
public:
	Vec2i tpos;
	std::list<RDemNode> condems;

	CrPipeDem() : DemNode()
	{
		demtype = DEM_CRPIPENODE;
	}
};

class DemsAtB : public DemNode
{
public:
	int bi;
	int btype;
	std::list<DemNode*> condems;	//construction material
	std::list<DemNode*> proddems;	//production input raw materials
	std::list<DemNode*> manufdems;	//manufacturing input raw materials
	std::list<CrPipeDem*> crpipedems;
	std::list<PowlDem*> powldems;
	std::list<RoadDem*> roaddems;
	int prodratio;
	int condem[RESOURCES];
	int supplying[RESOURCES];

	DemsAtB() : DemNode()
	{
		demtype = DEM_BNODE;
		prodratio = 0;
		Zero(supplying);
		Zero(condem);
	}

	~DemsAtB()
	{
		auto riter = condems.begin();
		while(riter != condems.end())
		{
			delete *riter;
			riter = condems.erase(riter);
		}

		riter = proddems.begin();
		while(riter != proddems.end())
		{
			delete *riter;
			riter = proddems.erase(riter);
		}

		riter = manufdems.begin();
		while(riter != manufdems.end())
		{
			delete *riter;
			riter = manufdems.erase(riter);
		}
	}
};

class DemsAtU : public DemNode
{
public:
	int ui;
	int utype;
	std::list<DemNode*> manufdems;
	std::list<DemNode*> consumdems;
	DemsAtU* opup;	//operator/driver
	int prodratio;
	int timeused;
	int totaldem[RESOURCES];
	
	DemsAtU() : DemNode()
	{
		demtype = DEM_UNODE;
		prodratio = 0;
		Zero(totaldem);
		opup = NULL;
		timeused = 0;
	}

	~DemsAtU()
	{
		auto riter = manufdems.begin();
		while(riter != manufdems.end())
		{
			delete *riter;
			riter = manufdems.erase(riter);
		}
		
		riter = consumdems.begin();
		while(riter != consumdems.end())
		{
			delete *riter;
			riter = consumdems.erase(riter);
		}
	}
};

class DemTree
{
public:
	std::list<DemNode*> nodes;
	std::list<DemsAtB*> supbpcopy;	//master copy, this one will be freed
	std::list<DemsAtU*> supupcopy;	//master copy, this one will be freed
	std::list<DemNode*> codems[CONDUIT_TYPES];	//conduit placements
	int pyrsup[PLAYERS][RESOURCES];	//player global res supplying

	void free()
	{
		auto biter = supbpcopy.begin();
		while(biter != supbpcopy.end())
		{
			delete *biter;
			biter = supbpcopy.erase(biter);
		}
		
		auto uiter = supupcopy.begin();
		while(uiter != supupcopy.end())
		{
			delete *uiter;
			uiter = supupcopy.erase(uiter);
		}
		
		auto riter = nodes.begin();
		while(riter != nodes.end())
		{
			delete *riter;
			riter = nodes.erase(riter);
		}

		for(int i=0; i<CONDUIT_TYPES; i++)
		{
			auto coiter = codems[i].begin();
			while(coiter != codems[i].end())
			{
				delete *coiter;
				coiter = codems[i].erase(coiter);
			}
		}

		for(int i=0; i<PLAYERS; i++)
			Zero(pyrsup[i]);
	}

	DemTree()
	{
		free();
	}

	~DemTree()
	{
		free();
	}
};

extern DemTree g_demtree;
extern DemTree g_demtree2[PLAYERS];

void CalcDem1();
void CalcDem2(Player* p);

#endif
