#ifndef _PAINT_H
#define _PAINT_H


enum {
	TILE_WIDTH					= 45,		// Width of a hex tile.
	TILE_HEIGHT					= 38,		// Height of a hex tile.
	TILE_RECT_WIDTH			= 70,		// Width of a rectangular tile, which is twice the width of a hex minus right corner.
	TILE_RECT_HEIGHT			= 19,		// Height of a rectangular tile, which is about half the hex tile height (lower part fits into other tiles).
	TILE_HALF_RECT_WIDTH		= 35,		// Width of a hex tile minus right corner (right corner isn't calculated because it fits into other tiles).
	TILE_HALF_RECT_HEIGHT	= 10,		// Half the height of a rectangular tile, which is ((tile height)-(rectangular tile height))/2.
	TILE_CORNER_WIDTH			= 10,		// Width of left and right corner of hex tile, which is (tile width)-(half rectangular tile width).
	TILE_CENTER_X				= 23,		// Width to center of tile. It's half the width of hex tile.
	TILE_CENTER_Y				= 23,		// Height to center of tile. It's a little more than half the height of hex tile.

	CITY_BOX_WIDTH				= 132,
	UNIT_BOX_WIDTH				= 150,

	WORK_WIDTH					= 12,
	WORK_HEIGHT					= 12,
	BUILDING_ICON_WIDTH		= 18,
	BUILDING_ICON_HEIGHT		= 24,
	UNIT_ICON_WIDTH			= 18,
	UNIT_ICON_HEIGHT			= 24,
	UNIT_SPRITE_WIDTH			= 27,
	UNIT_SPRITE_HEIGHT		= 27,
	RESOURCE_WIDTH				= 12,
	RESOURCE_HEIGHT			= 12,
	ROMAN_WIDTH					= 13,
	ROMAN_HEIGHT				= 9,
	FONT_WIDTH					= 5,
	FONT_HEIGHT					= 9,

	ICON_ALIGN_TOP				= 0x0001,
	ICON_ALIGN_CENTER			= 0x0002,
	ICON_ALIGN_BOTTOM			= 0x0004,
	ICON_ACTIVE					= 0x0008,
	ICON_DISABLED				= 0x0010,
};


struct tile_rectangle {
	int x;
	int y;
	int width;
	int height;
	int center_x;
	int center_y;
};


#endif /* _PAINT_H */


