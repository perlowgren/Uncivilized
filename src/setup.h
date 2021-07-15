
#ifndef _SETUP_H
#define _SETUP_H

#include <gtk/gtk.h>


#define MAX_AI_PLAYERS 6


class setup_dialog {
private:
	struct {
		GtkWidget *window;
		GtkWidget *map;
		GtkWidget *hwrap;
		GtkWidget *vwrap;
		GtkWidget *fow;
		GtkWidget *newcomers;
		GtkWidget *ai_check[MAX_AI_PLAYERS];
		GtkWidget *ai_button[MAX_AI_PLAYERS];
		GtkWidget *playerlist;
	} gui;

	int ai[MAX_AI_PLAYERS];

	static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data);
	static void player_status_toggled_callback(GtkWidget *widget,gpointer data);
	static void player_status_clicked_callback(GtkWidget *widget,gpointer data);
	static void ok_clicked_callback(GtkWidget *widget,gpointer data);
	static void cancel_clicked_callback(GtkWidget *widget,gpointer data);

public:
	setup_dialog(GtkWindow *parent);
	~setup_dialog();

	void setAIStatus(int n,int &s);
	void updatePlayers();
};


#endif /* _SETUP_H */


