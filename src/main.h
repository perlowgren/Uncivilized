#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <signal.h>
#include <amanita/aApplication.h>
#include <amanita/aHashtable.h>
#include <amanita/net/aClient.h>
#include <amanita/net/aServer.h>

#include "_config.h"
#include "game.h"
#include "paint.h"
#include "setup.h"

#define PORT 7403

enum {
	ACTION_NEW						= 1,
	ACTION_SAVE,
	ACTION_LOAD,
	ACTION_CONNECT,
	ACTION_QUIT,
	ACTION_SETUP,
	ACTION_END_TURN,
	ACTION_PLAYER_CLAIM_CITY,
	ACTION_PREV,
	ACTION_NEXT,
	ACTION_SHOW_MAP,
	ACTION_CONTENT,
	ACTION_ABOUT,

	ACTION_SHOW_STATUS,
	ACTION_PRINT,
	ACTION_LOGIN,
	ACTION_LOGOUT,

	ACTION_DISBAND_UNIT,
};


enum {
	CMD_UNSPECIFIED,
	CMD_TEXT_MESSAGE,
	CMD_WELCOME,
	CMD_REQUEST_CLIENTS,
	CMD_SEND_CLIENTS,
	CMD_KILL_CLIENT,
	CMD_ACTIVATE_CLIENT,
	CMD_REQUEST_GAME,
	CMD_SEND_GAME,
	CMD_START_GAME,
	CMD_STOP_GAME,
	CMD_SHOW_FOW,
	CMD_BEGIN_TURN,
	CMD_END_TURN,
	CMD_PLAYER_CLAIM_CITY,
	CMD_CITY_PRODUCTION,
	CMD_CITY_RESOURCE,
	CMD_MOVE_UNIT,
	CMD_DISBAND_UNIT,
};

enum {
	MSG_SERVER,
	MSG_INFO,
	MSG_WARNING,
	MSG_CHAT,
	MSG_GAME,
};


enum {
	DRAG_UNIT = 1,
	DRAG_VIEW,
	DRAG_MAP,
	DRAG_UNIT_FL,
	DRAG_UNIT_FR,
};


struct Unit;

class Game : public aApplication {
friend class Path;
friend class Player;
friend class Unit;
friend class setup_dialog;

private:
	struct menu_item {
		int pid;
		int id;
		const char *name;
		const char *status;
		int action;
		bool sensitive;
		int acc;
		int acc_mod;
		int index;
		int lvl;
		menu_item *parent;
		menu_item *child;
		GtkWidget *item;
		GtkWidget *submenu;
	};

	struct message {
		int id;				//!< ID of client who sent message. (0 for server.)
		char nick[13];		//!< Nick of the client who sent message.
		int type;			//!< Type of message.
		int status;			//!< Status of message (i.e. visible).
		int x;				//!< X coordinate of message on screen.
		int y;				//!< Y coordinate of message on screen.
		char *text;			//!< Message text.
		int text_len;		//!< Length in bytes of message text.
		time_t time;		//!< Time message was sent.
		message *next;		//!< Next message in log.
		message *prev;		//!< Previous message in log.
	};

	struct {
		int x;
		int y;
		int width;
		int height;
		int icon;
		char *text;
		int timer;
	} message_box;

	static const char *addr_127_0_0_1;
	static const char *addr_localhost;

	static const char *cmd_names[];

	uint32_t unit_id_index;

	FILE *out;
	aServer server;
	aClient client;
	struct {
		int port;
		char address[65];
		uint32_t id;
		char nick[33];
		Player *player;
	} local;
	char host[65];

	int width;
	int height;
	menu_item *items;
	aHashtable menu;
	struct {
		aMenu *menu;
		GtkWidget *window;
		GtkWidget *maparea;
		GtkWidget *chatentry;
		GtkWidget *statusbar;
//		GtkWidget *menubar;
		GtkWidget *unit_menu;
	} gui;
	GdkCursor *cursors[2];
	guint maparea_id;

	struct {
		int x;
		int y;
		bool pressed;
		bool dragged;
		GdkPoint drag;
	} mouse;
	struct {
		int x;
		int y;
		int dragging;
		int view_x;
		int view_y;
	} mouse_drag;
	struct {
		int cursor;
		int area;
		intptr_t object;
		int value;
		int tooltip;
	} mouse_over;
	struct {
		int ai[MAX_AI_PLAYERS];
		setup_dialog *dialog;
	} setup;
	struct {
		int x;
		int y;
		int width;
		int height;
		GdkPixmap *image;
	} mapimage;
	struct {
		int x;
		int y;
		int width;
		int height;
		int res_x;
		int res_y;
		int prod_x;
		int prod_y;
		int prod_rows;
	} city_box;
	struct {
		int x;
		int y;
		int width;
		int height;
		int unit_x;
		int unit_y;
		int unit_rows;
		int action_x;
		int action_y;
	} unit_box;
	struct {
		message *latest;			//!< Latest message in the log.
		message *index;			//!< Points to the bottom message in the message log view.
		message *top;				//!< Points to the uppermost message in the view.
		int rows;					//<! Numbers of visible rows in the message log.
		int max_rows;
		PangoLayout *layout;
		int width;
		int height;
		bool update;
	} log;

	aHashtable clients;			//!< Contains all players indexed by ID.
	aHashtable players;			//!< Contains all players indexed by Nick.

	struct {
		uint16_t width;			//!< Width of map in hex-tiles.
		uint16_t height;			//!< Height of map in hex-tiles.
		uint32_t size;				//!< Number of tiles all together, width*height.
		int32_t width_p;			//!< Width of map in points.
		int32_t height_p;			//!< Height of map in points.
		tile *tiles;				//!< Tiles in the map.
		uint8_t *fow;				//!< Fog of war, a bitmap where each bit represent a tile on the map for each alliance.
		uint32_t fow_n;			//!< Number of 8 bit integers to represent all players, it's 1+((number of alliances)-1)/8.
	} map;
	uint32_t status;
	uint32_t turns;
	int alliances[ALLIANCES];
	int alliance;
	int16_t focus_x;
	int16_t focus_y;
	Unit *focus_unit;
	Unit *move_unit;
	int16_t view_x;
	int16_t view_y;

	aHashtable cities;			//!< Contains all city-objects, indexed by city's map-location-index which is y*mapwidth+x.
	aHashtable units;				//!< Contains all unit-objects, indexed by Unit-ID which is generated with a counter.

	bool running;

	static uint32_t eventHandler(aWidget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);

	static gboolean maparea_configure_event_callback(GtkWidget *widget,GdkEventConfigure *event);
	static gboolean maparea_expose_event_callback(GtkWidget *widget,GdkEventExpose *event);
	static gboolean maparea_timeout_callback(gpointer data);
	static gboolean mouse_move_event_callback(GtkWidget *widget,GdkEventMotion *event,gpointer data);
	static gboolean mouse_press_event_callback(GtkWidget *widget,GdkEventButton *event,gpointer data);
	static gboolean mouse_release_event_callback(GtkWidget *widget,GdkEventButton *event,gpointer data);

	static void chat_entry_callback(GtkWidget *widget,gpointer data);

	static uint32_t server_listener(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);
	uint32_t handle_server_event(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);
	static uint32_t client_listener(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);
	uint32_t handle_client_event(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);

	void startUpdating();
	void stopUpdating();

	void startServer(int port);
	void stopServer();

//	int getBGRColor(int c) { return 0xff000000|((c<<24)&0xff0000)|(c&0xff00)|((c>>24)&0xff); }

	void updateDisplay();
	void updateMessageLog();

	void clearPlayers();
	void clearMap();
	void clearBattle();
//	void area(int *m,int *a[7],int x,int y);
	void erode(int *m,int s=0);

public:
	Game();
	~Game();

	int create();
	int destroy() { return handleAction(ACTION_QUIT); }

	void exit();
	void messageBox(int icon,const char *message, ...);
	void messageDialog(int code,intptr_t p1,intptr_t p2,GtkMessageType type,GtkButtonsType buttons,const char *title,const char *message, ...);
	int handleMessage(int code,int p1,const char *format, ...);
	int handleAction(int code,intptr_t p1=0,intptr_t p2=0,intptr_t p3=0);

	int mouseMove(int x,int y,GdkPoint *drag,GdkEventMotion *event);
	int mouseDown(int x,int y,GdkEventButton *event);
	int mouseUp(int x,int y,GdkEventButton *event);

	void setStatus(int n) { status = n; }
	void setStatus(int n,bool s) { status = s? (status|n) : (status&~n); }
	bool isGameEnded() { return status&GAME_ENDED; }
	bool isHWrap() { return status&GAME_MAP_HWRAP; }
	bool isVWrap() { return status&GAME_MAP_VWRAP; }
	void setShowMap(bool s) { status = s? (status|GAME_SHOW_MAP) : (status&~GAME_SHOW_MAP); }
	bool showMap() { return status&GAME_SHOW_MAP; }
	void setShowFogOfWar(bool s) { status = s? (status|GAME_SHOW_FOW) : (status&~GAME_SHOW_FOW); }
	bool showFogOfWar() { return status&GAME_SHOW_FOW; }

	void setNick(const char *n) { strncpy(local.nick,n,32);local.nick[32] = '\0'; }
	const char *getNick() { return local.nick; }

	void sendServerCommand(aConnection c,int cmd,int v=0);
	void sendClientCommand(int cmd,int v=0);
	void sendServerData(aConnection c,int cmd,const uint8_t *data,int len);
	void sendClientData(int cmd,const uint8_t *data,int len);
	void sendServerMessage(aConnection c,int type,const char *format, ...);
	void sendClientMessage(int type,const char *format, ...);
	void sendGame();
	void sendGame(aConnection c);
	void sendClient(aConnection c);
	void sendClients(aConnection c);
	int receiveClients(uint8_t *data);
	void setNick(aConnection c,const char *nick);

	void newGame(int w,int h,int s);
	void closeGame();
	void packGame(int cmd,uint8_t **data,int &len);
	void unpackGame(uint8_t *data);
	void saveGame(const char *fn);
	void loadGame(const char *fn);
	void startGame() { running = true; }
	void stopGame() { running = false; }
	bool isGameRunning() { return running; }
	void createMap(int w,int h);
	int mapWidth() { return map.width; }
	int mapHeight() { return map.height; }
	void pointToMap(int xp,int yp,int &xm,int &ym);
	tile **getLocation(int x,int y,int r,tile **l=0);
	void setViewAtLocation(int x,int y);
	void setViewAtFocus();
	void setViewAtMinimapPoint(int x,int y);
	void setView(int x,int y);
	void getView(int &x,int &y) { x = view_x,y = view_y; }
	void clearFogOfWar(Player &p,int x,int y) { map.fow[(y*map.width+x)*map.fow_n+(p.index>>4)] &= ~(1<<(p.index&7)); }
	void setFogOfWar(Player &p,int x,int y) { map.fow[(y*map.width+x)*map.fow_n+(p.index>>4)] |= (1<<(p.index&7)); }
	bool isFogOfWar(Player &p,int x,int y) { return showFogOfWar() && (map.fow[(y*map.width+x)*map.fow_n+(p.index>>4)]&(1<<(p.index&7)))>0; }
	void focusLocation(int x,int y);
	int focusX() { return focus_x; }
	int focusY() { return focus_y; }
	Unit *focusUnit() { return focus_unit; }
	void firstUnit(Player *p=0);
	void previousUnit(Player *p=0);
	void nextUnit(Player *p=0);
	bool hasCityFocus() { return focus_x!=-1 && focus_y!=-1 && map.tiles[focus_y*map.width+focus_x].city; }
	bool hasUnitFocus() { return focus_x!=-1 && focus_y!=-1 && map.tiles[focus_y*map.width+focus_x].unit; }
	bool setCityProduction(int x,int y,int pr);
	int getWorkTurns(int i,int n,int m);
	City *getCity() { return focus_x>=0 && focus_y>=0? map.tiles[focus_y*map.width+focus_x].city : 0; }
	City *getCity(int x,int y) { return map.tiles[y*map.width+x].city; }
	bool isLocalPlayerCity(City *c) { return c->player==local.player; }

	Player *getLocalPlayer() { return local.player; }
	bool isUnitFocusLocalPlayer() { return focus_x!=-1 && focus_y!=-1 && map.tiles[focus_y*map.width+focus_x].unit && map.tiles[focus_y*map.width+focus_x].unit->player==local.player; }
	bool isLocalPlayerUnit(Unit *u) { return u && u->player==local.player; }
	bool isPlayerTurn() { return alliance==local.player->alliance; }
	void waitPlayerTurn();
	/** Begins a new turn, for each alliance. If a==-1 and is server, it will search for an alliance with players
	 * that has not moved and send message to all, else it will set new alliance and give a message to players of this alliance. */
	void beginTurn(int a=-1);
	void endTurn(Player *p);
	Unit *getUnit() { return focus_x>=0 && focus_y>=0? map.tiles[focus_y*map.width+focus_x].unit : 0; }
	Unit *getUnit(int x,int y) { return map.tiles[y*map.width+x].unit; }
	void killUnit(Unit *u);
	void killUnit(int x,int y);
	Unit *getMovingUnit();
	void moveUnits();
	void moveUnit(int x1,int y1,int x2,int y2);
	void moveUnit(Unit *u);
	void doBattle(Unit *u1,Unit *u2);
	bool testWinner();

	void updateStatus();
	void updateEconomy();

	void doAI(Player *pl);

	void loadGraphics();
	void openPaint(GtkWidget *widget);
	void updatePaint(GtkWidget *widget,GdkEventExpose *event);
	void closePaint();
	void printText(int x,int y,const char *text, ...);
	void printText(int x,int y,int w,PangoAlignment a,const char *text, ...);
	void paint();
	void paintTile(tile &t,int x,int y,int s);
	void paintUnit(Unit &u,int x,int y,int s);
	void paintFlag(int x,int y,int c,int n);
	void paintBox(int x,int y,int w,int h,bool fill=true);
	void paintHorizontalLine(int x,int y,int w);
	void paintVerticalLine(int x,int y,int h);
	void paintCityBox();
	void paintUnitBox();
	void paintToolTip();
	int paintEconomy(int x,int y,int c1,int w,City *c);
	int paintCost(int x,int y,int c1,int c2,int w,int cost,int maintain);
	void paintMap();
	void paintMessageBox();
};

extern Game app;


#endif /* _MAIN_H */


