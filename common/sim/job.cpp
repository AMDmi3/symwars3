

#include "job.h"
#include "building.h"
#include "infrastructure.h"

bool FindJob(Unit* u)
{
#if 0
	//char msg[128];
	//sprintf(msg, "u %d fj @ %d", UnitID(this), (int)GetTickCount());
	//Chat(msg);

	if(frameslookjobago < LOOKJOB_DELAY_MAX)
		return false;

	bool pathed = false;

	int bestJobType = NONE;
	int bestTarget = -1;
	int bestTargetX = -1;
	int bestTargetZ = -1;
	//float bestDistWage = -1;
	//float distWage;
	float bestTimeEarn = -1;
	float timeEarn;
	float dist;
	//bool fullquota;

	//Vec3f pos = camera.Position();
	CBuilding* b;
	CBuildingType* t;
	CPlayer* p;
	CRoad* r;
	CPowerline* pow;
	CPipeline* pipe;
	CUnit* u;
	//CResource* res;
	CUnitType* ut = &g_unitType[type];

	//LastNum("before truck job");

	//Truck jobs
	for(int i=0; i<UNITS; i++)
	{
		u = &g_unit[i];

		if(!u->on)
			continue;

		if(u->hp <= 0.0f)
			continue;

		if(u->type != TRUCK)
			continue;

		//Chat("tj0");

		//if(u->mode != AWAITINGDRIVER)
		//	continue;

		if(u->mode != GOINGTOSUPPLIER && u->mode != GOINGTODEMANDERB && u->mode != GOINGTODEMROAD && u->mode != GOINGTODEMPOWL && u->mode != GOINGTODEMPIPE && u->mode != GOINGTOREFUEL)
			continue;

		//Chat("tj1");

		if(u->driver >= 0 && &g_unit[u->driver] != this)
			continue;

		//Chat("tj2");

		p = &g_player[u->owner];


		//Chat("tj3");

		if(p->global[CURRENC] < p->truckwage)
		{
			Bankrupt(u->owner, "truck expenses");
			continue;
		}

		//Chat("tj4");

		mode = GOINGTOTRUCK;
		target = i;
		goal = u->camera.Position();

		pathed = true;

		if(!Pathfind())
		{
			//Chat("!tj5");
			continue;
		}

		dist = pathlength();
		//dist = Magnitude2(u->camera.Position() - camera.Position());
		//distWage = p->truckwage / dist;

		float seconds = dist / ut->speed / (float)FRAME_RATE;

		//float trucklen = u->pathlength();
		//CUnitType* truckt = &g_unitType[u->type];
		//float truckseconds = trucklen / truckt->speed / (float)FRAME_RATE;
		float reqlab = labour;	//min(labour, max(1, truckseconds / (float)DRIVE_WORK_DELAY * 1000.0f));
		seconds += reqlab * (float)DRIVE_WORK_DELAY / 1000.0f;
		float earn = reqlab * p->truckwage;
		timeEarn = earn / seconds;

		//if(distWage < bestDistWage)
		//	continue;
		if(timeEarn < bestTimeEarn)
			continue;

		//bestDistWage = distWage;
		bestTimeEarn = timeEarn;
		bestTarget = i;
		bestJobType = DRIVING;
	}

	//LastNum("after truck job");

	// Construction jobs
	for(int i=0; i<BUILDINGS; i++)
	{
		b = &g_building[i];

		if(!b->on)
			continue;

		if(b->finished)
			continue;

		t = &g_buildingType[b->type];

		if(b->conmat[LABOUR] >= t->cost[LABOUR])
		{
			continue;
		}

		p = &g_player[b->owner];

		if(p->global[CURRENC] < p->conwage)
		{
			char reason[32];
			sprintf(reason, "%s construction", g_buildingType[b->type].name);
			Bankrupt(b->owner, reason);
			continue;
		}

		mode = GOINGTOCONJOB;
		target = i;
		goal = g_building[i].pos;

		pathed = true;

		if(!Pathfind())
			continue;

		dist = pathlength();
		//dist = Magnitude2(pos - b->pos);
		//distWage = p->conwage / dist;

		float seconds = dist / ut->speed / (float)FRAME_RATE;
		float reqlab = min(labour, b->netreq(LABOUR));
		seconds += reqlab * (float)WORK_DELAY / 1000.0f;
		float earn = reqlab * p->conwage;
		timeEarn = earn / seconds;

		//if(distWage < bestDistWage)
		if(timeEarn < bestTimeEarn)
		{
			continue;
		}

		bestTarget = i;
		bestJobType = CONJOB;
		//bestDistWage = distWage;
		bestTimeEarn = timeEarn;
	}


	//LastNum("after truck job 1");

	// Normal/building jobs
	for(int i=0; i<BUILDINGS; i++)
	{
		b = &g_building[i];

		if(!b->on)
			continue;

		if(!b->finished)
			continue;

		t = &g_buildingType[b->type];

		if(b->occupier.size() > 0)
			continue;

		//if(b->stock[LABOUR] >= t->input[LABOUR])
		//	continue;

		p = &g_player[b->owner];

		if(b->excessoutput())
			continue;

		if(b->excessinput(LABOUR))
			continue;

		if(p->global[CURRENC] < p->wage[b->type])
		{
			char reason[32];
			sprintf(reason, "%s expenses", g_buildingType[b->type].name);
			Bankrupt(b->owner, reason);
			continue;
		}

		mode = GOINGTONORMJOB;
		target = i;
		goal = g_building[i].pos;

		pathed = true;

		if(!Pathfind())
			continue;

		dist = pathlength();
		//dist = Magnitude2(pos - b->pos);
		//distWage = p->wage[b->type] / dist;

		float seconds = dist / ut->speed / (float)FRAME_RATE;
		float reqlab = min(labour, b->netreq(LABOUR));
		seconds += reqlab * (float)WORK_DELAY / 1000.0f;
		float earn = reqlab * p->wage[b->type];
		timeEarn = earn / seconds;

		//if(distWage < bestDistWage)
		if(timeEarn < bestTimeEarn)
			continue;
		/*
		fullquota = true;
		float prodquota;

		for(int j=0; j<RESOURCES; j++)
		{
		if(t->output[j] <= 0.0f)
		continue;

		//res = &g_resource[j];

		//if(res->capacity)
		//	prodquota = t->output[j];
		//else
		prodquota = b->prodquota * t->output[j];

		if(b->quotafilled[j] < prodquota)
		{
		fullquota = false;
		break;
		}
		}

		if(fullquota)
		continue;*/

		bestTarget = i;
		bestJobType = NORMJOB;
		//bestDistWage = distWage;
		bestTimeEarn = timeEarn;
	}


	//LastNum("after truck job 2");

	//Road construction jobs
	for(int x=0; x<g_map.m_widthX; x++)
	{
		for(int z=0; z<g_map.m_widthZ; z++)
		{
			r = RoadAt(x, z);

			if(!r->on)
				continue;

			if(r->finished)
				continue;

			if(r->conmat[LABOUR] >= g_roadCost[LABOUR])
				continue;

			p = &g_player[r->owner];

			if(p->global[CURRENC] < p->conwage)
			{
				Bankrupt(r->owner, "road construction");
				continue;
			}

			mode = GOINGTOROADJOB;
			target = x;
			target2 = z;
			goal = RoadPosition(x, z);

			pathed = true;

			if(!Pathfind())
			{

				//char msg[128];
				//sprintf(msg, "r%d,%d !path #%d", x, z, UnitID(this));
				//Chat(msg);
				//Chat("road inacc");

				continue;
			}
			//Chat("road good");

			dist = pathlength();
			//dist = Magnitude2(pos - RoadPosition(x, z));
			//distWage = p->conwage / dist;

			float seconds = dist / ut->speed / (float)FRAME_RATE;
			float reqlab = min(labour, r->netreq(LABOUR));
			seconds += reqlab * (float)WORK_DELAY / 1000.0f;
			float earn = reqlab * p->conwage;
			timeEarn = earn / seconds;

			//if(distWage < bestDistWage)
			if(timeEarn < bestTimeEarn)
				continue;

			bestTargetX = x;
			bestTargetZ = z;
			bestJobType = ROADJOB;
			//bestDistWage = distWage;
			bestTimeEarn = timeEarn;
		}
	}

	//LastNum("after truck job 3");

	//Pipeline construction jobs
	for(int x=0; x<g_map.m_widthX; x++)
	{
		for(int z=0; z<g_map.m_widthZ; z++)
		{
			pipe = PipeAt(x, z);

			if(!pipe->on)
				continue;

			if(pipe->finished)
				continue;

			if(pipe->conmat[LABOUR] >= g_pipelineCost[LABOUR])
				continue;

			p = &g_player[pipe->owner];

			if(p->global[CURRENC] < p->conwage)
			{
				Bankrupt(pipe->owner, "pipeline construction");
				continue;
			}

			mode = GOINGTOPIPEJOB;
			target = x;
			target2 = z;
			goal = PipelinePhysPos(x, z);

			pathed = true;

			if(!Pathfind())
			{
				//pipe->on = false;
				//TypePipelinesAround(x, z);
				continue;
			}

			dist = pathlength();
			//dist = Magnitude2(pos - PipelinePhysPos(x, z));
			//distWage = p->conwage / dist;

			float seconds = dist / ut->speed / (float)FRAME_RATE;
			float reqlab = min(labour, pipe->netreq(LABOUR));
			seconds += reqlab * (float)WORK_DELAY / 1000.0f;
			float earn = reqlab * p->conwage;
			timeEarn = earn / seconds;

			//if(distWage < bestDistWage)
			if(timeEarn < bestTimeEarn)
				continue;

			bestTargetX = x;
			bestTargetZ = z;
			bestJobType = PIPEJOB;
			//bestDistWage = distWage;
			bestTimeEarn = timeEarn;
		}
	}

	//LastNum("after truck job 4");

	//Powerline construction jobs
	for(int x=0; x<g_map.m_widthX; x++)
	{
		for(int z=0; z<g_map.m_widthZ; z++)
		{
			pow = PowlAt(x, z);

			if(!pow->on)
				continue;

			if(pow->finished)
				continue;

			if(pow->conmat[LABOUR] >= g_powerlineCost[LABOUR])
				continue;

			p = &g_player[pow->owner];

			if(p->global[CURRENC] < p->conwage)
			{
				Bankrupt(pow->owner, "powerline construction");
				continue;
			}

			mode = GOINGTOPOWLJOB;
			target = x;
			target2 = z;
			goal = PowerlinePosition(x, z);

			pathed = true;

			if(!Pathfind())
			{
				//pow->on = false;
				//TypePowerlinesAround(x, z);
				continue;
			}

			dist = pathlength();
			//dist = Magnitude2(pos - PipelinePhysPos(x, z));
			//distWage = p->conwage / dist;

			float seconds = dist / ut->speed / (float)FRAME_RATE;
			float reqlab = min(labour, pow->netreq(LABOUR));
			seconds += reqlab * (float)WORK_DELAY / 1000.0f;
			float earn = reqlab * p->conwage;
			timeEarn = earn / seconds;

			//if(distWage < bestDistWage)
			if(timeEarn < bestTimeEarn)
				continue;

			bestTargetX = x;
			bestTargetZ = z;
			bestJobType = POWLJOB;
			//bestDistWage = distWage;
			bestTimeEarn = timeEarn;
		}
	}

	//ResetGoal();
	//LastNum("after truck job 5");

	if(bestJobType == NONE)
	{
		//LastNum("after truck job 5a");
		//char msg[128];
		//sprintf(msg, "none j %d", UnitID(this));
		//Chat(msg);
		if(pathed)
			frameslookjobago = 0;
		ResetMode();
		return false;
	}
	else
	{
		//LastNum("after truck job 5b");
		ResetGoal();
	}

	//string jobname = "unknown";

	if(bestJobType == CONJOB)
	{
		//LastNum("after truck job 5c");
		mode = GOINGTOCONJOB;
		target = bestTarget;
		goal = g_building[target].pos;
		//jobname = "construction job";
	}
	else if(bestJobType == NORMJOB)
	{
		//LastNum("after truck job 5d");
		mode = GOINGTONORMJOB;
		target = bestTarget;
		goal = g_building[target].pos;
		//jobname = "normal job";
	}
	else if(bestJobType == ROADJOB)
	{
		//LastNum("after truck job 5e");
		mode = GOINGTOROADJOB;
		target = bestTargetX;
		target2 = bestTargetZ;
		goal = RoadPosition(bestTargetX, bestTargetZ);
		//jobname = "road job";
	}
	else if(bestJobType == POWLJOB)
	{
		//LastNum("after truck job 5f");
		mode = GOINGTOPOWLJOB;
		target = bestTargetX;
		target2 = bestTargetZ;
		goal = PowerlinePosition(bestTargetX, bestTargetZ);
		//jobname = "powl job";
	}
	else if(bestJobType == PIPEJOB)
	{
		//LastNum("after truck job 5g");
		mode = GOINGTOPIPEJOB;
		target = bestTargetX;
		target2 = bestTargetZ;
		goal = PipelinePhysPos(bestTargetX, bestTargetZ);
		//jobname = "pipe job";
	}
	else if(bestJobType == DRIVING)
	{
		//LastNum("after truck job 5h");
		mode = GOINGTOTRUCK;
		target = bestTarget;
		goal = g_unit[target].camera.Position();
		//jobname = "drive job";
	}
	//LastNum("after truck job 6");

	//Pathfind();
	//float pathlen = pathlength();

	//g_log<<"found job ["<<jobname<<"] path length = "<<pathlen<<" ("<<(pathlen/TILE_SIZE)<<" tiles)"<<endl;
	//g_log.flush();
#endif
	return true;
}