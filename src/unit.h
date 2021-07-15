#ifndef _UNIT_H
#define _UNIT_H



enum {
	UNIT_PEASANT,
	UNIT_WARRIOR,
	UNIT_VETERAN,
	UNIT_HERO,
	UNIT_HUNTER,
	UNIT_ARCHER,
	UNIT_RANGER,
	UNIT_RIDER,
	UNIT_KNIGHT,
	UNIT_KING,
	UNIT_CATAPULT,
	UNIT_SHIP,
	UNIT_TYPES,
};

enum {
	_UNIT_PEASANT				= 1<<UNIT_PEASANT,
	_UNIT_WARRIOR				= 1<<UNIT_WARRIOR,
	_UNIT_VETERAN				= 1<<UNIT_VETERAN,
	_UNIT_HERO					= 1<<UNIT_HERO,
	_UNIT_HUNTER				= 1<<UNIT_HUNTER,
	_UNIT_ARCHER				= 1<<UNIT_ARCHER,
	_UNIT_RANGER				= 1<<UNIT_RANGER,
	_UNIT_RIDER					= 1<<UNIT_RIDER,
	_UNIT_KNIGHT				= 1<<UNIT_KNIGHT,
	_UNIT_KING					= 1<<UNIT_KING,
	_UNIT_CATAPULT				= 1<<UNIT_CATAPULT,
	_UNIT_SHIP					= 1<<UNIT_SHIP,
};


enum {
	U_SELECTED					= 0x00000001,
	U_MOVED						= 0x00000002,

	U_RANGED						= 0x00000010,
	U_BOMBARD					= 0x00000020,
	U_TRANSPORT					= 0x00000f00,
	U_TRANSPORT_S				= 0x00000100,
	U_TRANSPORT_M				= 0x00000200,
	U_TRANSPORT_L				= 0x00000300,
	U_TRANSPORT_XL				= 0x00000400,
	U_FLOATING					= 0x00001000,
	U_FLYING						= 0x00002000,

	U_WORK						= 0x00ff0000,
	U_BUILD						= 0x000f0000,
	U_BUILD_VILLAGE			= 0x00010000,
	U_BUILD_FARM				= 0x00020000,
	U_BUILD_SAWMILL			= 0x00030000,
	U_BUILD_MINE				= 0x00040000,
	U_BUILD_TOWER				= 0x00050000,

	U_CUT_FOREST				= 0x00100000,
	U_DIG_ROAD					= 0x00200000,
	U_HARVEST					= 0x00300000,

	U_FORTIFY					= 0x01000000,
};

enum {
	ATT_MELEE_ATTACK,
	ATT_MELEE_DEFEND,
	ATT_RANGED_ATTACK,
	ATT_RANGED_DEFEND,
};


struct unit_type {
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
	uint16_t work;									//!< Turns to build unit.
	uint16_t cost;									//!< Cost in gold to build unit.
	uint16_t maintain;							//!< Cost in gold per turn to maintain unit.
	//! Move and attack values for different terrain types.
	struct {
		int16_t moves;								//!< Number of moves it cost to move one step in this terrain.
		int16_t attack;							//!< Attack strength in this terrain.
		int16_t defence;							//!< Defence strength in this terrain.
	} terrain[TERRAIN_TYPES];
};

extern unit_type unit_types[];


class Player;
class City;
class Trail;

class Unit {
public:
	Player *player;
	City *owner;
	uint32_t id;
	uint32_t status;
	int16_t x;
	int16_t y;
	uint16_t type;
	int16_t moves;
	uint8_t work;
	uint16_t xp;
	Trail *tr;
	Unit *transp;

	Unit();
	Unit(City &c,int id,int st,int x,int y,int t);
	~Unit();

	void setType(int n);
	int defence();
	int attack();
};



#endif /*_UNIT_H*/


