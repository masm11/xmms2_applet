/* XMMS2 Applet
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

#include "appconf.h"
#include "app.h"
//#include "i18n-support.h"

static void preferences_response_cb(GtkWidget *widget, gint arg, gpointer user_data)
{
    struct app_t *app = user_data;
    gtk_widget_destroy(widget);
    app->dialog = NULL;
}

static void int_changed_cb(GtkWidget *entry, gpointer user_data)
{
    struct app_t *app = user_data;
    
    const gchar *s = gtk_entry_get_text(GTK_ENTRY(entry));
    gchar *ep;
    gint val = strtoul(s, &ep, 10);
    if (*ep != '\0')
	return;
    
    const gchar *name = g_object_get_data(G_OBJECT(entry), "property");
    g_object_set(G_OBJECT(app->conf),
	    name, val,
	    NULL);
}

static void toggle_changed_cb(GtkWidget *btn, gpointer user_data)
{
    struct app_t *app = user_data;
    
    gboolean val = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn));
    const gchar *name = g_object_get_data(G_OBJECT(btn), "property");
    g_object_set(app->conf,
	    name, val,
	    NULL);
    
    GtkWidget *w;
    
    w = g_object_get_data(G_OBJECT(btn), "sens1");
    if (w != NULL)
	gtk_widget_set_sensitive(w, val);
    
    w = g_object_get_data(G_OBJECT(btn), "sens2");
    if (w != NULL)
	gtk_widget_set_sensitive(w, val);
}

static void path_changed_cb(GtkWidget *entry, gpointer user_data)
{
    struct app_t *app = user_data;
    
    const gchar *s = gtk_entry_get_text(GTK_ENTRY(entry));
    g_object_set(app->conf,
	    "path", s,
	    NULL);
}

static GtkWidget *preferences_make_size(struct app_t *app)
{
    GtkWidget *hbox;
    
    hbox = gtk_hbox_new(FALSE, 0);
    
    GtkWidget *w;
    w = gtk_label_new("Size:");
    gtk_box_pack_start(GTK_BOX(hbox), w, FALSE, FALSE, 0);
    
    w = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox), w, FALSE, FALSE, 0);
    {
	char buf[128];
	snprintf(buf, sizeof buf, "%d", app_conf_get_size(app->conf));
	gtk_entry_set_text(GTK_ENTRY(w), buf);
    }
    g_object_set_data(G_OBJECT(w), "property", "size");
    g_signal_connect(w, "activate", G_CALLBACK(int_changed_cb), app);
    
    return hbox;
}

static GtkWidget *preferences_make_title_scroll(struct app_t *app)
{
    GtkWidget *frame = gtk_frame_new("Title Scroll");
    
    GtkWidget *table = gtk_table_new(2, 3, FALSE);
    gtk_container_add(GTK_CONTAINER(frame), table);
    
    GtkWidget *w;
    
    w = gtk_label_new("Interval:");
    gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 0, 1);
    gtk_misc_set_alignment(GTK_MISC(w), 1.0, 0.5);
    
    w = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 0, 1);
    {
	char buf[128];
	snprintf(buf, sizeof buf, "%d", app_conf_get_scroll_msec(app->conf));
	gtk_entry_set_text(GTK_ENTRY(w), buf);
    }
    g_object_set_data(G_OBJECT(w), "property", "scroll-msec");
    g_signal_connect(w, "activate", G_CALLBACK(int_changed_cb), app);
    
    w = gtk_label_new("msec");
    gtk_table_attach_defaults(GTK_TABLE(table), w, 2, 3, 0, 1);
    gtk_misc_set_alignment(GTK_MISC(w), 0, 0.5);
    
    w = gtk_label_new("Step:");
    gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 1, 2);
    gtk_misc_set_alignment(GTK_MISC(w), 1.0, 0.5);
    
    w = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 1, 2);
    {
	char buf[128];
	snprintf(buf, sizeof buf, "%d", app_conf_get_scroll_dx(app->conf));
	gtk_entry_set_text(GTK_ENTRY(w), buf);
    }
    g_object_set_data(G_OBJECT(w), "property", "scroll-dx");
    g_signal_connect(w, "activate", G_CALLBACK(int_changed_cb), app);
    
    w = gtk_label_new("pixel");
    gtk_table_attach_defaults(GTK_TABLE(table), w, 2, 3, 1, 2);
    gtk_misc_set_alignment(GTK_MISC(w), 0, 0.5);
    
    return frame;
}

static GtkWidget *preferences_make_xmms2_path(struct app_t *app)
{
    GtkWidget *frame = gtk_frame_new("XMMS2 Path");
    
    GtkWidget *table = gtk_table_new(2, 3, FALSE);
    gtk_container_add(GTK_CONTAINER(frame), table);
    
    GtkWidget *w;
    gboolean use_this;
    
    w = gtk_check_button_new_with_label("Use This Path");
    gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 3, 0, 1);
    use_this = app_conf_get_path_specified(app->conf);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), use_this);
    g_object_set_data(G_OBJECT(w), "property", "path-specified");
    g_signal_connect(w, "toggled", G_CALLBACK(toggle_changed_cb), app);
    GtkWidget *btn = w;
    
    w = gtk_label_new("Path:");
    gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 1, 2);
    gtk_misc_set_alignment(GTK_MISC(w), 1.0, 0.5);
    if (use_this)
	gtk_widget_set_sensitive(w, use_this);
    g_object_set_data(G_OBJECT(btn), "sens1", w);
    
    w = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), w, 2, 3, 1, 2);
    {
	char *s = app_conf_get_path(app->conf);
	gtk_entry_set_text(GTK_ENTRY(w), s);
	g_free(s);
    }
    if (use_this)
	gtk_widget_set_sensitive(w, use_this);
    g_object_set_data(G_OBJECT(btn), "sens2", w);
    g_signal_connect(w, "activate", G_CALLBACK(path_changed_cb), app);
    
    return frame;
}

static void preferences_applet_destroy_cb(GtkWidget *dialog, gpointer user_data)
{
    struct app_t *app = user_data;
    gtk_widget_destroy(app->dialog);
    app->dialog = NULL;
}

static void preferences_destroy_cb(GtkWidget *dialog, gpointer user_data)
{
    struct app_t *app = user_data;
    
    g_signal_handlers_disconnect_by_func(app->applet, preferences_applet_destroy_cb, app);
}

static GtkWidget *preferences_make_dialog(struct app_t *app)
{
    GtkWidget *dialog;
    GtkWidget *w;
    
    dialog = gtk_dialog_new_with_buttons(
	    _("XMMS2 Applet Preferences"),
	    NULL,
	    0,
	    GTK_STOCK_CLOSE, GTK_RESPONSE_ACCEPT,
	    NULL);
    app->dialog = dialog;
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 5);
    
    gtk_box_set_spacing(GTK_BOX(GTK_DIALOG(dialog)->vbox), 5);
    
    g_signal_connect(G_OBJECT(app->applet), "destroy",
	    G_CALLBACK(preferences_applet_destroy_cb), app);
    g_signal_connect(G_OBJECT(dialog), "destroy",
	    G_CALLBACK(preferences_destroy_cb), app);
    g_signal_connect(G_OBJECT(dialog), "response",
	    G_CALLBACK(preferences_response_cb), app);
    
    w = preferences_make_size(app);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    
    w = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    
    w = preferences_make_title_scroll(app);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    
    w = preferences_make_xmms2_path(app);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    
    return dialog;
}

void preferences(BonoboUIComponent *uic, gpointer data, const gchar *verbname)
{
    struct app_t *app = data;
    
    if (app->dialog != NULL) {
	gdk_window_raise(GTK_WIDGET(app->dialog)->window);
    } else {
	app->dialog = preferences_make_dialog(app);
	gtk_widget_show_all(app->dialog);
    }
}

/*EOF*/
