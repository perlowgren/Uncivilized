
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <amanita/aRandom.h>

#include "main.h"
#include "city.h"
#include "unit.h"
#include "path.h"


/*struct unit_type {
	const char *code;								//!< Code that will be translated to the name.
	const char *name;								//!< Name of the unit type.
	uint8_t sign;									//!< Icon for unit type.
	uint8_t level;									//!< Level of unit type.
	uint8_t sprite;								//!< Sprite the unit type uses.
	uint16_t moves;								//!< Number of moves.
	uint32_t status;								//!< Default status of unit type.
	uint32_t actions;								//!< What actions this unit can do.
	uint64_t city_buildings;					//!< Buildings that are required for a city to build this unit type.
	uint64_t upgrade;								//!< Unittype this unittype upgrades to.
	//! Cost to build and maintain a unit of this type.
	struct {
		uint16_t build;							//!< Cost in resources to build unit.
		uint16_t maintain;						//!< Cost in resources per turn to maintain unit.
	} cost[RES];
	//! Move and attack values for different terrain types.
	struct {
		int16_t moves;								//!< Number of moves it cost to move one step in this terrain.
		int16_t attack;							//!< Attack strength in this terrain.
		int16_t defence;							//!< Defence strength in this terrain.
	} terrain[TERRAIN_TYPES];
};
*/
unit_type unit_types[] = {
	{
		_("Worker"),0,0,0,
		2,0,U_WORK|U_FORTIFY,_BLD_VILLAGE_HALL,_UNIT_WARRIOR|_UNIT_HUNTER|_UNIT_RIDER,
		1,10,2,
		{{0,0,0},{1,1,1},{2,1,1},{3,1,1},{1,1,1}},
	},{
		_("Warrior"),1,0,1,
		2,0,U_FORTIFY,_BLD_BARRACKS,_UNIT_VETERAN,
		2,10,6,
		{{0,0,0},{1,3,3},{2,3,3},{3,3,3},{1,3,3}},
	},{
		_("Veteran"),2,0,2,
		2,0,U_FORTIFY,_BLD_BARRACKS,_UNIT_HERO,
		1,10,18,
		{{0,0,0},{1,5,5},{2,5,5},{3,5,5},{1,5,5}},
	},{
		_("Hero"),2,0,2,
		3,0,U_FORTIFY,_BLD_BARRACKS,0,
		2,10,54,
		{{0,0,0},{1,7,7},{2,7,7},{3,7,7},{1,7,7}},
	},{
		_("Hunter"),3,0,3,
		3,U_RANGED,U_FORTIFY,_BLD_BARRACKS|_BLD_ARCHERY_RANGE,_UNIT_ARCHER,
		2,10,3,
		{{0,0,0},{1,2,2},{2,2,2},{3,2,2},{1,2,2}},
	},{
		_("Archer"),3,0,3,
		3,U_RANGED,U_FORTIFY,_BLD_BARRACKS|_BLD_ARCHERY_RANGE,_UNIT_RANGER,
		1,10,9,
		{{0,0,0},{1,4,4},{2,4,4},{3,4,4},{1,4,4}},
	},{
		_("Ranger"),3,0,3,
		4,U_RANGED,U_FORTIFY,_BLD_BARRACKS|_BLD_ARCHERY_RANGE,0,
		2,10,27,
		{{0,0,0},{1,6,6},{2,6,6},{3,6,6},{1,6,6}},
	},{
		_("Rider"),4,0,4,
		4,0,U_FORTIFY,_BLD_BARRACKS|_BLD_STABLES,_UNIT_KNIGHT,
		2,10,8,
		{{0,0,0},{1,4,4},{2,4,4},{3,4,4},{1,4,4}},
	},{
		_("Knight"),5,0,5,
		4,0,U_FORTIFY,_BLD_BARRACKS|_BLD_STABLES|_BLD_FORGE,_UNIT_KING,
		1,10,24,
		{{0,0,0},{1,6,6},{2,6,6},{3,6,6},{1,6,6}},
	},{
		_("King"),5,0,5,
		5,0,U_FORTIFY,_BLD_BARRACKS|_BLD_STABLES|_BLD_FORGE,0,
		2,10,72,
		{{0,0,0},{1,8,8},{2,8,8},{3,8,8},{1,8,8}},
	},{
		_("Catapult"),6,0,6,
		1,U_RANGED|U_BOMBARD,U_FORTIFY,_BLD_BARRACKS,0,
		2,20,2,
		{{0,0,0},{2,1,1},{3,1,1},{5,1,1},{2,1,1}},
	},{
		_("Ship"),7,0,7,
		4,U_TRANSPORT_S|U_FLOATING,U_FORTIFY,_BLD_HARBOR,0,
		2,20,2,
		{{1,1,1},{0,0,0},{0,0,0},{0,0,0},{1,1,1}},
	}
};



Unit::Unit() : player(0),owner(0),id(0),status(0),x(0),y(0),type(0) {
	moves = 0;
	work = 0;
	xp = 0;
	tr = 0;
	transp = 0;
}

Unit::Unit(City &c,int i,int s,int x,int y,int t) : player(c.player),owner(&c),id(i),status(s),x(x),y(y) {
	work = 0;
	xp = 0;
	tr = 0;
	transp = 0;
	setType(t);
}

Unit::~Unit() {
	if(tr) { delete tr;tr = 0; }
}

void Unit::setType(int n) {
	unit_type &t = unit_types[n];
	type = n;
	moves = t.moves;
}

int Unit::defence() {
	int n = unit_types[type].terrain[app.map.tiles[y*app.map.width+x].terrain].defence;
	if(xp>=5) n++;
	if(xp>=25) n++;
	return n;
}

int Unit::attack() {
	int n = unit_types[type].terrain[app.map.tiles[y*app.map.width+x].terrain].attack;
	if(xp>=5) n++;
	if(xp>=25) n++;
	return n;
}



