#ifndef _LOGIN_H
#define _LOGIN_H


#include <gtk/gtk.h>


class login_dialog {
private:
	struct {
		GtkWidget *window;
		GtkWidget *nick;
		GtkWidget *host;
	} gui;

	static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data);
	static void ok_clicked_callback(GtkWidget *widget,gpointer data);
	static void cancel_clicked_callback(GtkWidget *widget,gpointer data);

public:
	login_dialog(GtkWindow *parent);
	~login_dialog();
};


#endif /* _LOGIN_H */


