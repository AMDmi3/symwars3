#ifndef TRACE_H
#define TRACE_H

class Vec2i;
class Unit;
class Building;

int Trace(int utype, int umode,
		  Vec2i vstart, Vec2i vend,
		  Unit* thisu, Unit* ignoreu, Building* ignoreb);

#endif
