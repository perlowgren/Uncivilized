
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>

#include "main.h"
#include "setup.h"

#include "../data/images/ai0.xpm"
#include "../data/images/ai1.xpm"
#include "../data/images/ai2.xpm"
#include "../data/images/ai3.xpm"


enum {
	PLAYERLIST_NAME,
	PLAYERLIST_COLS,
};

#define MAP_DIMENSIONS_DEFAULT_INDEX 1

static const struct {
	int width;
	int height;
} map_dimensions[] = {
	{ 24,12 },
	{ 32,16 },
	{ 48,24 },
	{ 64,32 },
	{ 80,40 },
	{ 128,64 },
	{ 256,128 },
{0,0}};

static const char **xpm[] = { ai0_xpm,ai1_xpm,ai2_xpm,ai3_xpm, };



gboolean setup_dialog::delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	setup_dialog *dlg = app.setup.dialog;
	delete dlg;
	return TRUE;
}

void setup_dialog::player_status_toggled_callback(GtkWidget *widget,gpointer data) {
	setup_dialog *dlg = app.setup.dialog;
	int n = (int)((intptr_t)data);
	app.setup.ai[n] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dlg->gui.ai_check[n]))? 1 : 0;
	dlg->setAIStatus(n,app.setup.ai[n]);
}

void setup_dialog::player_status_clicked_callback(GtkWidget *widget,gpointer data) {
	setup_dialog *dlg = app.setup.dialog;
	int n = (int)((intptr_t)data);
	++app.setup.ai[n];
	dlg->setAIStatus(n,app.setup.ai[n]);
}

void setup_dialog::ok_clicked_callback(GtkWidget *widget,gpointer data) {
	setup_dialog *dlg = app.setup.dialog;
	if(app.server.isRunning() && !app.isGameRunning()) {
		int n,s;
		n = gtk_combo_box_get_active(GTK_COMBO_BOX(dlg->gui.map));
		s = 0;//GAME_MAP_HWRAP|GAME_SHOW_MAP/*|GAME_SHOW_FOW*/;
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dlg->gui.hwrap))) s |= GAME_MAP_HWRAP;
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dlg->gui.vwrap))) s |= GAME_MAP_VWRAP;
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dlg->gui.fow))) s |= GAME_SHOW_FOW;
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dlg->gui.newcomers))) s |= GAME_INVITE_NEWCOMERS;
		app.closeGame();
		app.newGame(map_dimensions[n].width/2,map_dimensions[n].height*2,s);
		app.sendGame();
		app.beginTurn();
	}
	delete dlg;
}

void setup_dialog::cancel_clicked_callback(GtkWidget *widget,gpointer data) {
	setup_dialog *dlg = app.setup.dialog;
	delete dlg;
}


setup_dialog::setup_dialog(GtkWindow *parent) {
	int i;
	char str[256];
	GtkWidget *vbox;
	GtkWidget *vbox2;
	GtkWidget *hbox;
	GtkWidget *hbox2;
	GtkWidget *hbox3;
	GtkWidget *frame;
	GtkWidget *scroll;
	GtkWidget *ok;
	GtkWidget *cancel;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkListStore *store;

debug_output("setup_dialog::setup_dialog()\n");
	gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(gui.window),_("Setup the Uncivilized Game"));
	gtk_window_set_transient_for(GTK_WINDOW(gui.window),parent);
	gtk_window_set_modal(GTK_WINDOW(gui.window),TRUE);
	gtk_window_set_resizable(GTK_WINDOW(gui.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(gui.window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(gui.window),5);

	vbox = gtk_vbox_new(FALSE,2);
	hbox = gtk_hbox_new(FALSE,2);
	{
		frame = gtk_frame_new(_("Settings"));
		vbox2 = gtk_vbox_new(FALSE,2);
		gtk_container_set_border_width(GTK_CONTAINER(vbox2),5);
		gui.map = gtk_combo_box_new_text();
		gtk_widget_set_size_request(gui.map,120,-1);
		for(i=0; map_dimensions[i].width>0; i++) {
			sprintf(str,"%d x %d",map_dimensions[i].width,map_dimensions[i].height);
			gtk_combo_box_append_text(GTK_COMBO_BOX(gui.map),(gchar *)str);
		}
		gtk_combo_box_set_active(GTK_COMBO_BOX(gui.map),MAP_DIMENSIONS_DEFAULT_INDEX);
		gtk_box_pack_start(GTK_BOX(vbox2),gui.map,FALSE,FALSE,0);

		const char *checkbox_labels[] = {
			_("Horizontal wrap"),
			_("Vertical wrap"),
			_("Fog of War"),
			_("Invite Newcomers"),
		};
		GtkWidget **checkboxes[] = {
			&gui.hwrap,
			&gui.vwrap,
			&gui.fow,
			&gui.newcomers,
		};
		for(i=0; i<4; i++) {
			*checkboxes[i] = gtk_check_button_new_with_label(checkbox_labels[i]);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(*checkboxes[i]),TRUE);
			gtk_box_pack_start(GTK_BOX(vbox2),*checkboxes[i],FALSE,FALSE,0);
		}

		gtk_container_add(GTK_CONTAINER(frame),vbox2);
		gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,FALSE,0);
	}
	{
		frame = gtk_frame_new(_("AI Players"));
		hbox2 = gtk_hbox_new(FALSE,2);
		gtk_container_set_border_width(GTK_CONTAINER(hbox2),5);
		{
			vbox2 = gtk_vbox_new(FALSE,0);
			for(i=0; i<MAX_AI_PLAYERS; i++) {
				hbox3 = gtk_hbox_new(FALSE,0);
				sprintf(str,_("AI Player %d"),i+1);
				gui.ai_check[i] = gtk_check_button_new_with_label(str);
				g_signal_connect(G_OBJECT(gui.ai_check[i]),"toggled",G_CALLBACK(player_status_toggled_callback),(gpointer)((intptr_t)i));
				gtk_box_pack_start(GTK_BOX(hbox3),gui.ai_check[i],TRUE,TRUE,2);

				gui.ai_button[i] = gtk_button_new();
				setAIStatus(i,app.setup.ai[i]);
				g_signal_connect(G_OBJECT(gui.ai_button[i]),"clicked",G_CALLBACK(player_status_clicked_callback),(gpointer)((intptr_t)i));
				gtk_box_pack_start(GTK_BOX(hbox3),gui.ai_button[i],FALSE,FALSE,2);
				gtk_box_pack_start(GTK_BOX(vbox2),hbox3,FALSE,FALSE,0);
			}
			gtk_box_pack_start(GTK_BOX(hbox2),vbox2,TRUE,FALSE,0);
		}
		{
			scroll = gtk_scrolled_window_new(NULL,NULL);
			gtk_widget_set_size_request(scroll,120,200);
			gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);

			gui.playerlist = gtk_tree_view_new();
			renderer = gtk_cell_renderer_text_new();
			column = gtk_tree_view_column_new_with_attributes(_("Players"),renderer,"text",PLAYERLIST_NAME,NULL);
			gtk_tree_view_append_column(GTK_TREE_VIEW(gui.playerlist),column);
			store = gtk_list_store_new(PLAYERLIST_COLS,G_TYPE_STRING);
			gtk_tree_view_set_model(GTK_TREE_VIEW(gui.playerlist),GTK_TREE_MODEL(store));
			g_object_unref(store);
			gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),gui.playerlist);

			gtk_box_pack_start(GTK_BOX(hbox2),scroll,TRUE,TRUE,5);
		}
		gtk_container_add(GTK_CONTAINER(frame),hbox2);
		gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,FALSE,0);
	}
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,FALSE,0);

	hbox = gtk_hbox_new(FALSE,2);
	ok = gtk_button_new_with_label(_("OK"));
	gtk_widget_set_size_request(ok,80,-1);
	g_signal_connect(G_OBJECT(ok),"clicked",G_CALLBACK(ok_clicked_callback),this);
	gtk_box_pack_start(GTK_BOX(hbox),ok,TRUE,FALSE,0);

	cancel = gtk_button_new_with_label(_("Cancel"));
	gtk_widget_set_size_request(cancel,80,-1);
	g_signal_connect(G_OBJECT(cancel),"clicked",G_CALLBACK(cancel_clicked_callback),this);
	gtk_box_pack_start(GTK_BOX(hbox),cancel,TRUE,FALSE,0);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

	gtk_container_add(GTK_CONTAINER(gui.window),vbox);
	g_signal_connect(G_OBJECT(gui.window),"delete_event",G_CALLBACK(delete_event_callback),this);

	if(!app.server.isRunning() || app.isGameRunning()) {
		gtk_widget_set_sensitive(gui.map,false);
		gtk_widget_set_sensitive(gui.hwrap,false);
		gtk_widget_set_sensitive(gui.vwrap,false);
		gtk_widget_set_sensitive(gui.fow,false);
		gtk_widget_set_sensitive(gui.newcomers,false);
		for(i=0; i<MAX_AI_PLAYERS; i++) {
			gtk_widget_set_sensitive(gui.ai_check[i],false);
			gtk_widget_set_sensitive(gui.ai_button[i],false);
		}
		gtk_widget_set_sensitive(ok,false);
		gtk_widget_set_sensitive(cancel,false);
	}
	gtk_widget_show_all(gui.window);

	app.setup.dialog = this;
	updatePlayers();
debug_output("setup_dialog::setup_dialog(done)\n");
}

setup_dialog::~setup_dialog() {
	gtk_widget_destroy(gui.window);
	app.setup.dialog = 0;
}

void setup_dialog::setAIStatus(int n,int &s) {
	if(n<0 || n>=MAX_AI_PLAYERS) return;
	if(s<0) s = 3;
	else if(s>3) s = 0;
	GdkColormap *colormap = gtk_widget_get_colormap(gui.window);
	GdkPixmap *tmpmap;
	GtkWidget *pixmap;
	GdkBitmap *mask;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui.ai_check[n]),s==0? FALSE : TRUE);
	tmpmap = gdk_pixmap_colormap_create_from_xpm_d(0,colormap,&mask,NULL,(gchar **)xpm[s]);
	pixmap = gtk_pixmap_new(tmpmap,mask);
	g_object_unref(tmpmap);
	g_object_unref(mask);
	gtk_button_set_image(GTK_BUTTON(gui.ai_button[n]),pixmap);
}

void setup_dialog::updatePlayers() {
debug_output("setup_dialog::updatePlayers(%d)\n",(int)app.players.size());
	Player *pl;
	GtkListStore *store;
	GtkTreeIter iter;
	store = gtk_list_store_new(PLAYERLIST_COLS+1,G_TYPE_STRING,G_TYPE_POINTER);
	for(aHashtable::iterator i=app.players.iterate(); (pl=(Player *)i.next());) {
		gtk_list_store_append(store,&iter);
		gtk_list_store_set(store,&iter,
			PLAYERLIST_NAME,pl->nick,
			PLAYERLIST_COLS,(gpointer)pl,-1);
debug_output("setup_dialog::updatePlayers(%s)\n",pl->nick);
	}
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui.playerlist),GTK_TREE_MODEL(store));
	g_object_unref(store);
}

/*
LRESULT WINAPI SetupDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	static const char *setup_plcb[] = { "Off","Human","Computer - Easy","Computer - Normal","Computer - Hard" };
	static int player_focus = 0;
	switch(msg) {
		case WM_COMMAND:
		{
			int n = LOWORD(wParam);
			if(n>=IDC_SETUP_PLRB && n<IDC_SETUP_PLRB+6) {
				if(HIWORD(wParam)==BN_CLICKED) {
					WaitForSingleObject(mutex,INFINITE);
					player_focus = n-IDC_SETUP_PLRB;
					SendMessage(setupplcb,CB_SETCURSEL,(WPARAM)pl[player_focus],0);
					connection **c1 = clients.table[1];
					if(pl[player_focus]==1) {
						for(n=0; n<clients.cap; n++) if(c1[n] && c1[n]->pl==player_focus) break;
						if(n==clients.cap) for(n=0; n<clients.cap; n++) if(c1[n] && c1[n]->pl==-1) break;
					} else n = -1;
					if(n>=0 && n<clients.cap) {
						c1[n]->pl = player_focus;
						SendMessage(setuplb,LB_SELECTSTRING,0,(LPARAM)c1[n]->nick);
					} else SendMessage(setuplb,LB_SETCURSEL,(WPARAM)-1,0);
					SendMessage(setupb[player_focus],BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)playerbmp[pl[player_focus]]);
					ReleaseMutex(mutex);
				}
			} else if(n>=IDC_SETUP_PLB && n<IDC_SETUP_PLB+6) {
				if(HIWORD(wParam)==BN_CLICKED) {
					WaitForSingleObject(mutex,INFINITE);
					player_focus = n-IDC_SETUP_PLB,pl[player_focus] = (pl[player_focus]+1)%5;
					SendMessage(setuprb[player_focus],BM_CLICK,0,0);
					ReleaseMutex(mutex);
				}
			} else switch(n) {
				case IDC_SETUP_PLCB:
					if(HIWORD(wParam)==CBN_SELENDOK) {
						WaitForSingleObject(mutex,INFINITE);
						pl[player_focus] = SendMessage(setupplcb,CB_GETCURSEL,0,0);
						SendMessage(setuprb[player_focus],BM_CLICK,0,0);
						ReleaseMutex(mutex);
					}
					break;
				case IDC_SETUP_PLLB:
					if(HIWORD(wParam)==LBN_SELCHANGE) {
						WaitForSingleObject(mutex,INFINITE);
						connection *c;
						for(n=0; n<clients.cap; n++) if((c=clients.table[1][n]) && c->pl==player_focus) c->pl = -1;
						n = SendMessage(setuplb,LB_GETCURSEL,0,0);
						c = (connection *)SendMessage(setuplb,LB_GETITEMDATA,(WPARAM)n,0);
						if(pl[player_focus]!=1) {
							pl[player_focus] = 1;
						}
						if(c->pl>=0) {
							SendMessage(setupb[c->pl],BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)playerbmp[0]);
							pl[c->pl] = 0;
						}
						c->pl = player_focus;
						SendMessage(setuprb[player_focus],BM_CLICK,0,0);
						ReleaseMutex(mutex);
					}
					break;
				case IDOK:
					if(HIWORD(wParam)==BN_CLICKED) {
						WaitForSingleObject(mutex,INFINITE);
						connection *c;
						for(n=0; n<6; n++) pl_clients[n] = 0;
						for(n=0; n<clients.cap; n++) if((c=clients.table[1][n]) && c->pl>=0) pl_clients[c->pl] = c;
						n = SendMessage(setupgcb,CB_GETCURSEL,0,0);
						game.newGame(pl,map_sizes[n].w,map_sizes[n].h);
						n = SendMessage(setupfow,BM_GETCHECK,0,0);
						show_fow = n==BST_CHECKED;
						sendGame();
						ReleaseMutex(mutex);
						DestroyWindow(hwnd);
					}
					break;
				case IDCANCEL:
					if(HIWORD(wParam)==BN_CLICKED) DestroyWindow(hwnd);
					break;
			}
			return 0;
		}
		case WM_SETFOCUS:winapi.setFocusWnd(hwnd);return 0;
		case WM_DESTROY:setupwnd = 0;return 0;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}
*/


