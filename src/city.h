#ifndef _CITY_H
#define _CITY_H


#define CITY_COORD_TO_ID(c) ((c)+1)
#define CITY_ID_TO_COORD(id) ((id)-1)

enum {
	CONSTR_NONE,
	CONSTR_CITY,
	CONSTR_FARM,
	CONSTR_SAWMILL,
	CONSTR_MINE,
	CONSTR_TOWER,
	CONSTR_TYPES,
};

enum {
	BLD_WALLS,
	BLD_FORT,
	BLD_CITADEL,
	BLD_CASTLE,
	BLD_VILLAGE_HALL,
	BLD_TOWN_HALL,
	BLD_CITY_HALL,
	BLD_BARRACKS,
	BLD_ARCHERY_RANGE,
	BLD_STABLES,
	BLD_FORGE,
	BLD_MARKET,
	BLD_HARBOR,
	BLD_SHIPYARD,
	BLD_BUILDINGS,

	BLD_BUILD_BUILDING			= 0x40,
};

enum {
	_BLD_WALLS						= 1<<BLD_WALLS,
	_BLD_FORT						= 1<<BLD_FORT,
	_BLD_CITADEL					= 1<<BLD_CITADEL,
	_BLD_CASTLE						= 1<<BLD_CASTLE,
	_BLD_VILLAGE_HALL				= 1<<BLD_VILLAGE_HALL,
	_BLD_TOWN_HALL					= 1<<BLD_TOWN_HALL,
	_BLD_CITY_HALL					= 1<<BLD_CITY_HALL,
	_BLD_BARRACKS					= 1<<BLD_BARRACKS,
	_BLD_ARCHERY_RANGE			= 1<<BLD_ARCHERY_RANGE,
	_BLD_STABLES					= 1<<BLD_STABLES,
	_BLD_FORGE						= 1<<BLD_FORGE,
	_BLD_MARKET						= 1<<BLD_MARKET,
	_BLD_HARBOR						= 1<<BLD_HARBOR,
	_BLD_SHIPYARD					= 1<<BLD_SHIPYARD,
	_BLD_BUILDINGS					= ~BLD_BUILD_BUILDING,
};

enum {
	C_HAS_WATER						= 0x000001,								//!< The city is next to water making it possible to build ships.
	C_HAS_LAND						= 0x000002,								//!< The city is next to land.
	C_HAS_FOREST					= 0x000004,								//!< The city is next to forest.
	C_HAS_MOUNTAIN					= 0x000008,								//!< The city is next to mountain.
};


struct city_building {
	const char *name;					//!< The name of the building.
	uint8_t sign;						//!< Guild sign for the building, the icon representing the building in the city box.
	uint8_t level;						//!< Level of the building.
	uint32_t status;					//!< Status required of the city to build (e.g. has water for a shipyard).
	uint64_t required;				//!< Buildings required to build.
	uint64_t replace;					//!< Buildings that is replaced by this building.
	uint16_t work;						//!< How many turns to produce.
	uint16_t cost;						//!< Cost in gold to build.
	uint16_t maintain;				//!< Cost in gold to maintain.
};

extern city_building city_buildings[];

struct tile;

class City {
public:
	Player *player;
	City *owner;
	uint32_t id;
	uint32_t status;
	uint8_t sprite;
	int16_t x;
	int16_t y;
	uint64_t buildings;
	int16_t work;
	int32_t gold;
	int16_t income;
	int16_t wages;
	struct {
		int8_t type;
		int8_t queue;
		uint64_t unit_types;
	} build;

	City();
	City(Player *p,int i,int s,int x,int y);
	~City() {}

	bool setProduction(int pr);
	uint64_t calculateUnitTypes();
	bool canBuild(int b);
	uint64_t getAllBuildings();
	uint64_t getTopBuildings();
	uint64_t getCanBuildBuildings();
	bool canProduce(int u);
	uint64_t getCanBuildUnits();

	int defence();
};




#endif /*_CITY_H*/


