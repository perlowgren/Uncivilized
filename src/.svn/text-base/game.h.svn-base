#ifndef _GAME_H
#define _GAME_H


#define ALLIANCES 6

enum {
	TILE_FOG_OF_WAR			= 0x0001,
	TILE_MARKER					= 0x0002,
	TILE_BORDER					= 0x0004,
	TILE_RESOURCE				= 0x0008,
	TILE_MAP						= 0x000f,
	TILE_BOX						= 0x0008
};

enum {
	AREA_CB_RESPROD,
	AREA_CB_BUILDING,
	AREA_CB_RESOURCE,
	AREA_CB_BLDPROD,
	AREA_CB_UNITPROD,
	AREA_UB_UNIT,
	AREA_UB_ACTION,
};

enum {
	TOOLTIP_RESOURCE,
	TOOLTIP_BUILDING,
	TOOLTIP_BUILDING_BUILD,
	TOOLTIP_UNIT,
	TOOLTIP_UNIT_BUILD,
	TOOLTIP_UNIT_ACTION,
};

enum {
	ICON_BUILD_VILLAGE,
	ICON_BUILD_FARM_OR_SAWMILL,
	ICON_BUILD_MINE,
	ICON_BUILD_TOWER,
	ICON_CUT_FOREST,
	ICON_DIG_ROAD,
	ICON_HARVEST,
	ICON_CANCEL,
};

enum {
	ATT_ICON_STOCK,
	ATT_ICON_TURNS,
	ATT_ICON_MOVES,
	ATT_ICON_HP,
	ATT_ICON_ATTACK,
	ATT_ICON_DEFENCE,
	ATT_ICON_RANGED,
	ATT_ICON_BOMBARD,
	ATT_ICON_LIEUTENANT,
	ATT_ICON_CAPTAIN,
	ATT_ICON_MAJOR,
	ATT_ICON_COLONEL,
	ATT_ICON_GENERAL,
};

enum {
	STAT_MOVES,
	STAT_HP,
	STAT_MAGIC,
};

/** Game status flags
 */
enum {
	GAME_ENDED					= 0x0001,
	GAME_MAP_HWRAP				= 0x0002,
	GAME_MAP_VWRAP				= 0x0004,
	GAME_SHOW_MAP				= 0x0008,
	GAME_SHOW_FOW				= 0x0010,
	GAME_INVITE_NEWCOMERS	= 0x0100,
};

/** Terrain types
 */
enum {
	TER_WATER,
	TER_LAND,
	TER_FOREST,
	TER_MOUNTAIN,
	TER_BUILDING,
	TERRAIN_TYPES,
};

enum {
	_TER_WATER					= 0x0001,
	_TER_LAND					= 0x0002,
	_TER_FOREST					= 0x0004,
	_TER_MOUNTAIN				= 0x0008,
	_TER_BUILDING				= 0x0010,
};


/** Resource types
 */
enum {
	RES_POPULATION,
	RES_WORK,
	RES_GOLD,
	RES_FOOD,
	RES_TIMBER,
	RES_STONE,
	RES_IRON,
	RES,

	RES_FISH,
	RES_HORSE,
	RES_ALL,
};

/** Player status flags
 */
enum {
	PL_AI_PASSIVE				= 0x0001,
	PL_AI_SIMPLE				= 0x0002,
	PL_AI_NORMAL				= 0x0003,
	PL_AI_HARD					= 0x0004,
	PL_AI							= 0x000f,
	PL_PENDING					= 0x0010,
	PL_END_TURN					= 0x0020,
};



struct terrain_action {
	int terrain;
	int action;
	int icon;
	int work;
	bool owned;
};

extern const terrain_action terrain_actions[];


class Player {
public:
	uint32_t id;			//!< Client ID
	char nick[13];
	uint32_t status;
	uint8_t alliance;
	uint8_t rank;
	uint8_t title;

	uint16_t index;		//!< Player index value. Used internally for Fog-of-War-map.
	uint32_t cities;
	uint32_t units;

	Player() : id(0),status(0),rank(0),title(0) { *nick = '\0'; }
	Player(uint32_t id,const char *n,int s);
	~Player() {}
};


#include "city.h"
#include "unit.h"


struct tile {
	int16_t x;
	int16_t y;
	uint8_t terrain;
	uint8_t resource;
	uint8_t construction;
	City *city;
	City *owner;
	Unit *unit;
};



#endif /*_GAME_H*/


