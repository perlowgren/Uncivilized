
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <amanita/aRandom.h>

#include "main.h"
#include "path.h"


/*
enum {
	_BLD_WALLS						= 1<<BLD_WALLS,
	_BLD_FORT						= 1<<BLD_FORT,
	_BLD_CITADEL					= 1<<BLD_CITADEL,
	_BLD_CASTLE					= 1<<BLD_CASTLE,
	_BLD_VILLAGE_HALL			= 1<<BLD_VILLAGE_HALL,
	_BLD_TOWN_HALL				= 1<<BLD_TOWN_HALL,
	_BLD_CITY_HALL				= 1<<BLD_CITY_HALL,
	_BLD_BARRACKS					= 1<<BLD_BARRACKS,
	_BLD_ARCHERY_RANGE			= 1<<BLD_ARCHERY_RANGE,
	_BLD_STABLES					= 1<<BLD_STABLES,
	_BLD_FORGE						= 1<<BLD_FORGE,
	_BLD_MARKET					= 1<<BLD_MARKET,
	_BLD_HARBOR					= 1<<BLD_HARBOR,
	_BLD_SHIPYARD					= 1<<BLD_SHIPYARD,
	_BLD_BUILDINGS				= ~BLD_BUILD_BUILDING,
};
struct city_building {
	const char *code;					//!< The code that will be translated to the name of the building.
	const char *name;					//!< The name of the building.
	uint8_t sign;						//!< Guild sign for the building, the icon representing the building in the city box.
	uint8_t level;						//!< Level of the building.
	uint32_t population;				//!< Minimum population required to build.
	uint32_t status;					//!< Status required of the city to build (e.g. has water for a shipyard).
	uint64_t required;				//!< Buildings required to build.
	uint64_t replace;					//!< Buildings that is replaced by this building.
	struct {
		uint16_t build;				//!< Cost in resources to build.
		uint16_t maintain;			//!< Cost in resources to maintain.
	} cost[RES];
};
*/

city_building city_buildings[] = {
	{
		_("Walls"),0,0,
		0,
		0,
		0,
		1,10,0
	},{
		_("Fort"),0,1,
		0,
		_BLD_WALLS,
		_BLD_WALLS,
		2,20,0,
	},{
		_("Citadel"),0,2,
		0,
		_BLD_FORT,
		_BLD_WALLS|_BLD_FORT,
		15,150,0,
	},{
		_("Castle"),0,3,
		0,
		_BLD_CITADEL,
		_BLD_WALLS|_BLD_FORT|_BLD_CITADEL,
		20,500,0,
	},{
		_("Village Hall"),1,0,
		0,
		_BLD_WALLS,
		0,
		1,5,0,
	},{
		_("Town Hall"),1,1,
		0,
		_BLD_VILLAGE_HALL,
		_BLD_VILLAGE_HALL,
		1,10,0,
	},{
		_("City Hall"),1,2,
		0,
		_BLD_TOWN_HALL,
		_BLD_VILLAGE_HALL|_BLD_TOWN_HALL,
		5,100,0,
	},{
		_("Barracks"),2,0,
		0,
		_BLD_VILLAGE_HALL,
		0,
		1,10,0,
	},{
		_("Archery Range"),3,0,
		0,
		_BLD_BARRACKS,
		0,
		1,10,0,
	},{
		_("Stables"),4,0,
		0,
		0,
		0,
		1,10,0,
	},{
		_("Forge"),5,0,
		0,
		0,
		0,
		1,10,0,
	},{
		_("Market"),6,0,
		0,
		_BLD_VILLAGE_HALL,
		0,
		1,10,0,
	},{
		_("Harbor"),7,0,
		C_HAS_WATER,
		_BLD_VILLAGE_HALL,
		0,
		3,30,0,
	},{
		_("Shipyard"),7,1,
		0,
		_BLD_HARBOR,
		_BLD_HARBOR,
		5,100,0,
	},
};


City::City() : player(0),id(0),status(0),sprite(0),x(0),y(0) {
	buildings = _BLD_VILLAGE_HALL;
	work = 0;
	gold = 0;
	income = 0;
	wages = 0;
	build.type = -1;
	build.queue = -1;
	build.unit_types = calculateUnitTypes();
}

City::City(Player *p,int i,int s,int x,int y) : player(p),id(i),status(s),sprite(0),x(x),y(y) {
	buildings = _BLD_VILLAGE_HALL;
	work = 0;
	gold = 0;
	income = 0;
	wages = 0;
	build.type = -1;
	build.queue = -1;
	build.unit_types = calculateUnitTypes();
}

bool City::setProduction(int pr) {
debug_output("City::setCityProduction(pr=%d)\n",pr);
	if(pr&BLD_BUILD_BUILDING) {
		pr &= _BLD_BUILDINGS;
debug_output("City::setCityProduction(building=%d)\n",pr);
		uint64_t b = getCanBuildBuildings();
		if(pr>=0 && pr<BLD_BUILDINGS && (b&(1<<pr))) {
			if(canBuild(pr)) {
				if(build.type>=0) build.queue = pr|BLD_BUILD_BUILDING;
				else {
					build.type = pr|BLD_BUILD_BUILDING;
					build.queue = -1;
					gold -= city_buildings[pr].cost;
				}
				return true;
			}
		}
	} else if(pr>=0 && pr<UNIT_TYPES && (build.unit_types&(1<<pr))) {
		if(canProduce(pr)) {
			if(build.type>=0) build.queue = pr;
			else {
				build.type = pr;
				build.queue = -1;
				gold -= unit_types[pr].cost;
			}
			return true;
		}
	}
	build.type = -1;
	build.queue = -1;
	return false;
}

uint64_t City::calculateUnitTypes() {
	uint64_t n = getAllBuildings();
	uint64_t u = 0;
	for(int i=0; i<UNIT_TYPES; i++) if((n&unit_types[i].city_buildings)==unit_types[i].city_buildings) u |= 1<<i;
	return u;
}

bool City::canBuild(int b) {
	b &= _BLD_BUILDINGS;
	return gold>=city_buildings[b].cost;
}

uint64_t City::getAllBuildings() {
	uint64_t n = buildings;
	for(int i=0; i<BLD_BUILDINGS; i++) if(buildings&(1<<i)) n |= city_buildings[i].replace;
	return n;
}

uint64_t City::getTopBuildings() {
	uint64_t n = buildings;
	for(int i=0; i<BLD_BUILDINGS; i++) if(buildings&(1<<i)) n &= ~city_buildings[i].replace;
	return n;
}

uint64_t City::getCanBuildBuildings() {
	int i;
	uint64_t b1 = getAllBuildings();
	uint64_t b2 = 0;
	for(i=0; i<BLD_BUILDINGS; i++) if((b1&city_buildings[i].required)==city_buildings[i].required) b2 |= 1<<i;
//debug_output("City::getCanBuildBuildings(buildings=%" PRIx64 ",b1=%" PRIx64 ",b2=%" PRIx64 ")\n",buildings,b1,b2);
	b2 &= ~b1;
	return b2;
}

bool City::canProduce(int u) {
	return gold>=unit_types[u].cost;
}

uint64_t City::getCanBuildUnits() {
	int i;
	uint64_t b = getAllBuildings();
	uint64_t u = 0;
	for(i=0; i<UNIT_TYPES; i++) if((b&unit_types[i].city_buildings)==unit_types[i].city_buildings) u |= 1<<i;
//debug_output("City::getCanBuildBuildings(buildings=%" PRIx64 ",b1=%" PRIx64 ",b2=%" PRIx64 ")\n",buildings,b1,b2);
	return u;
}

int City::defence() {
	return 1;
}


