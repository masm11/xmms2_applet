/* XMMS2Applet
 *  Copyright (C) 2005-2006 Yuuki Harano
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <panel-applet.h>
#include "appconn.h"
#include "appscrolledtext.h"
#include "about.h"
#include "appconf.h"
#include "apppref.h"
#include "app.h"

static const BonoboUIVerb menu_verbs[] = 
{
    BONOBO_UI_VERB("XMMS2AppletPreferences", preferences),
    BONOBO_UI_VERB("XMMS2AppletAbout", about),
    
    BONOBO_UI_VERB_END
};

/**************** size / orient changed ****/

static gboolean get_applet_vert(PanelApplet *applet)
{
    switch (panel_applet_get_orient(applet)) {
    case PANEL_APPLET_ORIENT_UP:
    case PANEL_APPLET_ORIENT_DOWN:
	return FALSE;
	break;
    default:
	return TRUE;
	break;
    }
}

static void set_size(struct app_t *app)
{
    if (!get_applet_vert(app->applet)) {
	gtk_widget_set_size_request(GTK_WIDGET(app->applet), app_conf_get_size(app->conf), -1);
    } else {
	gtk_widget_set_size_request(GTK_WIDGET(app->applet), -1, app_conf_get_size(app->conf));
    }
}

static void change_orient_cb(PanelApplet *applet, gint arg1, gpointer user_data)
{
    struct app_t *app = user_data;
    set_size(app);
}

static void size_changed_cb(AppConf *conf, GParamSpec *arg1, gpointer user_data)
{
    struct app_t *app = user_data;
    set_size(app);
}

/**************** main ****/

static void playtime_changed_cb(gpointer obj, guint play_time, gpointer user_data)
{
    struct app_t *app = user_data;
    char buf[128];
    guint hh, mm, ss;
    
    ss = play_time % 60,
    mm = play_time / 60 % 60,
    hh = play_time / 60 / 60;
    
    if (hh < 1)
	snprintf(buf, sizeof buf, "%d:%02d", mm, ss);
    else
	snprintf(buf, sizeof buf, "%d:%02d:%02d", hh, mm, ss);
    
    gtk_label_set_text(GTK_LABEL(app->play_time_label), buf);
}

static void playstatus_changed_cb(gpointer obj, gint status, gpointer user_data)
{
    struct app_t *app = user_data;
    const gchar *stock = NULL;
    
    switch (status) {
    case APP_CONN_PLAY_STATUS_BROKEN:
	stock = GTK_STOCK_DIALOG_ERROR;
	break;
    case APP_CONN_PLAY_STATUS_PLAY:
	stock = GTK_STOCK_MEDIA_PLAY;
	break;
    case APP_CONN_PLAY_STATUS_STOP:
	stock = GTK_STOCK_MEDIA_STOP;
	break;
    case APP_CONN_PLAY_STATUS_PAUSE:
	stock = GTK_STOCK_MEDIA_PAUSE;
	break;
    }
    
    if (stock != NULL) {
	gtk_image_set_from_stock(GTK_IMAGE(app->play_status_image),
		stock, GTK_ICON_SIZE_SMALL_TOOLBAR);
    }
}

static void title_changed_cb(gpointer obj, gchar *title, gpointer user_data)
{
    struct app_t *app = user_data;
    
    app_scrolled_text_set_text(APP_SCROLLED_TEXT(app->title_stext), title);
}

static void play_or_stop_cb(GtkWidget *w, gpointer user_data)
{
    struct app_t *app = user_data;
    app_conn_play_or_stop(app->conn);
}

static gboolean do_not_eat_button_press(GtkWidget *widget, GdkEventButton *event)
{
    /* gnome-applet 2.12.2 の clock-applet からもらってきた。
     * 右クリックを透過させる。
     */
    
    if (event->button != 1)
	g_signal_stop_emission_by_name(widget, "button_press_event");
    return FALSE;
}

static void destroy_cb(PanelApplet *applet, gpointer data)
{
    struct app_t *app = data;
    
    if (app->conn != NULL) {
	g_object_unref(app->conn);
	app->conn = NULL;
    }
    
    if (app->conf != NULL) {
	g_signal_handler_disconnect(app->conf, app->conf_sig_id);
	g_object_unref(app->conf);
	app->conf = NULL;
    }
    
    g_free(app);
}

static gboolean xmms2_applet_start(
	PanelApplet *applet, const gchar *iid, gpointer data)
{
    struct app_t *app;
    
    if (strcmp (iid, "OAFIID:XMMS2Applet") != 0)
	return FALSE;
    
    /* button に focus の枠を表示しないように */
    gtk_rc_parse_string("							\n\
	style \"xmms2-applet-button-style\" {					\n\
	  GtkWidget::focus-line-width = 0					\n\
	  GtkWidget::focus-padding = 0						\n\
	}									\n\
	widget \"*.xmms2-applet-button\" style \"xmms2-applet-button-style\"	\n\
    ");
    
    app = g_new(struct app_t, 1);
    memset(app, 0, sizeof *app);
    
    app->applet = applet;
    
    panel_applet_set_flags(applet, PANEL_APPLET_EXPAND_MINOR);
    panel_applet_setup_menu_from_file(applet, NULL, "XMMS2Applet.xml", NULL, menu_verbs, app);
    
    g_signal_connect(G_OBJECT(applet), "destroy",
	    G_CALLBACK(destroy_cb), app);
    g_signal_connect(G_OBJECT(applet), "change_orient",
	    G_CALLBACK(change_orient_cb), app);
    
    panel_applet_add_preferences(applet, "/schemas/apps/xmms2_applet/prefs", NULL);
    
    app->conf = app_conf_new(applet);
    app->conf_sig_id = g_signal_connect(G_OBJECT(app->conf), "notify::size", G_CALLBACK(size_changed_cb), app);
    
    set_size(app);
    
    GtkWidget *button = gtk_button_new();
    gtk_widget_set_name(button, "xmms2-applet-button");
    gtk_container_add(GTK_CONTAINER(applet), button);
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    g_signal_connect(button, "clicked", G_CALLBACK(play_or_stop_cb), app);
    g_signal_connect(button, "button_press_event", G_CALLBACK(do_not_eat_button_press), app);
    
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(button), vbox);
    
    app->title_stext = app_scrolled_text_new(app->conf);
    gtk_box_pack_start(GTK_BOX(vbox), app->title_stext, FALSE, FALSE, 0);
    
    app->play_time_label = gtk_label_new("0:00");
    gtk_box_pack_start(GTK_BOX(vbox), app->play_time_label, FALSE, FALSE, 0);
    gtk_misc_set_alignment(GTK_MISC(app->play_time_label), 1.0, 0.5);
    
    app->play_status_image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_SMALL_TOOLBAR);
//    app->play_status_image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR, "xmms2-applet");
    gtk_box_pack_start(GTK_BOX(vbox), app->play_status_image, TRUE, TRUE, 0);
    gtk_misc_set_alignment(GTK_MISC(app->play_status_image), 1.0, 1.0);
    
    gtk_widget_show_all(GTK_WIDGET(applet));
    
    app->conn = app_conn_new(app->conf);
    
    g_signal_connect(app->conn, "playtime-changed",
	    G_CALLBACK(playtime_changed_cb), app);
    g_signal_connect(app->conn, "title-changed",
	    G_CALLBACK(title_changed_cb), app);
    g_signal_connect(app->conn, "playstat-changed",
	    G_CALLBACK(playstatus_changed_cb), app);
    
    return TRUE;
}

PANEL_APPLET_BONOBO_FACTORY(
	"OAFIID:XMMS2Applet_Factory",
	PANEL_TYPE_APPLET,
	PACKAGE,
	VERSION,
	xmms2_applet_start,
	NULL);
