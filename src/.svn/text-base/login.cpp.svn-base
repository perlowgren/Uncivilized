
#include "config.h"
#include "main.h"
#include "login.h"


gboolean login_dialog::delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	login_dialog *dlg = (login_dialog *)data;
	delete dlg;
	app.handleAction(ACTION_QUIT);
	return TRUE;
}

void login_dialog::ok_clicked_callback(GtkWidget *widget,gpointer data) {
debug_output("login_dialog::ok_clicked(1)\n");
	login_dialog *dlg = (login_dialog *)data;
	char nick[33],host[65];
	strncpy(nick,gtk_entry_get_text(GTK_ENTRY(dlg->gui.nick)),32);
	strncpy(host,gtk_entry_get_text(GTK_ENTRY(dlg->gui.host)),64);
	if(!*nick) {
//		ShowMessage(dlg->window,app.get("login_err_uspw"),app.get("login_err_capt"));
		return;
	}
	delete dlg;
	app.setNick(nick);
	app.handleAction(ACTION_LOGIN,PORT,(intptr_t)host);
/*	login_dialog *dlg = (login_dialog *)data;
	strncpy(app.local_user,gtk_entry_get_text(GTK_ENTRY(dlg->usertext)),32);
	strncpy(app.local_password,gtk_entry_get_text(GTK_ENTRY(dlg->passwordtext)),32);
app.printf("login.. 1");
	if(!*app.local_user || !*app.local_password) {
		ShowMessage(dlg->window,app.get("login_err_uspw"),app.get("login_err_capt"));
		return;
	}
app.printf("login.. 2");
	aHttp http;
	http.setUserAgent(app.agent);
	const char *file = http.post(WEB_HOST,WEB_PATH "login.php","u=%s&p=%s",app.local_user,app.local_password);
	if(!file || !*file) ShowMessage(dlg->window,app.get("login_err_csrv"),app.get("login_err_capt"));
	else if(!strncmp(file,"[ERROR]",7)) ShowMessage(dlg->window,app.get(&file[7]),app.get("login_err_capt"));
	else {
		FILE *fp = fopen(DAT_PATH "properties.dat","w");
		fprintf(fp,"%s",file);
		fclose(fp);
		app.properties.removeAll();
		app.properties.load(DAT_PATH "properties.dat");
		app.setLocalID(atoi(app.properties.getString("id")));
		app.mainFrame->loadHosts();
		app.mainFrame->loadChars();
		gtk_widget_destroy(dlg->window);
		delete dlg;
	}
app.printf("login: %s",file);*/
}

void login_dialog::cancel_clicked_callback(GtkWidget *widget,gpointer data) {
	login_dialog *dlg = (login_dialog *)data;
	delete dlg;
	app.handleAction(ACTION_QUIT);
}


login_dialog::login_dialog(GtkWindow *parent) {
	GtkWidget *table;
	GtkWidget *widget;

debug_output("login_dialog::login_dialog(1)\n");
	gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(gui.window),_("Login to Uncivilized"));
	gtk_window_set_transient_for(GTK_WINDOW(gui.window),parent);
	gtk_window_set_modal(GTK_WINDOW(gui.window),TRUE);
//	gtk_widget_set_size_request(window,250,180);
	gtk_window_set_resizable(GTK_WINDOW(gui.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(gui.window),GTK_WIN_POS_CENTER);
//	gtk_window_set_icon(GTK_WINDOW(window),app.createPixbuf(IMAGES_PATH "icons/icon16.png"));
	gtk_container_set_border_width(GTK_CONTAINER(gui.window),5);

	table = gtk_table_new(2,7,FALSE);

	widget = gtk_label_new(_("Nickname"));
	gtk_misc_set_alignment(GTK_MISC(widget),0,0.5);
	gtk_table_attach(GTK_TABLE(table),widget,0,2,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gui.nick = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(gui.nick),app.getNick());
	g_signal_connect(G_OBJECT(gui.nick),"activate",G_CALLBACK(ok_clicked_callback),this);
	gtk_table_attach(GTK_TABLE(table),gui.nick,0,2,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	widget = gtk_label_new(_("Game Host"));
	gtk_misc_set_alignment(GTK_MISC(widget),0,0.5);
	gtk_table_attach(GTK_TABLE(table),widget,0,2,2,3,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gui.host = gtk_entry_new();
//	gtk_entry_set_text(GTK_ENTRY(gui.host),"");
	g_signal_connect(G_OBJECT(gui.host),"activate",G_CALLBACK(ok_clicked_callback),this);
	gtk_table_attach(GTK_TABLE(table),gui.host,0,2,3,4,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	widget = gtk_button_new_with_label(_("OK"));
	gtk_widget_set_size_request(widget,80,-1);
	g_signal_connect(G_OBJECT(widget),"clicked",G_CALLBACK(ok_clicked_callback),this);
	gtk_table_attach(GTK_TABLE(table),widget,0,1,6,7,
		(GtkAttachOptions)(GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),2,2);
	widget = gtk_button_new_with_label(_("Cancel"));
	gtk_widget_set_size_request(widget,80,-1);
	g_signal_connect(G_OBJECT(widget),"clicked",G_CALLBACK(cancel_clicked_callback),this);
	gtk_table_attach(GTK_TABLE(table),widget,1,2,6,7,
		(GtkAttachOptions)(GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),2,2);

	gtk_container_add(GTK_CONTAINER(gui.window),table);
	gtk_widget_show_all(gui.window);
	g_signal_connect(G_OBJECT(gui.window),"delete_event",G_CALLBACK(delete_event_callback),this);
}

login_dialog::~login_dialog() {
	gtk_widget_destroy(gui.window);
}



