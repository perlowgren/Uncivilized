
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <gdk/gdkkeysyms.h>
#include <amanita/aFile.h>
#include <amanita/aMath.h>
#include <amanita/aString.h>

#include "main.h"
#include "game.h"
#include "path.h"
#include "login.h"
#include "setup.h"
//#include "network.h"

#include "../data/images/icon16.xpm"
#include "../data/images/icon32.xpm"
#include "../data/images/icon48.xpm"
#include "../data/images/icon64.xpm"


const char *Game::addr_127_0_0_1 = "127.0.0.1";
const char *Game::addr_localhost = "localhost";

Game app;


int main(int argc,char *argv[]) {
	int ret;
	app.setProjectName(PACKAGE);
	app.setApplicationName(PACKAGE_STRING);
	app.setUserAgent(PACKAGE_STRING);
	app.setDirectories(
		LOCALEDIR
	);
	app.init(argc,argv,
		AMANITA_INIT_GETTEXT |
		AMANITA_INIT_GUI |
		AMANITA_INIT_THREADS |
		AMANITA_INIT_SOCKETS);
	ret = app.open(900,600,600,400,true);
	app.close();
	return ret;
}


Game::Game() : aApplication(),server(server_listener),client(client_listener),clients(),players(),cities(),units() {
	int i;
	maparea_id = 0;
	server.setStatus(SERVER_ST_INTERNAL_CLIENT_ID,true);
	message_box.text = 0;
	message_box.timer = 0;
	local.port = PORT;
	local.player = 0;
	log.latest = 0;
	log.index = 0;
	log.top = 0;
	log.rows = 0;
	log.max_rows = 16;
	log.layout = 0;
	log.update = false;
	width = 0;
	height = 0;
	mouse_drag.dragging = 0;
	mouse_over.area = -1;
	mouse_over.object = 0;
	mouse_over.value = -1;
	mouse_over.tooltip = -1;
	for(i=0; i<MAX_AI_PLAYERS; i++) setup.ai[i] = 0;
	mapimage.width = 0;
	mapimage.height = 0;
	mapimage.image = 0;
	city_box.x = 0;
	city_box.y = 0;
	city_box.width = 0;
	city_box.height = 0;
	city_box.prod_rows = 0;
	unit_box.x = 0;
	unit_box.y = 0;
	unit_box.width = 0;
	unit_box.height = 0;
	unit_box.height = 0;

	status = 0;
	map.width = 0;
	map.height = 0;
	map.width_p = 0;
	map.height_p = 0;
	map.tiles = 0;
	map.fow = 0;
	map.fow_n = 0;
	focus_x = -1;
	focus_y = -1;
	focus_unit = 0;
	move_unit = 0;
	running = false;
}

Game::~Game() {
}


uint32_t BrowserApp::eventHandler(aWidget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	app.handleAction(st);
	return 0;
}

gboolean Game::maparea_configure_event_callback(GtkWidget *widget,GdkEventConfigure *event) {
//	gdk_threads_enter();
//	app.lock();
	app.width = widget->allocation.width;
	app.height = widget->allocation.height;
	app.openPaint(widget);
	app.updateDisplay();
	app.setView(app.view_x,app.view_y);
	app.paint();
//	gdk_threads_leave();
//	app.unlock();
	return TRUE;
}

gboolean Game::maparea_expose_event_callback(GtkWidget *widget,GdkEventExpose *event) {
//	gdk_threads_enter();
//	app.lock();
	app.updatePaint(widget,event);
//	gdk_threads_leave();
//	app.unlock();
	return TRUE;
}

gboolean Game::maparea_timeout_callback(gpointer data) {
	static int t = 0;
//fprintf(stderr,"Game::maparea_expose_event_callback(lock)\n");
//fflush(stderr);
	gdk_threads_enter();
//	app.lock();
	if(((++t)%3)==0) app.moveUnits();
	app.paint();
//fprintf(stderr,"Game::maparea_expose_event_callback(unlock)\n");
//fflush(stderr);
//	app.unlock();
	gdk_window_invalidate_rect(app.gui.maparea->window,0,false);
	gdk_window_process_updates(app.gui.maparea->window,false);
	gdk_threads_leave();
	return TRUE;
}

void menuitem_callback(int code) {
debug_output("menuitem_callback(%d)\n",code);
//	gdk_threads_enter();
//	app.lock();
	app.handleAction(code);
//	gdk_threads_leave();
//	app.unlock();
}

gboolean statusbar_push(GtkWidget *widget,GdkEventCrossing *ev,gpointer data) {
	app.handleAction(ACTION_SHOW_STATUS,0,(intptr_t)g_object_get_data(G_OBJECT(widget),"StatusbarText"));
	return FALSE;
}

gboolean statusbar_pop(GtkWidget *widget,GdkEventCrossing *ev,gpointer data) {
	app.handleAction(ACTION_SHOW_STATUS,0);
	return FALSE;
}

void Game::chat_entry_callback(GtkWidget *widget,gpointer data) {
	const char *msg = gtk_entry_get_text(GTK_ENTRY(app.gui.chatentry));
debug_output("chat_entry_callback(msg=%s)\n",msg);
	app.sendClientMessage(MSG_CHAT,"%s",msg);
	gtk_entry_set_text(GTK_ENTRY(app.gui.chatentry),"");
}

gboolean Game::mouse_move_event_callback(GtkWidget *widget,GdkEventMotion *event,gpointer data) {
	int x = aMath::round(event->x),y = aMath::round(event->y);
//	gdk_threads_enter();
//	app.lock();
	if(app.mouse.pressed) {
		app.mouse.dragged = true;
		app.mouseMove(x,y,&app.mouse.drag,event);
		app.mouse.drag.x = x;
		app.mouse.drag.y = y;
	} else {
		app.mouseMove(x,y,0,event);
	}
//	gdk_threads_leave();
//	app.unlock();
	return TRUE;
}

gboolean Game::mouse_press_event_callback(GtkWidget *widget,GdkEventButton *event,gpointer data) {
debug_output("Game::mouse_press_event_callback(button=%d,type=%d)\n",event->button,event->type);
	int x = aMath::round(event->x),y = aMath::round(event->y);
//	gdk_threads_enter();
	if(event->button==1) {
		app.mouse.pressed = true;
		app.mouse.dragged = false;
		app.mouse.drag.x = x;
		app.mouse.drag.y = y;
	}
//	app.lock();
	app.mouseDown(x,y,event);
//	gdk_threads_leave();
//	app.unlock();
	return TRUE;
}

gboolean Game::mouse_release_event_callback(GtkWidget *widget,GdkEventButton *event,gpointer data) {
debug_output("Game::mouse_release_event_callback(button=%d,type=%d)\n",event->button,event->type);
	int x = aMath::round(event->x),y = aMath::round(event->y);
//	gdk_threads_enter();
//	app.lock();
	app.mouseUp(x,y,event);
//	app.unlock();
	app.mouse.pressed = false;
	app.mouse.dragged = false;
//	gdk_threads_leave();
	return TRUE;
}


enum {
	KEY_CONTROL_MASK				= GDK_CONTROL_MASK,
	KEY_ALT_MASK					= GDK_MOD1_MASK,
	KEY_ENTER						= GDK_Return,
	KEY_SPACE						= GDK_space,
	KEY_a								= GDK_a,
	KEY_b								= GDK_b,
	KEY_c								= GDK_c,
	KEY_d								= GDK_d,
	KEY_e								= GDK_e,
	KEY_f								= GDK_f,
	KEY_g								= GDK_g,
	KEY_h								= GDK_h,
	KEY_i								= GDK_i,
	KEY_j								= GDK_j,
	KEY_k								= GDK_k,
	KEY_l								= GDK_l,
	KEY_m								= GDK_m,
	KEY_n								= GDK_n,
	KEY_o								= GDK_o,
	KEY_p								= GDK_p,
	KEY_q								= GDK_q,
	KEY_r								= GDK_r,
	KEY_s								= GDK_s,
	KEY_t								= GDK_t,
	KEY_u								= GDK_u,
	KEY_v								= GDK_v,
	KEY_w								= GDK_w,
	KEY_x								= GDK_x,
	KEY_y								= GDK_y,
	KEY_z								= GDK_z,
	KEY_F1							= GDK_F1,
	KEY_F2							= GDK_F2,
	KEY_F3							= GDK_F3,
	KEY_F4							= GDK_F4,
	KEY_F5							= GDK_F5,
	KEY_F6							= GDK_F6,
	KEY_F7							= GDK_F7,
	KEY_F8							= GDK_F8,
	KEY_F9							= GDK_F9,
	KEY_F10							= GDK_F10,
	KEY_F11							= GDK_F11,
	KEY_F12							= GDK_F12,
};


int Game::create() {
	int i,n;

	gui.window = (GtkWidget *)getWindow();
debug_output("load images\n");
	{
//		int w,h;
		const char **xpm1[] = { icon16_xpm,icon32_xpm,icon48_xpm,icon64_xpm };
//		GdkColormap *colormap = gtk_widget_get_colormap(gui.window);
//		GdkPixmap *pixmap;

		GdkPixbuf *pixbuf[4];
debug_output("load images 1\n");
		for(i=0,n=0; i<4; i++) {
			//pixmap = gdk_pixmap_colormap_create_from_xpm_d(0,colormap,0,0,(gchar **)xpm1[i]);
			//gdk_pixbuf_get_from_drawable(0,pixmap,0,0,0,0,0,-1,-1);
			pixbuf[n++] = gdk_pixbuf_new_from_xpm_data(xpm1[i]);
			//g_object_ref_sink(pixmap);
		}
debug_output("load images 2\n");
		GList *iconlist = 0;
		for(i=0; i<4; i++) {
			iconlist = g_list_append(iconlist,pixbuf[i]);
		}
debug_output("load images 3\n");
		gtk_window_set_default_icon_list(iconlist);
		gtk_window_set_icon_list(GTK_WINDOW(gui.window),iconlist);

		loadGraphics();

debug_output("load cursors\n");
		cursors[0] = gdk_cursor_new(GDK_ARROW);
		cursors[1] = gdk_cursor_new(GDK_HAND2);
	}

debug_output("make accelerator\n");
//	GtkAccelGroup *accel_group = gtk_accel_group_new();
//	gtk_window_add_accel_group(GTK_WINDOW(gui.window),accel_group);

	GtkWidget *vbox = gtk_vbox_new(FALSE,0);

debug_output("make statusbar\n");
	gui.statusbar = gtk_statusbar_new();
	gtk_box_pack_end(GTK_BOX(vbox),gui.statusbar,FALSE,FALSE,0);

debug_output("make menu\n");
//	gui.menubar = gtk_menu_bar_new();
	{
		enum {
			MENU_FILE,
			MENU_NEW,
			MENU_SAVE,
			MENU_LOAD,
			MENU_CONNECT,
			MENU_QUIT,
			MENU_GAME,
			MENU_SETUP,
			MENU_END_TURN,
			MENU_PREVIOUS,
			MENU_NEXT,
			MENU_VIEW,
			MENU_SHOW_MAP,
			MENU_HELP,
			MENU_CONTENT,
			MENU_ABOUT,
		};
		const menu_item menu_items[] = {
			{ -1,MENU_FILE,_("File"),0,0,true,-1 },
				{ MENU_FILE,MENU_NEW,_("New"),0,ACTION_NEW,true,KEY_n,KEY_CONTROL_MASK },
				{ MENU_FILE,MENU_SAVE,_("Save"),0,ACTION_SAVE,true,KEY_s,KEY_CONTROL_MASK },
				{ MENU_FILE,MENU_LOAD,_("Load"),0,ACTION_LOAD,true,KEY_l,KEY_CONTROL_MASK },
				{ MENU_FILE,-1 },
				{ MENU_FILE,MENU_CONNECT,_("Connect"),0,ACTION_CONNECT,true,-1 },
				{ MENU_FILE,-1 },
				{ MENU_FILE,MENU_QUIT,_("Quit"),0,ACTION_QUIT,true,KEY_x,KEY_ALT_MASK },
			{ -1,MENU_GAME,_("Game"),0,true,-1 },
				{ MENU_GAME,MENU_SETUP,_("Setup"),0,ACTION_SETUP,true,KEY_F2,0 },
				{ MENU_GAME,-1 },
				{ MENU_GAME,MENU_END_TURN,_("End Turn"),0,ACTION_END_TURN,true,KEY_ENTER,KEY_CONTROL_MASK },
				{ MENU_GAME,-1 },
				{ MENU_GAME,MENU_PREVIOUS,_("Previous"),0,ACTION_PREV,true,KEY_F3,0 },
				{ MENU_GAME,MENU_NEXT,_("Next"),0,ACTION_NEXT,true,KEY_F4,0 },
			{ -1,MENU_VIEW,_("View"),0,0,true,-1 },
				{ MENU_VIEW,MENU_SHOW_MAP,_("Show Map"),0,ACTION_SHOW_MAP,true,KEY_m,KEY_CONTROL_MASK },
			{ -1,MENU_HELP,_("Help"),0,0,true,-1 },
				{ MENU_HELP,MENU_CONTENT,_("Content"),0,ACTION_CONTENT,true,KEY_F1,0 },
				{ MENU_HELP,-1 },
				{ MENU_HELP,MENU_ABOUT,_("About"),0,ACTION_ABOUT,true,-1 },
			{ -1,-1 }
		};

		gui.menu = new Menu(eventHandler,gui.window,menu_items);
/*
		char str[256];
		int nitems = 0;
		for(const menu_item *m1=menu_items; m1->pid!=-1 || m1->id!=-1; ++nitems,++m1);
debug_output("nitems=%d\n",nitems);
		items = (menu_item *)malloc(nitems*sizeof(menu_item));
		memcpy(items,menu_items,nitems*sizeof(menu_item));
		menu_item *m,*mp;
		for(i=0; i<nitems; ++i) {
			m = &items[i];
			m->index = i;
			m->lvl = 0;
			if(m->id==-1) m->name = "---",m->status = 0;
			else if(!m->name) m->name = "?";
			if(m->pid!=-1) {
				for(n=0; n<i; n++) if(items[n].id==m->pid) {
					m->parent = &items[n];
					if(!items[n].child) items[n].child = m;
					break;
				}
			}
		}

		for(i=0; i<nitems; ++i) {
			m = &items[i];
			if(m->parent) for(mp=m->parent,m->lvl=0; mp; mp=mp->parent) ++m->lvl;
		}

		for(i=0,m=items; m && i<nitems; ++i) {
*str = '\0';
for(n=0; n<m->lvl; ++n) strcat(str,"--> ");
strcat(str,m->name);
debug_output("init(menu: index=%d,id='%d',parent='%d',child='%d',menuitem='%s')\n",m->index,m->id,m->parent? m->parent->id : -1,m->child? m->child->id : -1,str);

			if(m->id==-1) {
				if(m->parent) {
					m->item = gtk_separator_menu_item_new();
					gtk_menu_shell_append(GTK_MENU_SHELL(m->parent->submenu),m->item);
				}
			} else {
				m->item = gtk_image_menu_item_new_with_label(m->name);
				menu.put(m->action,m);
				if(!m->sensitive) gtk_widget_set_sensitive(m->item,false);
				if(m->acc!=-1) {
					gtk_image_menu_item_set_accel_group(GTK_IMAGE_MENU_ITEM(m->item),accel_group);
					gtk_widget_add_accelerator(m->item,"activate",accel_group,m->acc,(GdkModifierType)m->acc_mod,GTK_ACCEL_VISIBLE);
				}
				if(m->status) {
					g_object_set_data(G_OBJECT(m->item),"StatusbarText",(gpointer)m->status);
					g_signal_connect(m->item,"enter-notify-event",G_CALLBACK(statusbar_push),(gpointer)gui.statusbar);
					g_signal_connect(m->item,"leave-notify-event",G_CALLBACK(statusbar_pop),(gpointer)gui.statusbar);
				}
				if(m->child) {
					m->submenu = gtk_menu_new();
					gtk_menu_item_set_submenu(GTK_MENU_ITEM(m->item),m->submenu);
				}
				if(m->parent) {
					gtk_menu_shell_append(GTK_MENU_SHELL(m->parent->submenu),m->item);
					if(m->action) gtk_signal_connect_object(GTK_OBJECT(m->item),"activate",GTK_SIGNAL_FUNC(menuitem_callback),(gpointer)((intptr_t)m->action));
				} else {
					gtk_menu_shell_append(GTK_MENU_SHELL(gui.menubar),m->item);
				}
			}

			if(i==nitems-1) break;
			if(m->child) m = m->child;
			else if(items[m->index+1].parent==m->parent || m->parent) m = &items[m->index+1];
//			else if(m->parent) {
//debug_output("init(menu: next <--)\n");
//				for(n=m->index+1,m=m->parent; m && items[n].parent!=m->parent; m=m->parent);
//				if(m) {
//					if(!m->parent && items[m->index+1].parent) break;
//					m = &items[m->index+1];
//				}
//			}
			else break;
		}
*/
	}
//	gtk_box_pack_start(GTK_BOX(vbox),gui.menubar,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget *)gui.menu->getHandle(),FALSE,FALSE,0);

	gui.maparea = gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(gui.maparea),"expose_event",G_CALLBACK(maparea_expose_event_callback),0);
	g_signal_connect(G_OBJECT(gui.maparea),"configure_event",G_CALLBACK(maparea_configure_event_callback),0);
	g_signal_connect(G_OBJECT(gui.maparea),"motion_notify_event",G_CALLBACK(mouse_move_event_callback),0);
	g_signal_connect(G_OBJECT(gui.maparea),"button_press_event",G_CALLBACK(mouse_press_event_callback),0);
	g_signal_connect(G_OBJECT(gui.maparea),"button_release_event",G_CALLBACK(mouse_release_event_callback),0);
	gtk_widget_set_events(gui.maparea,GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK);
	gtk_box_pack_start(GTK_BOX(vbox),gui.maparea,TRUE,TRUE,0);

	gui.chatentry = gtk_entry_new();
	gtk_box_pack_end(GTK_BOX(vbox),gui.chatentry,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(gui.chatentry),"activate",G_CALLBACK(chat_entry_callback),this);

	gtk_container_add(GTK_CONTAINER(gui.window),vbox);

	gtk_widget_show_all(gui.window);

	startUpdating();
	new login_dialog(GTK_WINDOW(gui.window));
	return 1;
}

void Game::exit() {
debug_output("Exit. 1\n");
	stopUpdating();
debug_output("Exit. 2\n");
	stopServer();
debug_output("Exit. 3\n");
	client.stop();
debug_output("Exit. 4\n");
debug_output("Exit. 5\n");
   if(message_box.text) free(message_box.text);
	message_box.text = 0;
	for(message *m=log.latest,*m1; m; m=m1) {
		m1 = m->next;
		free(m->text);
		free(m);
	}
debug_output("Exit. 6\n");
	closeGame();
	if(log.layout) g_object_unref(G_OBJECT(log.layout));
debug_output("Exit. 7\n");
	closePaint();
	if(mapimage.image) g_object_unref(mapimage.image);
	gdk_cursor_unref(cursors[0]);
	gdk_cursor_unref(cursors[1]);
	if(setup.dialog) delete setup.dialog;
	free(items);

debug_output("Exit. done\n");
	gtk_main_quit();
//	fclose(out);
}

void Game::messageBox(int icon,const char *message, ...) {
	int len;
	char msg[257];
	va_list args;
   va_start(args,message);
	vsnprintf(msg,256,message,args);
	msg[256] = '\0';
   va_end(args);
   if(message_box.text) free(message_box.text);
   message_box.text = strdup(msg);
   len = strlen(message_box.text);
   message_box.width = len*6+32;
   message_box.height = 48;
   message_box.x = (width-message_box.width)/2;
   message_box.y = (height-message_box.height)/2;
   message_box.timer = 10;
}

void Game::messageDialog(int code,intptr_t p1,intptr_t p2,GtkMessageType type,GtkButtonsType buttons,const char *title,const char *message, ...) {
	GtkWidget *msgbox;
	char msg[257];
	va_list args;
	intptr_t p3;
   va_start(args,message);
	vsnprintf(msg,256,message,args);
	msg[256] = '\0';
   va_end(args);
	msgbox = gtk_message_dialog_new(GTK_WINDOW(app.gui.window),
			GTK_DIALOG_MODAL,type,buttons,"%s",msg);
	gtk_window_set_title(GTK_WINDOW(msgbox),title);
	p3 = (intptr_t)gtk_dialog_run(GTK_DIALOG(msgbox));
	gtk_widget_destroy(msgbox);
	if(code>=0) app.handleAction(code,p1,p2,p3);
}

int Game::handleMessage(int code,int p1,const char *format, ...) {
	char str[257];
	va_list args;
   va_start(args,format);
	vsnprintf(str,256,format,args);
	str[256] = '\0';
   va_end(args);
	return handleAction(code,p1,(intptr_t)str,0);
}


int Game::handleAction(int code,intptr_t p1,intptr_t p2,intptr_t p3) {
debug_output("Game::handleAction(code=%d,p1=%ld,p2=%ld,p3=%ld)\n",code,(long)p1,(long)p2,(long)p3);
	switch(code) {
		case ACTION_NEW:break;
		case ACTION_SAVE:
		{
/*
debug_output("Game::handleAction(save 1)\n");
			uint8_t *data;
			int len;
debug_output("Game::handleAction(save 2)\n");
			packGame(0,&data,len);
			aFile f;
debug_output("Game::handleAction(save 3)\n");
			f.open("wb","%ssave.dat",getHomeDir());
debug_output("Game::handleAction(save 4)\n");
			f.write((const char *)data,len);
debug_output("Game::handleAction(save 5)\n");
			free(data);
*/
			break;
		}
		case ACTION_LOAD:
		{
/*
			char *data;
			size_t len;
			aFile f;
			f.open("rb","%ssave.dat",getHomeDir());
			f.read(&data,len);
			if(len>0) {
				unpackGame((uint8_t *)data);
				free(data);
				sendGame();
			}
*/
			break;
		}
		case ACTION_CONNECT:break;
		case ACTION_QUIT:
			exit();
			break;
		case ACTION_SETUP:
			new setup_dialog(GTK_WINDOW(gui.window));
			break;
		case ACTION_END_TURN:
			if(local.player && local.player->alliance==alliance)
				sendClientCommand(CMD_END_TURN,local.player->id);
			break;
		case ACTION_PLAYER_CLAIM_CITY:
		{
			Player *pl = (Player *)clients.get(p1);
			City *c = (City *)cities.get(CITY_ID_TO_COORD(p2));
debug_output("message(ACTION_PLAYER_CLAIM_CITY %p %p)\n",pl,c);
			if(pl && c) {
				int n;
				tile *l[7];
				pl->alliance = p3;
				c->player = pl;
				c->gold = 10;
				getLocation(c->x,c->y,1,l);
				for(n=0; n<7; n++) if(l[n])
					clearFogOfWar(*pl,l[n]->x,l[n]->y);
				if(pl==local.player) {
					focusLocation(c->x,c->y);
					setViewAtFocus();
				}
debug_output("message(ACTION_PLAYER_CLAIM_CITY %s x=%d,y=%d,alliance=%d)\n",pl->nick,c->x,c->y,pl->alliance);
			}
			break;
		}
		case ACTION_PREV:previousUnit();break;
		case ACTION_NEXT:nextUnit();break;
		case ACTION_SHOW_MAP:setShowMap(!showMap());break;
		case ACTION_CONTENT:break;
		case ACTION_ABOUT:break;

		case ACTION_SHOW_STATUS:
			if(p2) gtk_statusbar_push(GTK_STATUSBAR(gui.statusbar),p1,(gchar *)p2);
			else gtk_statusbar_pop(GTK_STATUSBAR(gui.statusbar),p1);
			break;
		case ACTION_PRINT:
		{
debug_output("message(client=%p,type=%x,\"%s\")\n",(void *)p3,(int)p1,(const char *)p2);
			message *m = (message *)malloc(sizeof(message));
			Player *pl = (Player *)(p3? clients.get((int)p3) : 0);
			if(log.latest) log.latest->prev = m;
			m->id = pl? pl->id : 0;
			if(pl) strcpy(m->nick,pl->nick);
			else *m->nick = '\0';
			m->type = p1;//((p1>>16)&0xff)|(p1&0xff00)|((p1&0xff)<<16);
			m->status = 0;
			m->x = 0;
			m->y = 0;
			m->text = strdup((char *)p2);
			m->text_len = strlen(m->text);
			m->time = time(0);
			m->next = log.latest;
			m->prev = 0;
			if(log.index==log.latest) log.index = m;
			log.latest = m;
			log.update = true;
			break;
		}
		case ACTION_LOGIN:
		{
			strncpy(host,(const char *)p2,64);
			host[64] = '\0';
			if(!*host) strcpy(host,addr_127_0_0_1);
			else if(!strcmp(host,addr_localhost)) strcpy(host,addr_127_0_0_1);
			if(!strcmp(host,addr_127_0_0_1)) {
				startServer(p1);
			} else if(server.isRunning()) stopServer();
debug_output("client.start()\n");
			if(client.isRunning()) client.stop();
			client.start(host,p1,0,local.nick);
debug_output("started\n");

			new setup_dialog(GTK_WINDOW(gui.window));
			break;
		}
		case ACTION_DISBAND_UNIT:
			if(p3==GTK_RESPONSE_OK) {
				Unit *u = getUnit();
				uint8_t data[SOCKET_HEADER+4],*p = data;
				pack_header(&p,CMD_DISBAND_UNIT);
				pack_uint32(&p,u->id);
				client.send(data,sizeof(data));
			}
			break;
	}
	return 1;
}


void Game::startUpdating() {
	maparea_id = g_timeout_add(125,maparea_timeout_callback,this);
}

void Game::stopUpdating() {
	if(maparea_id) g_source_remove(maparea_id);
}

void Game::startServer(int port) {
	if(server.isRunning()) return;
debug_output("server.start()\n");
	server.start(port);
}

void Game::stopServer() {
	if(!server.isRunning()) return;
	server.stop();
	clearPlayers();
}

void Game::updateDisplay() {
	mapimage.x = width-mapimage.width-9;
	mapimage.y = height-mapimage.height-9;
}

void Game::updateMessageLog() {
	static const int type_colors[] = { 0x00ffff,0xffff00,0xff0000,0xffffff,0xff00ff };
	if(!log.index) return;
//debug_output("Game::updateMessageLog(1)\n");
	aString str(512*log.rows);
	int i,y,t1 = time(0)-300;
	tm *t2;
	char s[512],*p;
	message *m = log.index,*list[log.max_rows];
	for(log.rows=0; log.rows<log.max_rows && m && m->time>t1; log.rows++,m=m->next) {
		list[log.rows] = m;
		log.top = m;
	}
//debug_output("Game::updateMessageLog(2)\n");
	str.append("<span font_desc=\"ProFont 8\">");
	for(i=log.rows-1,y=0; i>=0; i--,y+=11) {
		m = list[i];
		t2 = localtime(&m->time);
		sprintf(s,"<span foreground=\"#%06x\">[%d:%d] ",type_colors[m->type],t2->tm_hour,t2->tm_min);
		str.append(s);
		if(*m->nick) {
			p = g_markup_escape_text(m->nick,-1);
			str.append(p).append("> ");
			free(p);
		}
		p = g_markup_escape_text(m->text,-1);
		str.append(p).append("</span>");
		if(i>0) str.append('\n');
		free(p);
	}
	str.append("</span>");
debug_output("Game::updateMessageLog(str=\n%s\n)\n",str.toCharArray());
	if(log.layout) g_object_unref(G_OBJECT(log.layout));
	log.layout = pango_layout_new(gtk_widget_get_pango_context(gui.maparea));
	pango_layout_set_markup(log.layout,str.toCharArray(),str.length());
//debug_output("Game::updateMessageLog(4,str=%s)\n",str.toCharArray());
	if(log.rows==0) log.top = 0,log.width = 0,log.height = 0;
	else pango_layout_get_pixel_size(log.layout,&log.width,&log.height);
	log.update = false;
}



int Game::mouseDown(int x,int y,GdkEventButton *event) {
debug_output("Game::mouseDown(x=%d,y=%d)\n",x,y);
	int i,xp = x,yp = y,xm,ym;
	mouse_over.area = -1;
	if(showMap() && xp>=mapimage.x-4 && yp>=mapimage.y-4 && xp<mapimage.x+mapimage.width+4 && yp<mapimage.y+mapimage.height+4) {
		setViewAtMinimapPoint(xp-mapimage.x,yp-mapimage.y);
		mouse_drag.dragging = DRAG_MAP;
	} else if(city_box.height && xp>=city_box.x && yp>=city_box.y && xp<city_box.x+city_box.width && yp<city_box.y+city_box.height) {
		xp -= city_box.x;
		yp -= city_box.y;
		City *c = getCity();
		if(isLocalPlayerCity(c)) {
			if(xp>=city_box.prod_x && yp>=city_box.prod_y && xp<city_box.prod_x+CITY_BOX_WIDTH && yp<city_box.prod_y+city_box.prod_rows*UNIT_ICON_HEIGHT) {
				xp -= city_box.prod_x;
				yp -= city_box.prod_y;
				int j,x,y;
				uint64_t b = c->getCanBuildBuildings();
				const city_building *cb;
				for(i=-1,j=0,x=0,y=0; j<BLD_BUILDINGS; j++) if(b&(1<<j)) {
					cb = &city_buildings[j];
					if(x+UNIT_ICON_WIDTH>city_box.x+city_box.prod_x+CITY_BOX_WIDTH) x = 0,y += BUILDING_ICON_HEIGHT+1;
					if(xp>=x && yp>=y && xp<x+BUILDING_ICON_WIDTH && yp<y+BUILDING_ICON_HEIGHT) { i = j|BLD_BUILD_BUILDING;break; }
					x += BUILDING_ICON_WIDTH+1;
				}
				if(i==-1) for(j=0; j<UNIT_TYPES; j++) if(c->build.unit_types&(1<<j)) {
					if(x+UNIT_ICON_WIDTH>CITY_BOX_WIDTH) x = 0,y += UNIT_ICON_HEIGHT+1;
					if(xp>=x && yp>=y && xp<x+UNIT_ICON_WIDTH && yp<y+UNIT_ICON_HEIGHT) { i = j;break; }
					x += UNIT_ICON_WIDTH+1;
				}
				if(i!=-1 && (i&BLD_BUILD_BUILDING? c->canBuild(i) : c->canProduce(i))) {
					if(isPlayerTurn()) {
						uint8_t data[SOCKET_HEADER+12],*p = data;
						pack_header(&p,CMD_CITY_PRODUCTION);
						pack_int32(&p,(int32_t)focus_x);
						pack_int32(&p,(int32_t)focus_y);
						pack_int32(&p,(int32_t)i);
						client.send(data,sizeof(data));
					} else waitPlayerTurn();
				}
			}
		}
	} else if(unit_box.height && xp>=unit_box.x && yp>=unit_box.y && xp<unit_box.x+unit_box.width && yp<unit_box.y+unit_box.height) {
		xp -= unit_box.x;
		yp -= unit_box.y;
		Unit *u = getUnit();
		if(xp>=unit_box.unit_x && yp>=unit_box.unit_y && xp<unit_box.unit_x+UNIT_BOX_WIDTH && yp<unit_box.unit_y+unit_box.unit_rows*UNIT_SPRITE_HEIGHT) {
			if(u) {
				if(!isUnitFocusLocalPlayer()) mouse_drag.dragging = DRAG_UNIT_FL;
				else if(event->button==1) {
					mouse_drag.dragging = DRAG_UNIT_FL;
				} else mouse_drag.dragging = DRAG_UNIT_FR;
			}
		} else if(xp>=unit_box.action_x && yp>=unit_box.action_y && xp<unit_box.action_x+UNIT_BOX_WIDTH && yp<unit_box.action_y+WORK_HEIGHT) {
			if(xp>=unit_box.action_x+UNIT_BOX_WIDTH-WORK_WIDTH) {
				if(u && isLocalPlayerUnit(u)) {
					if(isPlayerTurn()) messageDialog(ACTION_DISBAND_UNIT,0,0,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK_CANCEL,_("Disband Unit"),_("Are you sure you want to disband this unit?"));
					else waitPlayerTurn();
				}
			} else if(focus_unit) {
				const unit_type *ut = &unit_types[focus_unit->type];
				if(ut->actions&U_WORK) {
					tile &t = map.tiles[focus_unit->y*map.width+focus_unit->x];
					const terrain_action *ta;
					int terrain = 1<<t.terrain;
					xp -= unit_box.action_x;
					for(i=0; (ta=&terrain_actions[i])->action!=-1; i++)
						if((terrain&ta->terrain) && (ut->actions&ta->action) && (!ta->owned || (t.owner && t.owner->player==local.player))) {
							if(xp<WORK_WIDTH) break;
							xp -= WORK_WIDTH+1;
						}
					if(ta && ta->action!=-1) {
						/*mouse_over.area = AREA_UB_ACTION;
						mouse_over.value = ta->icon;
						mouse_over.tooltip = TOOLTIP_UNIT_ACTION;*/
					}
				}
			}
		} else u = 0;
		if(!u) u = getUnit();
		focus_unit = u;
	} else {
		pointToMap(xp,yp,xm,ym);
debug_output("xp=%d,yp=%d,xm=%d,ym=%d\n",xp,yp,xm,ym);
		if(xm>=0 && ym>=0 && xm<map.width && ym<map.height) {
			if(event->button==1) {
				if(isFogOfWar(*local.player,xm,ym)) xm = -1,ym = -1;
				focusLocation(xm,ym);
tile &t = map.tiles[ym*map.width+xm];
debug_output("owner: %s, alliance=%d, city->owner: %s, unit->owner: %s\n",t.owner && t.owner->player? t.owner->player->nick : "none",t.owner && t.owner->player? t.owner->player->alliance : -1,
	t.city && t.city->player? t.city->player->nick : "none",t.unit && t.unit->owner && t.unit->owner->player? t.unit->owner->player->nick : "none");
				mouse_drag.x = xp;
				mouse_drag.y = yp;
				mouse_drag.dragging = DRAG_VIEW;
				getView(mouse_drag.view_x,mouse_drag.view_y);
			} else if(event->button==3) {
				if(isUnitFocusLocalPlayer()) {
					Unit *u = getUnit();
					if(u) {
						if(isPlayerTurn())  {
							uint8_t data[SOCKET_HEADER+16],*p = data;
							pack_header(&p,CMD_MOVE_UNIT);
							pack_int32(&p,focus_x);
							pack_int32(&p,focus_y);
							pack_int32(&p,xm);
							pack_int32(&p,ym);
debug_output("move_packet: x1=%d,y1=%d,x2=%d,y2=%d\n",focus_x,focus_y,xm,ym);
							client.send(data,sizeof(data));
						} else waitPlayerTurn();
					}
				}
			}
		}
	}
	return 0;
}

int Game::mouseUp(int x,int y,GdkEventButton *event) {
debug_output("Game::mouseUp(x=%d,y=%d)\n",x,y);
	mouse_drag.dragging = 0;
	return 0;
}

int Game::mouseMove(int x,int y,GdkPoint *drag,GdkEventMotion *event) {
//	if(!mouse_drag.dragging) return 0;
//debug_output("Game::mouseMove(x=%d,y=%d)\n",x,y);
	mouse.x = x;
	mouse.y = y;
	mouse_over.area = -1;
	mouse_over.object = 0;
	mouse_over.value = -1;
	mouse_over.tooltip = 0;

	int i,xp = x,yp = y;
	if(mouse_drag.dragging==DRAG_VIEW) setView(mouse_drag.view_x+xp-mouse_drag.x,mouse_drag.view_y+yp-mouse_drag.y);
	else if(mouse_drag.dragging==DRAG_MAP && showMap()) setViewAtMinimapPoint(xp-mapimage.x,yp-mapimage.y);
	else if(city_box.height && xp>=city_box.x && yp>=city_box.y && xp<city_box.x+city_box.width && yp<city_box.y+city_box.height) {
		xp -= city_box.x;
		yp -= city_box.y;
		City *c = getCity();
		if(xp>=city_box.prod_x && yp>=city_box.prod_y && xp<city_box.prod_x+CITY_BOX_WIDTH && yp<city_box.prod_y+city_box.prod_rows*UNIT_ICON_HEIGHT) {
			xp -= city_box.prod_x;
			yp -= city_box.prod_y;
			int j,x,y;
			uint64_t b = c->getCanBuildBuildings();
			const city_building *cb;
			for(i=-1,j=0,x=0,y=0; j<BLD_BUILDINGS; j++) if(b&(1<<j)) {
				cb = &city_buildings[j];
				if(x+UNIT_ICON_WIDTH>city_box.x+city_box.prod_x+CITY_BOX_WIDTH) x = 0,y += BUILDING_ICON_HEIGHT+1;
				if(xp>=x && yp>=y && xp<x+BUILDING_ICON_WIDTH && yp<y+BUILDING_ICON_HEIGHT) { i = j|BLD_BUILD_BUILDING;break; }
				x += BUILDING_ICON_WIDTH+1;
			}
			if(i==-1) for(j=0; j<UNIT_TYPES; j++) if(c->build.unit_types&(1<<j)) {
				if(x+UNIT_ICON_WIDTH>CITY_BOX_WIDTH) x = 0,y += UNIT_ICON_HEIGHT+1;
				if(xp>=x && yp>=y && xp<x+UNIT_ICON_WIDTH && yp<y+UNIT_ICON_HEIGHT) { i = j;break; }
				x += UNIT_ICON_WIDTH+1;
			}
//				for(j=0,n=-1; i>0 && j<UNIT_TYPES; j++) if(c->build.types&(1<<j)) i--,n++;
			if(i!=-1) {
				if(i&BLD_BUILD_BUILDING) {
					if(c->canBuild(i)) mouse_over.area = AREA_CB_BLDPROD;
					mouse_over.value = i&_BLD_BUILDINGS;
					mouse_over.tooltip = TOOLTIP_BUILDING_BUILD;
				} else {
					if(c->canProduce(i)) mouse_over.area = AREA_CB_UNITPROD;
					mouse_over.value = i;
					mouse_over.tooltip = TOOLTIP_UNIT_BUILD;
				}
			}
		}
	} else if(unit_box.height && xp>=unit_box.x && yp>=unit_box.y && xp<unit_box.x+unit_box.width && yp<unit_box.y+unit_box.height) {
		xp -= unit_box.x;
		yp -= unit_box.y;
		Unit *u = getUnit();
		if(xp>=unit_box.unit_x && yp>=unit_box.unit_y && xp<unit_box.unit_x+UNIT_BOX_WIDTH && yp<unit_box.unit_y+unit_box.unit_rows*UNIT_SPRITE_HEIGHT) {
			if(u) {
				mouse_over.area = AREA_UB_UNIT;
				mouse_over.object = (intptr_t)u;
				mouse_over.value = u->type;
				mouse_over.tooltip = TOOLTIP_UNIT;
			}
		} else if(xp>=unit_box.action_x && yp>=unit_box.action_y && xp<unit_box.action_x+UNIT_BOX_WIDTH && yp<unit_box.action_y+WORK_HEIGHT) {
			if(xp>=unit_box.action_x+UNIT_BOX_WIDTH-WORK_WIDTH) {
				mouse_over.area = AREA_UB_ACTION;
				mouse_over.value = ICON_CANCEL;
				mouse_over.tooltip = TOOLTIP_UNIT_ACTION;
			} else if(focus_unit) {
				const unit_type *ut = &unit_types[focus_unit->type];
				if(ut->actions&U_WORK) {
					tile &t = map.tiles[focus_unit->y*map.width+focus_unit->x];
					const terrain_action *ta;
					int terrain = 1<<t.terrain;
					xp -= unit_box.action_x;
					for(i=0; (ta=&terrain_actions[i])->action!=-1; i++)
						if((terrain&ta->terrain) && (ut->actions&ta->action) && (!ta->owned || (t.owner && t.owner->player==local.player))) {
							if(xp<WORK_WIDTH) break;
							xp -= WORK_WIDTH+1;
						}
					if(ta && ta->action!=-1) {
						mouse_over.area = AREA_UB_ACTION;
						mouse_over.value = ta->icon;
						mouse_over.tooltip = TOOLTIP_UNIT_ACTION;
					}
				}
			}
		}
	}
	i = mouse_over.area!=-1? 1 : 0;
	if(i!=mouse_over.cursor) {
		mouse_over.cursor = i;
		gdk_window_set_cursor(gui.maparea->window,cursors[mouse_over.cursor]);
	}
	return 0;
}



