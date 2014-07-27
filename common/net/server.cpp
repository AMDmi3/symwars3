

#include "server.h"

#ifdef _SERVER

#include "../database/mysql.h"
#include "../server/svmain.h"
#include "client.h"
#include "../sim/player.h"

/*
bool Register(char* username, char* email, char* crypt)
{
	char query[256];
	sprintf(query, "INSERT INTO players (username, email, crypt) VALUES ('%s', '%s', '%s')", username, email, crypt);
	return MySQLQuery(query);
}

bool UsernameExists(char* username)
{
	char query[256];
	sprintf(query, "SELECT * FROM players WHERE username='%s'", username);
	MySQLQuery(query);

	if(MySQLFetch())
		return true;

	return false;
}
*/

/*
bool EmailExists(char* email)
{
	char query[256];
	sprintf(query, "SELECT * FROM players WHERE email='%s'", email);
	MySQLQuery(query);

	if(MySQLFetch())
		return true;

	return false;
}

bool LoadPlayer(char* username, Client* c, char* rawusername)
{
	char query[256];
	sprintf(query, "SELECT * FROM players WHERE username='%s'", username);
	MySQLQuery(query);

	if(!MySQLFetch())
		return false;

	int client = MatchClient(c);
	int player = NewPlayer();
	CPlayer* p = &g_player[player];
	p->on = true;
	p->client = client;
	strcpy(p->username, rawusername);
	c->player = player;

	MySQLGetInt(MYSQL_PLAYERS_ID, &p->id);

	return true;
}

void CheckFirst()
{
	char query[128];
	sprintf(query, "SELECT * FROM entities");
	MySQLQuery();
	
	if(!MySQLFetch())
		g_first = true;
	else
		g_first = false;
}

void LoadEntities()
{
	char query[128];
	sprintf(query, "SELECT * FROM entities");
	MySQLQuery();

	int i;
	CEntity* e;
	char lmodel[32];

	while(MySQLFetch())
	{
		i = NewEntity();
		e = &g_entity[i];
		MySQLGetInt(MYSQL_ENTITIES_ID, &e->id);
		
	}
}

void InsertEntity(CEntity* e)
{
	char map[64];
	EscapeString(g_map[e->map].name, map);

	CEntityType* t = &g_entityType[e->type];
	char lmodel[64];
	EscapeString(t->lmodel, lmodel);

	CCamera* c = &e->camera;
	CVector3 p = c->Position();

	char query[1024];
	sprintf(query, "INSERT INTO entities (map, lmodel, posx, posy, posz, yaw) VALUES ('%s', '%s', %f, %f, %f, %f)", map, lmodel, p.x, p.y, p.z, c->Yaw());
	MySQLQuery(query);

	sprintf(query, "SELECT * FROM entities ORDER BY id DESC");
	MySQLQuery(query);
	MySQLFetch();
	MySQLGetInt(MYSQL_ENTITIES_ID, &e->id);
}

void SaveAll()
{
}
*/

#endif


