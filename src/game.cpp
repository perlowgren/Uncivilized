
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <amanita/aMath.h>
#include <amanita/aRandom.h>

#include "main.h"
#include "path.h"


/*	_TER_WATER					= 0x0001,
	_TER_LAND					= 0x0002,
	_TER_FOREST					= 0x0004,
	_TER_MOUNTAIN				= 0x0008,
	_TER_BUILDING				= 0x0010,
	U_BUILD_VILLAGE			= 0x00010000,
	U_BUILD_FARM				= 0x00020000,
	U_BUILD_SAWMILL			= 0x00030000,
	U_BUILD_MINE				= 0x00040000,
	U_BUILD_TOWER				= 0x00050000,

	U_CUT_FOREST				= 0x00100000,
	ICON_BUILD_VILLAGE,
	ICON_BUILD_FARM_OR_SAWMILL,
	ICON_BUILD_MINE,
	ICON_BUILD_TOWER,
	ICON_CUT_FOREST,
	ICON_DIG_ROAD,
	ICON_HARVEST,
	ICON_CANCEL,
struct terrain_action {
	int terrain;
	int action;
	int icon;
	int work;
};*/
const terrain_action terrain_actions[] = {
	{_TER_LAND|_TER_FOREST,						U_BUILD_VILLAGE,	ICON_BUILD_VILLAGE,				2,	false},
	{_TER_LAND,										U_BUILD_FARM,		ICON_BUILD_FARM_OR_SAWMILL,	2,	true},
	{_TER_FOREST,									U_BUILD_SAWMILL,	ICON_BUILD_FARM_OR_SAWMILL,	2,	true},
	{_TER_LAND|_TER_FOREST|_TER_MOUNTAIN,	U_BUILD_MINE,		ICON_BUILD_MINE,					2,	true},
	{_TER_LAND|_TER_FOREST|_TER_MOUNTAIN,	U_BUILD_TOWER,		ICON_BUILD_TOWER,					2,	false},
	{_TER_FOREST,									U_CUT_FOREST,		ICON_CUT_FOREST,					2,	true},
	{-1,-1,-1,-1}
};


Player::Player(uint32_t id,const char *n,int s) {
	id = id;
	strncpy(nick,n,12);
	nick[12] = '\0';
	status = s;
	rank = 0;
	title = 0;
}

void Game::clearPlayers() {
	if(players.size()==0) return;
	aHashtable::iterator iter;
	Player *pl;
	for(iter=players.iterate(); (pl=(Player *)iter.next());) delete pl;
	players.removeAll();
	clients.removeAll();
}

void Game::clearMap() {
	aHashtable::iterator iter;
	if(cities.size()>0) {
		City *c;
		for(iter=cities.iterate(); (c=(City *)iter.next());) delete c;
		cities.removeAll();
	}
	if(units.size()>0) {
		Unit *u;
		for(iter=units.iterate(); (u=(Unit *)iter.next());) delete u;
		units.removeAll();
	}
}


void Game::newGame(int w,int h,int s) {
debug_output("Game::newGame(w=%d,h=%d)\n",w,h);
	int i,n = -1;
	aHashtable::iterator iter;
//	tile *l[7];
	Player *pl;
//	City *c;
//	type_t t;
	running = false;
	turns = 0;
	focus_x = -1;
	focus_y = -1;
	focus_unit = 0;
	view_x = 0;
	view_y = 0;
	setStatus(s);
debug_output("Game::newGame(players=%d)\n",(int)players.size());
	unit_id_index = 0;
debug_output("Game::newGame(creatMmap)\n");
	alliance = -1;

debug_output("Game::newGame(cities=%d,players=%d)\n",(int)cities.size(),(int)players.size());
	memset(alliances,0,sizeof(alliances));
	for(iter=players.iterate(); (pl=(Player *)iter.next());) {
		pl->status = (pl->status&0xff0f)|PL_PENDING;
		pl->alliance = 0;
		pl->rank = 0;
		pl->title = 0;
		pl->index = ++n;
		pl->cities = 0;
		pl->units = 0;
	}
	for(i=0; i<MAX_AI_PLAYERS; i++)
		if(setup.ai[i]>0) {
			pl = new Player();
			pl->id = i+1;
			sprintf(pl->nick,"AI %d",pl->id);
			pl->status = (pl->status&0xff0f)|(setup.ai[i]+1)|PL_PENDING;
			pl->alliance = 0;
			pl->rank = 0;
			pl->title = 0;
			pl->index = ++n;
			pl->cities = 0;
			pl->units = 0;
			clients.put(pl->id,pl);
			players.put(pl->nick,pl);
		}

	clearMap();
	createMap(w,h);

debug_output("Game::newGame(1)\n");
debug_output("Game::newGame(2)\n");
	paintMap();
debug_output("Game::newGame(3)\n");
	updateEconomy();
debug_output("Game::newGame(4)\n");
}

void Game::closeGame() {
	running = false;
	clearMap();
	if(map.tiles) free(map.tiles);
	if(map.fow) free(map.fow);
	map.width = 0;
	map.height = 0;
	map.tiles = 0;
	map.fow = 0;
}


void Game::packGame(int cmd,uint8_t **data,int &len) {
	int i,n;
	char header[256];
	uint8_t *p;
	Player *pl;
	tile *h1;
	Unit *u1;
	City *c1;
	aHashtable::iterator iter;

debug_output("\nSend Game Data:\n");
	sprintf(header,"\n" PACKAGE_STRING "\n");

	i = strlen(header)+1;
	n = SOCKET_HEADER+i+70 // Header and strings
		+4+(24*players.size()) // Players
		+(12) // Data
		+4+(6*map.size) // Map
		+4+(map.size*map.fow_n) // Fog of war
		+4+((24+10+10)*cities.size()) // Cities
		+4+((23+1)*units.size()); // Units
	for(iter=units.iterate(); (u1=(Unit *)iter.next());) // Unit trails
		if(u1->tr) n += u1->tr->bytes();

	*data = (uint8_t *)malloc(n);
	len = n;
	p = *data;
	pack_header(&p,cmd);

	memcpy(p,header,i);
	p += i;
debug_output("Players:\n");
	memcpy(p,"\nPlayers:\n\0",25);
	p += 25;
	pack_uint32(&p,(uint32_t)players.size());
debug_output("Number of players: %d\n",(int)players.size());
	for(iter=players.iterate(); (pl=(Player *)iter.next());) {
		pack_uint32(&p,pl->id);
		memcpy(p,pl->nick,13);
		p += 13;
		pack_uint32(&p,pl->status);
		pack_uint8(&p,pl->alliance);
		pack_uint8(&p,pl->rank);
		pack_uint8(&p,pl->title);
debug_output("[id:%d, nick:\"%s\", status:%d, alliance:%d, rank:%d, title:%d]\n",pl->id,pl->nick,pl->status,pl->alliance,pl->rank,pl->title);
	}
	memcpy(p,"\nGame Data:\n\0",13);
	p += 13;
	pack_uint32(&p,unit_id_index);
	pack_uint32(&p,status);
	pack_uint32(&p,turns);
	memcpy(p,"\nMap:\n\0",7);
	p += 7;
	pack_uint16(&p,map.width);
	pack_uint16(&p,map.height);
	for(i=0,n=map.size,h1=map.tiles; i<n; ++i,++h1) {
		pack_uint8(&p,h1->terrain|(h1->resource<<4));
		pack_uint8(&p,h1->construction);
		pack_uint32(&p,h1->owner? h1->owner->id : 0);
	}
	pack_uint32(&p,map.fow_n);
	memcpy(p,map.fow,map.size*map.fow_n);
	p += map.size*map.fow_n;
	memcpy(p,"\nCities:\n\0",10);
	p += 10;
	pack_uint32(&p,(uint32_t)cities.size());
	for(iter=cities.iterate(); (c1=(City *)iter.next());) {
		pack_uint32(&p,c1->player? c1->player->id : 0);
		pack_uint32(&p,c1->id);
		pack_uint32(&p,c1->status);
		pack_int16(&p,c1->x);
		pack_int16(&p,c1->y);
		pack_uint64(&p,c1->buildings);
		pack_int16(&p,c1->work);
		pack_int32(&p,c1->gold);
		pack_int16(&p,c1->income);
		pack_int16(&p,c1->wages);
		pack_int8(&p,c1->build.type);
		pack_int8(&p,c1->build.queue);
		pack_uint64(&p,c1->build.unit_types);
	}
	memcpy(p,"\nUnits:\n\0",9);
	p += 9;
	pack_uint32(&p,(uint32_t)units.size());
	for(iter=units.iterate(); (u1=(Unit *)iter.next());) {
		pack_uint32(&p,u1->owner->id);
		pack_uint32(&p,u1->id);
		pack_uint32(&p,u1->status);
		pack_int16(&p,u1->x);
		pack_int16(&p,u1->y);
		pack_uint16(&p,u1->type);
		pack_int16(&p,u1->moves);
		pack_uint8(&p,u1->work);
		pack_uint16(&p,u1->xp);
		pack_uint8(&p,u1->tr? 1 : 0);
		if(u1->tr) u1->tr->pack(&p);
	}
	memcpy(p,"\nEnd\n\0",6);
debug_output("\n");
}

void Game::unpackGame(uint8_t *data) {
debug_output("Game::unpackGame(1)\n");
	closeGame();
	int i,n,m;
	uint32_t sz,p,id;
	uint8_t t;
	Player *pl;
	tile *h1;
	Unit *u1;
	City *c1;
	aHashtable::iterator iter;

	data += SOCKET_HEADER;
debug_output("%s",data); // Uncivilized v. X.X.X
	while(*data) ++data;
	++data;
debug_output("%s",data); // Players:
	data += 25;
	unpack_uint32(&data,sz);
debug_output("Number of players: %d\n",sz);
	for(i=0,n=sz; i<(int)n; i++) {
		unpack_uint32(&data,id);
		pl = (Player *)clients.get(id);
		if(pl) {
			memcpy(pl->nick,data,13);
			pl->nick[12] = '\0';
			data += 13;
			unpack_uint32(&data,pl->status);
			unpack_uint8(&data,pl->alliance);
			unpack_uint8(&data,pl->rank);
			unpack_uint8(&data,pl->title);
		} else data += 20;
debug_output("[id:%d, nick:\"%s\", status:%d, alliance:%d, rank:%d, title:%d]\n",pl->id,pl->nick,pl->status,pl->alliance,pl->rank,pl->title);
	}
debug_output("%s",data); // Game Data:
	data += 13;
	unpack_uint32(&data,unit_id_index);
	unpack_uint32(&data,status);
	unpack_uint32(&data,turns);
debug_output("%s",data); // Map:
	data += 7;
	unpack_uint16(&data,map.width);
	unpack_uint16(&data,map.height);
	map.size = map.width*map.height;
	map.width_p = map.width*TILE_RECT_WIDTH;
	map.height_p = map.height*TILE_RECT_HEIGHT;
	map.tiles = (tile *)malloc(sizeof(tile)*map.size);
	memset(map.tiles,0,sizeof(tile)*map.size);
	uint32_t owner[map.size];
	for(i=0,n=map.size,h1=map.tiles; i<n; ++i,++h1) {
		h1->x = i%map.width;
		h1->y = i/map.width;
		unpack_uint8(&data,t);
		h1->terrain = t&0xf;
		h1->resource = t>>4;
		unpack_uint8(&data,h1->construction);
		unpack_uint32(&data,owner[i]);
	}
	unpack_uint32(&data,map.fow_n);
	map.fow = (uint8_t *)malloc(map.size*map.fow_n);
	memcpy(map.fow,data,map.size*map.fow_n);
	data += map.size*map.fow_n;
debug_output("%s",data); // Cities:
	data += 10;
	unpack_uint32(&data,sz);
	for(i=0,n=sz; i<(int)n; ++i) {
		c1 = new City();
		unpack_uint32(&data,p);
		c1->player = (Player *)(p? clients.get(p) : 0);
		unpack_uint32(&data,c1->id);
		unpack_uint32(&data,c1->status);
		unpack_int16(&data,c1->x);
		unpack_int16(&data,c1->y);
		unpack_uint64(&data,c1->buildings);
		unpack_int16(&data,c1->work);
		unpack_int32(&data,c1->gold);
		unpack_int16(&data,c1->income);
		unpack_int16(&data,c1->wages);
		unpack_int8(&data,c1->build.type);
		unpack_int8(&data,c1->build.queue);
		unpack_uint64(&data,c1->build.unit_types);
		m = c1->y*map.width+c1->x;
		map.tiles[m].city = c1;
		cities.put(m,c1);
	}
debug_output("%s",data); // Units:
	data += 9;
	unpack_uint32(&data,sz);
	for(i=0,n=sz; i<(int)n; ++i) {
		u1 = new Unit();
		unpack_uint32(&data,p);
		u1->owner = (City *)cities.get(CITY_ID_TO_COORD(p));
		unpack_uint32(&data,u1->id);
		unpack_uint32(&data,u1->status);
		unpack_int16(&data,u1->x);
		unpack_int16(&data,u1->y);
		unpack_uint16(&data,u1->type);
		unpack_int16(&data,u1->moves);
		unpack_uint8(&data,u1->work);
		unpack_uint16(&data,u1->xp);
		unpack_uint8(&data,t);
		if(t) {
			u1->tr = new Trail();
			u1->tr->unpack(&data);
		}
		map.tiles[u1->y*map.width+u1->x].unit = u1;
		u1->player = u1->owner->player;
		units.put(u1->id,u1);
	}
	for(i=0,n=map.size; i<n; ++i) if(owner[i]) {
		p = owner[i],c1 = (City *)cities.get(CITY_ID_TO_COORD(p));
		for(m=i; m<n; ++m) if(owner[m]==p) map.tiles[m].city = c1,owner[m] = 0;
	}

debug_output("%s",data); // End
	paintMap();
	updateEconomy();
	updateStatus();
debug_output("Game::unpackGame(done)\n");
}

void Game::saveGame(const char *fn) {
	FILE *fp = fopen(fn,"wb");
	uint8_t *data;
	int len;
	packGame(0,&data,len);
	fwrite(data,len,1,fp);
	fclose(fp);
	free(data);
}

void Game::loadGame(const char *fn) {
	size_t n;
	FILE *fp = fopen(fn,"rb");
	fseek(fp,0,SEEK_END);
	int len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	uint8_t *data = (uint8_t *)malloc(len);
	n = fread(data,len,1,fp);
	fclose(fp);
	unpackGame(data);
	free(data);
}

void Game::erode(int *m,int s) {
	for(int i,j,n,x,y=0,x1; y<map.height; y++)
		for(x=0; x<map.width; x++) {
			x1 = x+(y&1),i = 0;
			if(isHWrap() || x1<map.width) {
				if(x1>=map.width) x1 -= map.width;
				i |= y-1>=0? m[(y-1)*map.width+x1] : (isVWrap()? m[(y-1)*map.width+x1] : 0);
				i |= y+1<map.height? (m[(y+1)*map.width+x1]<<1) : (isVWrap()? (m[(y-map.height+1)*map.width+x1]<<1) : 0);
			}
			x1 = x+(y&1)-1;
			if(isHWrap() || x1>=0) {
				if(x1<0) x1 += map.width;
				i |= y+1<map.height? (m[(y+1)*map.width+x1]<<3) : (isVWrap()? (m[(y+1-map.height)*map.width+x1]<<3) : 0);
				i |= y-1>=0? (m[(y-1)*map.width+x1]<<4) : (isVWrap()? (m[(y+map.height-1)*map.width+x1]<<4) : 0);
			}
			i |= y+2<map.height? (m[(y+2)*map.width+x]<<2) : (isVWrap()? (m[(y+2-map.height)*map.width+x]<<2) : 0);
			i |= y-2>=0? (m[(y-2)*map.width+x]<<5) : (isVWrap()? (m[(y+map.height-2)*map.width+x]<<5) : 0);
			if(i) {
				for(j=0,n=0; j<6; j++) if(i&(1<<j)) n++;
				if(s==0) {
					if(m[y*map.width+x]) { if(n<6 && (n==0 || !rnd.uint32(n))) m[y*map.width+x] = 0; }
					else if(n>0 && (n==6 || !rnd.uint32(6-n))) m[y*map.width+x] = 1;
				} else if(s==1) {
					if(!m[y*map.width+x] && (n==6 || !rnd.uint32(6-n))) m[y*map.width+x] = 1;
				} else if(s==2) {
					if(m[y*map.width+x] && (n==0 || !rnd.uint32(n))) m[y*map.width+x] = 0;
				}
			}
		}
}

void Game::createMap(int w,int h) {
	int m[w*h],i,j,x,y;
	tile *l[19];
	City *c;

	if(!map.tiles || w!=map.width || h!=map.height) {
		map.width = w;
		map.height = h;
		map.size = w*h;
		map.width_p = w*TILE_RECT_WIDTH;
		map.height_p = h*TILE_RECT_HEIGHT;
		if(map.tiles) free(map.tiles);
		map.tiles = (tile *)malloc(sizeof(tile)*map.size);
		if(map.fow) free(map.fow);
		map.fow_n = 1+players.size()/8;
		map.fow = (uint8_t *)malloc(4*map.fow_n*map.size);
	}
	memset(map.tiles,0,sizeof(tile)*map.size);

	for(i=map.size*map.fow_n-1; i>=0; i--) map.fow[i] = 0xff; // Set Fog of War

	// Terraform WATER/LAND:
	memset(m,0,sizeof(m));
	for(x=0; x<map.width; x++) for(y=8; y<map.height; y+=8) m[y*map.width+x] = 1;
	for(i=0; i<3; i++) erode(m,1);
	erode(m,2);
	for(i=0; i<3; i++) erode(m,0);
	if(!isHWrap()) for(y=0; y<map.height; y++) {
		m[y*map.width] = 0;
		m[y*map.width+map.width-1] = 0;
	}
	if(!isVWrap()) for(x=0; x<map.width; x++) {
		m[x] = 0;
		m[map.width+x] = 0;
		m[(map.height-2)*map.width+x] = 0;
		m[(map.height-1)*map.width+x] = 0;
	}
	for(i=map.size-1; i>=0; i--) if(m[i]) map.tiles[i].terrain = TER_LAND;

	// Add FOREST:
	memset(m,0,sizeof(m));
	int terrain_types[] = { 10,5,2 };
	int resource_types[][RES_ALL] = {
		{95,0,0,0,0,0,0,0,5,0},
		{92,0,1,4,0,1,0,0,0,2},
		{85,0,0,0,15,0,0,0,0,0},
		{82,0,3,0,0,10,5,0,0,0},
	};
	for(i=map.size-1; i>=0; i--)
		if(map.tiles[i].terrain==TER_LAND) {
			map.tiles[i].terrain = 1+rnd.rollTable(terrain_types,17);
			map.tiles[i].resource = rnd.rollTable(resource_types[map.tiles[i].terrain],100);
		} else {
			getLocation(i%map.width,i/map.width,1,l);
			for(j=1; j<=6; j++) if(l[j] && l[j]->terrain!=TER_WATER) break;
			if(j<=6) map.tiles[i].resource = rnd.rollTable(resource_types[TER_WATER],100);
		}

	// Add CITY:
	for(y=0; y<map.height; y++) for(x=0; x<map.width; x++) {
		if(map.tiles[i=y*map.width+x].terrain && !rnd.uint32(10)) {
			getLocation(x,y,1,l);
			for(j=1; j<=6; j++) if(l[j] && l[j]->construction!=CONSTR_NONE) break;
			if(j==7) {
				map.tiles[i].terrain = TER_BUILDING;
				map.tiles[i].construction = CONSTR_CITY;
			}
		}
	}
	for(y=0; y<map.height; y++) for(x=0; x<map.width; x++) {
		i = y*map.width+x;
		map.tiles[i].x = x;
		map.tiles[i].y = y;
		if(map.tiles[i].construction==CONSTR_CITY) {
			c = new City(0,CITY_COORD_TO_ID(i),0,x,y);
			cities.put(i,c);
			map.tiles[i].city = c;
			map.tiles[i].owner = c;
			getLocation(x,y,1,l);
			int ter[] = {0,0,0,0,0};
			for(j=1; j<7; j++) if(l[j]) ter[l[j]->terrain]++;
			if(ter[TER_WATER]>=1) c->status |= C_HAS_WATER;
			if(ter[TER_LAND]>=1) c->status |= C_HAS_LAND;
			if(ter[TER_FOREST]>=1) c->status |= C_HAS_FOREST;
			if(ter[TER_MOUNTAIN]>=1) c->status |= C_HAS_MOUNTAIN;
		}
	}
}


void Game::pointToMap(int xp,int yp,int &xm,int &ym) {
	static const char data[] = {
		5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	};
	int xh,yh,i;
	ym = (yp-view_y-TILE_HALF_RECT_HEIGHT)/TILE_RECT_HEIGHT;
	xm = (xp-view_x-(ym&1)*TILE_HALF_RECT_WIDTH)/TILE_RECT_WIDTH;
	yh = yp-((ym*TILE_RECT_HEIGHT)+view_y+TILE_HALF_RECT_HEIGHT);
	xh = xp-((xm*TILE_RECT_WIDTH)+view_x+(ym&1)*TILE_HALF_RECT_WIDTH);
	i = data[yh*TILE_RECT_WIDTH+xh];
	if(i==0);
	else if(i==1) xm += ym&1? 1 : 0,ym--;
	else if(i==2) xm += ym&1? 1 : 0,ym++;
	else if(i==4) xm -= ym&1? 0 : 1,ym++;
	else if(i==5) xm -= ym&1? 0 : 1,ym--;
	if(xm>=map.width) xm -= map.width;
	if(ym>=map.height) ym -= map.height;
}

tile **Game::getLocation(int x,int y,int r,tile **l) {
	static tile *l1[7];
	if(!l) l = l1;
	if(x<0) x += map.width;
	else if(x>=map.width) x -= map.width;
	if(y<0) y += map.height;
	else if(y>=map.height) y -= map.height;
	l[0] = &map.tiles[y*map.width+x];
	if(r>=1) {
		int n = y&1,x1;
		if(isHWrap() || x+n<map.width) {
			x1 = x+n<map.width? x+n : x+n-map.width;
			l[1] = y-1>=0? &map.tiles[(y-1)*map.width+x1] : (isVWrap()? &map.tiles[(y+map.height-1)*map.width+x1] : 0);
			l[2] = y+1<map.height? &map.tiles[(y+1)*map.width+x1] : (isVWrap()? &map.tiles[(y+1-map.height)*map.width+x1] : 0);
			if(r>=2) {
				l[7] = y-3>=0? &map.tiles[(y-3)*map.width+x1] : (isVWrap()? &map.tiles[(y+map.height-3)*map.width+x1] : 0);
				l[11] = y+3<map.height? &map.tiles[(y+3)*map.width+x1] : (isVWrap()? &map.tiles[(y+3-map.height)*map.width+x1] : 0);
			}
		} else {
			l[1] = l[2] = 0;
			if(r>=2) l[7] = l[11] = 0;
		}
		n = !n;
		if(isHWrap() || x-n>=0) {
			x1 = x-n>=0? x-n : x+map.width-n;
			l[4] = y+1<map.height? &map.tiles[(y+1)*map.width+x1] : (isVWrap()? &map.tiles[(y+1-map.height)*map.width+x1] : 0);
			l[5] = y-1>=0? &map.tiles[(y-1)*map.width+x1] : (isVWrap()? &map.tiles[(y+map.height-1)*map.width+x1] : 0);
			if(r>=2) {
				l[13] = y+3<map.height? &map.tiles[(y+3)*map.width+x1] : (isVWrap()? &map.tiles[(y-map.height+3)*map.width+x1] : 0);
				l[17] = y-3>=0? &map.tiles[(y-3)*map.width+x1] : (isVWrap()? &map.tiles[(y-3+map.height)*map.width+x1] : 0);
			}
		} else {
			l[4] = l[5] = 0;
			if(r>=2) l[13] = l[17] = 0;
		}
		l[3] = y+2<map.height? &map.tiles[(y+2)*map.width+x] : (isVWrap()? &map.tiles[(y+2-map.height)*map.width+x] : 0);
		l[6] = y-2>=0? &map.tiles[(y-2)*map.width+x] : (isVWrap()? &map.tiles[(y+map.height-2)*map.width+x] : 0);
		if(r>=2) {
			if(isHWrap() || x+1<map.width) {
				x1 = x+1<map.width? x+1 : x+1-map.width;
				l[8] = y-2>=0? &map.tiles[(y-2)*map.width+x1] : (isVWrap()? &map.tiles[(y+map.height-2)*map.width+x1] : 0);
				l[9] = &map.tiles[y*map.width+x1];
				l[10] =y+2<map.height? &map.tiles[(y+2)*map.width+x1] : (isVWrap()? &map.tiles[(y+2-map.height)*map.width+x1] : 0);
			} else l[8] = l[9] = l[10] = 0;
			if(isHWrap() || x-1>=0) {
				x1 = x-1>=0? x-1 : x+map.width-1;
				l[14] = y+2<map.height? &map.tiles[(y+2)*map.width+x-1] : (isVWrap()? &map.tiles[(y+2-map.height)*map.width+x-1] : 0);
				l[15] = &map.tiles[y*map.width+x-1];
				l[16] = y-2>=0? &map.tiles[(y-2)*map.width+x-1] : (isVWrap()? &map.tiles[(y+map.height-2)*map.width+x-1] : 0);
			} else l[14] = l[15] = l[16] = 0;
			l[12] = y+4<map.height? &map.tiles[(y+4)*map.width+x] : (isVWrap()? &map.tiles[(y+4-map.height)*map.width+x] : 0);
			l[18] = y-4>=0? &map.tiles[(y-4)*map.width+x] : (isVWrap()? &map.tiles[(y+map.height-4)*map.width+x] : 0);
		}
	}
	return l;
}

void Game::setViewAtLocation(int x,int y) {
	if(x>=0 && y>=0 && x<map.width && y<map.height) setView(width/2-8-x*TILE_RECT_WIDTH-(y&1)*TILE_HALF_RECT_WIDTH,height/2-y*TILE_RECT_HEIGHT);
}

void Game::setViewAtFocus() {
	if(focus_x>=0 && focus_y>=0 && focus_x<map.width && focus_y<map.height) setView(width/2-8-focus_x*TILE_RECT_WIDTH-(focus_y&1)*TILE_HALF_RECT_WIDTH,height/2-focus_y*TILE_RECT_HEIGHT);
}

void Game::setViewAtMinimapPoint(int x,int y) {
	if(x>=0 && y>=0 && x<mapimage.width && y<mapimage.height) {
		int xp,yp,xm,ym;
		xp = (10*mapimage.width)/(2*map.width);
		yp = (10*(mapimage.height+xp))/map.height;
debug_output("xp=%d,yp=%d\n",xp,yp);
		xm = (10*x)/xp;
		ym = ((((10*y)+(!(xm&1))*xp/2)/(2*yp))*2)+(xm&1);
		xm /= 2;
debug_output("xm=%d,ym=%d\n",xm,ym);
		setViewAtLocation(xm,ym);
	}
}

void Game::setView(int x,int y) {
	view_x = x,view_y = y;
	if(isHWrap()) {
		if(view_x>-TILE_CORNER_WIDTH)
			while(view_x>-TILE_CORNER_WIDTH) view_x -= map.width_p;
		else if(view_x<=-map.width_p)
			while(view_x<=-map.width_p) view_x += map.width_p;
	} else if(map.width_p>width+TILE_CORNER_WIDTH) {
		if(view_x<=width-map.width_p) view_x = width-map.width_p;
		if(view_x>-TILE_CORNER_WIDTH) view_x = -TILE_CORNER_WIDTH;
	} else {
		view_x = (width-(map.width_p+TILE_CORNER_WIDTH))/2;
	}
	if(isVWrap()) {
		if(view_y>-TILE_RECT_HEIGHT)
			while(view_y>-TILE_RECT_HEIGHT) view_y -= map.height_p;
		else if(view_y<=-map.height_p)
			while(view_y<=-map.height_p) view_y += map.height_p;
	} else if(map.height_p>height+TILE_RECT_HEIGHT) { // (map.height_p+TILE_RECT_HEIGHT)>(height+TILE_RECT_HEIGHT*2)
		if(view_y<=height-map.height_p) view_y = height-map.height_p;
		if(view_y>-TILE_RECT_HEIGHT) view_y = -TILE_RECT_HEIGHT;
	} else {
		view_y = (height-(map.height_p+TILE_RECT_HEIGHT))/2;
//debug_output("Game::setView(x=%d,y=%d,height=%d,height_p=%d,TILE_RECT_HEIGHT=%d,view_y=%d)\n",x,y,height,map.height_p,TILE_RECT_HEIGHT,view_y);
	}
//debug_output("Game::setView(x=%d,y=%d,style=%d,view_x=%d,view_y=%d)\n",x,y,map.style,view_x,view_y);
}

void Game::focusLocation(int x,int y) {
	if(x<0 || y<0 || x>=map.width || y>=map.height) focus_x = -1,focus_y = -1,focus_unit = 0;
	else {
		int i = y*map.width+x;
		if(map.tiles[i].city || map.tiles[i].unit) {
			focus_x = x,focus_y = y;
			if(map.tiles[i].unit) focus_unit = map.tiles[i].unit;
			else focus_unit = 0;
		} else focus_x = -1,focus_y = -1,focus_unit = 0;
	}
debug_output("focusLocation(x=%d,y=%d,focus_x=%d,focus_y=%d)\n",x,y,focus_x,focus_y);
}

void Game::firstUnit(Player *pl) {
	if(!pl) pl = local.player;
debug_output("Game::firstUnit(local.player=%p,pl=%p)\n",local.player,pl);
	int i,n = map.size;
	tile *m = 0;
	City *c = 0;
	Unit *u = 0;
	for(i=0; i<n; i++) {
		m = &map.tiles[i];
		if(m->unit && m->unit->player==pl && (!u || (m->unit->moves>0 && m->unit->type>u->type))) u = m->unit;
		else if(m->city && m->city->player==pl && (!c || m->city->gold>c->gold)) c = m->city;
	}
	if(u) focusLocation(u->x,u->y);
	else if(c) focusLocation(c->x,c->y);
	else focusLocation(-1,-1);
	setViewAtFocus();
}

void Game::previousUnit(Player *pl) {
	if(!pl) pl = local.player;
	if(focus_x==-1 || focus_y==-1) firstUnit(pl);
	else {
		int i,n = focus_y*map.width+focus_x;
		tile *m;
		for(i=n-1; i!=n; i--) {
			if(i==-1) i = map.size-1;
			m = &map.tiles[i];
			if(!m->unit || m->unit->player!=pl) continue;
			if(m->unit) {
				focusLocation(m->unit->x,m->unit->y);
				break;
			}
		}
		setViewAtFocus();
	}
}

void Game::nextUnit(Player *pl) {
	if(!pl) pl = local.player;
	if(focus_x==-1 || focus_y==-1) firstUnit(pl);
	else {
		int i,n = focus_y*map.width+focus_x,o = map.size;
		tile *m;
		for(i=n+1; i!=n; i++) {
			if(i==o) i = 0;
			m = &map.tiles[i];
			if(!m->unit || m->unit->player!=pl) continue;
			if(m->unit) {
				focusLocation(m->unit->x,m->unit->y);
				break;
			}
		}
		setViewAtFocus();
	}
}

bool Game::setCityProduction(int x,int y,int pr) {
	if(x>=0 && x<map.width && y>=0 && y<map.height) {
		City *c = map.tiles[y*map.width+x].city;
		if(c) return c->setProduction(pr);
	}
	return false;
}

int Game::getWorkTurns(int i,int n,int m) {
	int t = (2*(m-n))/i;
	if(t&1) t++;
	return t/2;
}

void Game::waitPlayerTurn() {
	messageBox(0,_("Wait for your turn to move please."));
}

void Game::beginTurn(int a) {
	if(a>=0) {
		alliance = a;
		getMovingUnit();
		if(local.player && local.player->alliance==alliance) {
			firstUnit();
			messageBox(0,_("It is now your turn to move."));
		} else if(server.isRunning()) {
			Player *pl;
			aHashtable::iterator iter;
			for(iter=players.iterate(); (pl=(Player *)iter.next());)
				if(!(pl->status&PL_END_TURN) && (pl->status&PL_AI) && pl->alliance==alliance)
					doAI(pl);
		}
	} else if(server.isRunning()) {
debug_output("Game::beginTurn(1,players=%d)\n",(int)players.size());
		Player *pl,*pl1 = 0;
		aHashtable::iterator iter;
		for(iter=players.iterate(); (pl=(Player *)iter.next());) {
debug_output("Game::beginTurn(%s)\n",pl->nick);
			if((pl->status&PL_PENDING)) {
debug_output("Game::beginTurn(PL_PENDING)\n");
				int i,n;
				City *c;
				type_t t;
				uint8_t data[SOCKET_HEADER+9],*p = data;
				for(i=0,n=0; i<ALLIANCES; ++i) if(alliances[i]<alliances[n]) n = i;
				++alliances[n];
				pl->alliance = n;
				do c = (City *)cities.getByIndex(rnd.uint32(cities.size()),t);while(c->player);
				pack_header(&p,CMD_PLAYER_CLAIM_CITY);
				pack_uint32(&p,pl->id);
				pack_uint32(&p,c->id);
				pack_uint8(&p,pl->alliance);
				server.send(data,sizeof(data));
				pl->status ^= PL_PENDING;
				c->player = pl; // Make sure city's not selected again, though it's set in ACTION_PLAYER_CLAIM_CITY-event.
			}
			if(!(pl->status&PL_END_TURN)) {
				if(((!pl1 && pl->alliance>=alliance) || 
						pl->alliance==alliance || 
							(pl1 && pl->alliance>=alliance && pl->alliance<pl1->alliance))) pl1 = pl;
			}
		}
		if(alliance==-1)
			sendServerCommand(0,CMD_BEGIN_TURN,0);
		else if(pl1 && pl1->alliance>alliance)
			sendServerCommand(0,CMD_BEGIN_TURN,pl1->alliance);
	}
}

void Game::endTurn(Player *pl) {
debug_output("Game::endTurn(1)\n");
	if((status&GAME_ENDED) || pl->alliance!=alliance) return;
	int i;
	aHashtable::iterator iter;
	City *c;
	Unit *u;
	tile *m;

	pl->status |= PL_END_TURN;

debug_output("Game::endTurn(2)\n");
	// Make sure all players have ended their turn.
	for(iter=players.iterate(); (pl=(Player *)iter.next());)
		if(!(pl->status&PL_END_TURN)) {
			beginTurn();
			return;
		}

debug_output("Game::endTurn(3)\n");
	turns++;
	updateEconomy();

debug_output("Game::endTurn(4)\n");
debug_output("Game::endTurn(6)\n");
	for(i=(int)map.size-1,m=map.tiles; i>=0; --i,++m) {

		// Gain movement at new turn:
		if((u=m->unit)) {
debug_output("Gain movement at new turn: %p\n",u);
			u->moves += unit_types[u->type].moves;
			if(u->moves>unit_types[u->type].moves) u->moves = unit_types[u->type].moves;
		}

		// City Production:
		if((c=m->city)) {
			if(!c->player) continue;
			// Cities gain new income:
			c->gold += c->income-c->wages;
			// City produce new unit:
			if(c->build.type>=0) {
				++c->work;
				if(c->build.type&BLD_BUILD_BUILDING) {
					int pr = c->build.type&_BLD_BUILDINGS;
					const city_building &cb = city_buildings[pr];
					if(c->work==cb.work) {
						c->buildings |= 1<<pr;
						c->buildings = c->getTopBuildings();
						c->build.unit_types = c->getCanBuildUnits();
						c->build.type = -1;
						c->work -= cb.work;
					}
				} else {
					const unit_type &ut = unit_types[c->build.type];
					if(c->work==ut.work) {
debug_output("City produce new unit: type %d, work %d/%d\n",c->build.type,c->work,ut.work);
						u = new Unit(*c,++unit_id_index,ut.status,c->x,c->y,c->build.type);
						if(c->player) ++c->player->units;
						units.put(u->id,u);
						m->unit = u; // TODO! Make sure tile is empty, or place unit on near tile, or else stop production.
debug_output("endTurn 5 u=%p,id=%d\n",m->unit,m->unit->id);
						c->build.type = -1;
						c->work -= ut.work;
					}
				}
				if(c->build.type==-1 && c->build.queue>=0) c->setProduction(c->build.queue);
			}
		}
	}

debug_output("Game::endTurn(8)\n");
	updateEconomy();
	updateStatus();
debug_output("Game::endTurn(done,u=%p)\n",getUnit());

	paintMap();

	// Begin new turn.
	for(iter=players.iterate(); (pl=(Player *)iter.next());)
		pl->status ^= PL_END_TURN;
	move_unit = 0;
	alliance = -1;
	beginTurn();
}

void Game::killUnit(Unit *u) {
	if(!u) return;
	units.remove(u->id);
	map.tiles[u->y*map.width+u->x].unit = 0;
	if(focus_unit==u) focus_unit = 0;
	--u->player->units;
	delete u;
}

void Game::killUnit(int x,int y) {
	tile &m = map.tiles[y*map.width+x];
	Unit *u = m.unit;
	if(!u) return;
	m.unit = 0;
	if(focus_unit==u) focus_unit = 0;
	--u->player->units;
	delete u;
}

Unit *Game::getMovingUnit() {
	if(!move_unit || !move_unit->tr || move_unit->moves<=0) {
		aHashtable::iterator i;
		for(i=units.iterate(); (move_unit=(Unit *)i.next());)
			if(move_unit->player->alliance==alliance && move_unit->tr && move_unit->moves>0) break;
	}
	return move_unit;
}

void Game::moveUnits() {
	if(move_unit) moveUnit(move_unit);
}

void Game::moveUnit(int x1,int y1,int x2,int y2) {
	Unit *u = map.tiles[y1*map.width+x1].unit;
	if(!u) return;
	// Delete trails from all selected units, and ensure no trail is deleted twice.
	if(u->tr) delete u->tr;
	Path pt;
	Trail *tr = pt.search(u,x1,y1,x2,y2);
	u->tr = tr;
	getMovingUnit();
}

void Game::moveUnit(Unit *u) {
	if(!u || !u->tr || u->moves<=0) return;
debug_output("	/* Split units with different trails: */\n");
	Player &pl = *u->player;
	Trail &tr = *u->tr;
	Unit *u2 = 0;
	tile *l[7];
	int i1,i2 = u->y*map.width+u->x,j;

debug_output("	/* Move unit: */\n");
	tr.next();
	i1 = i2;
	i2 = tr.getY()*map.width+tr.getX();
	u2 = map.tiles[i2].unit;

debug_output("	/* Check if Unit on next tile and handle: */\n");
	if(u2) {
		if(u2->player==&pl && !(u->status&U_TRANSPORT) && !u->transp && (u2->status&U_TRANSPORT)) {
			u->transp = u2;
			if(u->moves>u2->moves) u->moves = u2->moves;
		} else {
			tr.last();
			if(u2->player!=&pl) doBattle(u,u2);
		}
	}
	if(unit_types[u->type].terrain[map.tiles[i2].terrain].moves==0 &&
			(!u->transp || unit_types[u->transp->type].terrain[map.tiles[i2].terrain].moves==0)) {
		tr.last();
	} else {
		int m1 = 1,m2;
		if(map.tiles[i2].owner && map.tiles[i2].owner!=u->owner) {
			getLocation(tr.getX(),tr.getY(),1,l);
			for(j=0; j<7; j++)
				if(l[j] && l[j]->owner!=u->owner && 
					((l[j]->unit && l[j]->unit->defence()>=u->attack()) ||
						(l[j]->city && l[j]->city->defence()>=u->attack()))) {
debug_output("attack: %d, unit-defence: %d, city-defence: %d\n",u->attack(),l[j]->unit? l[j]->unit->defence() : -1,l[j]->city? l[j]->city->defence() : 0);
							if(u->player==local.player) messageBox(0,"Tile is guarded by equal or higher strength.");
							m1 = 0;
							tr.last();
							break;
						}
			if(m1 && map.tiles[i2].city) {
				if(u->player==local.player) messageBox(0,"City was captured in battle.");
				else if(map.tiles[i2].city->player==local.player) messageBox(0,"City was lost to enemies.");
			}
		}
		if(m1) {
			m1 = unit_types[u->type].terrain[map.tiles[i2].terrain].moves;
			map.tiles[i1].unit = 0;
			if(u->transp) {
				m2 = unit_types[u->transp->type].terrain[map.tiles[i2].terrain].moves;
				u->transp->moves -= m2;
				if(m2==0) {
					map.tiles[i1].unit = u->transp;
					u->transp = 0;
				} else m1 = m2;
			}
			u->moves -= m1;
			u->x = u->tr->getX();
			u->y = u->tr->getY();
			map.tiles[i2].owner = u->owner;
			if(map.tiles[i2].city) map.tiles[i2].city->player = &pl;
debug_output("getLocation(u=%p,x=%d,y=%d)\n",u,u->x,u->y);
			getLocation(u->x,u->y,1,l);
			if(&pl==local.player) for(j=0; j<7; j++) if(l[j]) clearFogOfWar(pl,l[j]->x,l[j]->y);
		}
	}

debug_output("	/* Place units on new maplocation: */\n");
	map.tiles[u->y*map.width+u->x].unit = u;
	focusLocation(u->x,u->y);
	if(!u->tr->hasMoreSteps()) {
debug_output("	/* Delete trail when finished: */\n");
		delete u->tr;
		u->tr = 0;
	}
debug_output("	/* Done. (u[%p],x=%d,y=%d)*/\n",u,u->x,u->y);
}

void Game::doBattle(Unit *u1,Unit *u2) {
	if(u1->moves>0 && u1->attack()>u2->defence()) {
		--u1->moves;
		++u1->xp;
		if(u1->owner && u1->owner->player==local.player) {
			if(map.tiles[u2->y*map.width+u2->x].city) messageBox(0,"Enemy was defeated and City captured!");
			else messageBox(0,"Unit was victorious in battle.");
		} else if(u2->owner && u2->owner->player==local.player) {
			if(map.tiles[u2->y*map.width+u2->x].city) messageBox(0,"Unit and City was lost in battle.");
			else messageBox(0,"Unit was lost in battle.");
		}
		killUnit(u2);
		updateEconomy();
		if(focus_x!=-1 && focus_y!=-1 && !map.tiles[focus_y*map.width+focus_x].unit && !map.tiles[focus_y*map.width+focus_x].city) focusLocation(-1,-1);
		testWinner();
	}
}

bool Game::testWinner() {
	if(status&GAME_ENDED) return false;
	aHashtable::iterator i;
	Player *pl;
	City *c;
	Unit *u;
	for(i=players.iterate(); (pl=(Player *)i.next());) {
		pl->cities = 0;
		pl->units = 0;
	}
	for(i=cities.iterate(); (c=(City *)i.next());)
		if(c->player) c->player->cities++;
	for(i=units.iterate(); (u=(Unit *)i.next());)
		if(u->player) u->player->units++;
	for(i=players.iterate(); (pl=(Player *)i.next());) {
		if(pl->cities==0 && pl->units==0) {
			handleMessage(ACTION_PRINT,MSG_GAME,_("%s has been eliminated."),pl->nick);
			i.remove();
			delete pl;
		}
	}
	if(players.size()<=1) {
		if(players.size()==0) handleMessage(ACTION_PRINT,0xffff00,_("All players are eliminated, noone has won."));
		else {
			i = players.iterate();
			pl = (Player *)i.first();
			handleMessage(ACTION_PRINT,MSG_GAME,_("%s is the winner!"),pl->nick);
		}
		status |= GAME_ENDED;
		return true;
	}
	return false;
}

void Game::updateStatus() {
	handleMessage(ACTION_SHOW_STATUS,0,_("Turn: %d"),turns);
//	handleMessage(ACTION_SHOW_STATUS,1,"Player: %d",pl+1);
}


void Game::updateEconomy() {
debug_output("Game::updateEconomy(1)\n");
	int i;
	tile *m;
	City *c;
	Unit *u;
	aHashtable::iterator iter;
	for(iter=cities.iterate(); (c=(City *)iter.next());) {
		if(!c->player) continue;
		c->income = 0;
		c->wages = 0;
	}
	for(i=map.size-1; i>=0; i--) {
		m = &map.tiles[i];
		if(m->owner) {
			++m->owner->income;
			if(m->city) ++m->owner->income;
		}
	}
debug_output("Game::updateEconomy(3)\n");
	for(iter=units.iterate(); (u=(Unit *)iter.next());)
		u->owner->wages += unit_types[u->type].maintain;
debug_output("Game::updateEconomy(done)\n");
}



