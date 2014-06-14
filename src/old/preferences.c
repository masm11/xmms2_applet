/* CPUFreqApplet
 *  Copyright (C) 2004-2005 Yuuki Harano
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
 * 
 * $Id: preferences.c 312 2005-07-01 12:48:15Z masm $
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

#include "app.h"
#include "cfameter.h"
#include "cfaconn.h"
#include "cfastate.h"
#include "cfacpustates.h"
#include "cfapreferences.h"
#include "cfacondeditordialog.h"
#include "appconf.h"
//#include "i18n-support.h"

static void condition_response_cb(GtkWidget *widget, gint arg, gpointer closure)
{
    struct app_t *app = closure;
    
    xlog_debug(1, "condition_response_cb: arg=%d.\n", arg);
    
    switch (arg) {
    case GTK_RESPONSE_OK:
	xlog_debug(1, "condition_response_cb: save.\n");
	cfa_preferences_set_cond_list(app->prefs,
		cfa_cond_editor_dialog_get_cond_list(CFA_COND_EDITOR_DIALOG(widget)));
	appconf_save(app);
	break;
	
    case GTK_RESPONSE_CANCEL:
    case GTK_RESPONSE_NONE:
    case GTK_RESPONSE_DELETE_EVENT:
	xlog_debug(1, "condition_response_cb: cancel.\n");
	break;
	
    default:
	xlog_debug(1, "condition_response_cb: unknown response_id %d.\n", arg);
	break;
    }
    
    xlog_debug(1, "condition_response_cb: destroy.\n");
    gtk_widget_destroy(widget);
    
    xlog_debug(1, "condition_response_cb: done.\n");
}

static void preferences_condition_cb(GtkWidget *widget, gpointer closure)
{
    struct app_t *app = closure;
    GtkWidget *w;
    
    xlog_debug(1, "condition_cb: 1");
    w = cfa_cond_editor_dialog_new(widget, _("CPUFreq Applet Conditions"),
	    cfa_preferences_get_cond_list(app->prefs));
    g_signal_connect(G_OBJECT(w), "response", G_CALLBACK(condition_response_cb), app);
    xlog_debug(1, "condition_cb: 2");
    gtk_widget_show(w);
    xlog_debug(1, "condition_cb: 3");
}

static void preferences_smooth_cb(GtkWidget *widget, gpointer closure)
{
    struct app_t *app = closure;
    
    xlog_debug(1, "smooth_cb: 1");
    cfa_preferences_set_smooth(app->prefs,
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
    xlog_debug(1, "smooth_cb: 2");
}

static void preferences_response_cb(GtkWidget *widget, gint arg, gpointer closure)
{
    struct app_t *app = closure;
    
    xlog_debug(1, "preferences_response_cb: arg=%d.\n", arg);
    
    switch (arg) {
    case GTK_RESPONSE_ACCEPT:
    case GTK_RESPONSE_NONE:
    case GTK_RESPONSE_DELETE_EVENT:
	xlog_debug(1, "preferences_response_cb: save.\n");
	appconf_save(app);
	break;
	
    default:
	xlog_debug(1, "preferences_response_cb: unknown response_id %d.\n", arg);
	break;
    }
    
    xlog_debug(1, "preferences_response_cb: destroy.\n");
    gtk_widget_destroy(widget);
    
    xlog_debug(1, "preferences_response_cb: done.\n");
    app->dialog = NULL;
}

static void preferences_change_font(GtkFontButton *widget, gpointer user_data)
{
    struct app_t *app = user_data;
    gint font_type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "font-type"));
    const gchar *font_name;
    
    font_name = gtk_font_button_get_font_name(widget);
    
    cfa_preferences_set_font_name(app->prefs, font_type, font_name);
}

static void preferences_change_color(GtkColorButton *widget, gpointer user_data)
{
    struct app_t *app = user_data;
    gint color_type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "color-type"));
    GdkColor *color = g_new(GdkColor, 1);
    
    gtk_color_button_get_color(widget, color);
    gdk_colormap_alloc_color(gtk_widget_get_colormap(GTK_WIDGET(widget)), color, FALSE, TRUE);
    
    cfa_preferences_set_color(app->prefs, color_type, color);
}

static GtkWidget *preferences_make_design(struct app_t *app)
{
    GtkWidget *hbox;
    
    hbox = gtk_hbox_new(TRUE, 5);
    
    {
	GtkWidget *frame;
	GtkWidget *table;
	GtkWidget *w;
	GdkColor color;
	
	frame = gtk_frame_new(_("Auto Mode"));
	gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);
	
	table = gtk_table_new(2, 6, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);
	gtk_container_set_border_width(GTK_CONTAINER(table), 5);
	
	w = gtk_label_new(_("Font"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 0, 1);
	
	w = gtk_font_button_new_with_font(cfa_preferences_get_font_name(app->prefs, CFA_DESIGN_FONT_NAME_AUTO));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 0, 1);
	gtk_font_button_set_title(GTK_FONT_BUTTON(w), _("CPUFreq Applet Font Selection"));
	g_signal_connect(w, "font-set", G_CALLBACK(preferences_change_font), app);
	g_object_set_data(G_OBJECT(w), "font-type", GINT_TO_POINTER(CFA_DESIGN_FONT_NAME_AUTO));
	
	w = gtk_label_new(_("BG Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 1, 2);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_BG_COLOR_AUTO, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 1, 2);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_BG_COLOR_AUTO));
	
	w = gtk_label_new(_("Text Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 2, 3);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_FONT_COLOR_AUTO, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 2, 3);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_FONT_COLOR_AUTO));
	
	w = gtk_label_new(_("Arc Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 3, 4);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_ARC_COLOR_AUTO, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 3, 4);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_ARC_COLOR_AUTO));
	
	w = gtk_label_new(_("Hand Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 4, 5);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_HAND_COLOR_AUTO, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 4, 5);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_HAND_COLOR_AUTO));
	
	w = gtk_label_new(_("Drag Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 5, 6);
	gtk_widget_set_sensitive(w, FALSE);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_DRAG_COLOR_MANUAL, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 5, 6);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_DRAG_COLOR_MANUAL));
	gtk_widget_set_sensitive(w, FALSE);
    }
    
    {
	GtkWidget *frame;
	GtkWidget *table;
	GtkWidget *w;
	GdkColor color;
	
	frame = gtk_frame_new(_("Manual Mode"));
	gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);
	
	table = gtk_table_new(2, 6, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);
	gtk_container_set_border_width(GTK_CONTAINER(table), 5);
	
	w = gtk_label_new(_("Font"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 0, 1);
	
	w = gtk_font_button_new_with_font(cfa_preferences_get_font_name(app->prefs, CFA_DESIGN_FONT_NAME_MANUAL));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 0, 1);
	gtk_font_button_set_title(GTK_FONT_BUTTON(w), _("CPUFreq Applet Font Selection"));
	g_signal_connect(w, "font-set", G_CALLBACK(preferences_change_font), app);
	g_object_set_data(G_OBJECT(w), "font-type", GINT_TO_POINTER(CFA_DESIGN_FONT_NAME_MANUAL));
	
	w = gtk_label_new(_("BG Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 1, 2);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_BG_COLOR_MANUAL, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 1, 2);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_BG_COLOR_MANUAL));
	
	w = gtk_label_new(_("Text Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 2, 3);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_FONT_COLOR_MANUAL, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 2, 3);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_FONT_COLOR_MANUAL));
	
	w = gtk_label_new(_("Arc Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 3, 4);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_ARC_COLOR_MANUAL, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 3, 4);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_ARC_COLOR_MANUAL));
	
	w = gtk_label_new(_("Hand Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 4, 5);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_HAND_COLOR_MANUAL, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 4, 5);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_HAND_COLOR_MANUAL));
	
	w = gtk_label_new(_("Drag Color"));
	gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 5, 6);
	
	cfa_preferences_get_color(app->prefs, CFA_DESIGN_DRAG_COLOR_MANUAL, &color);
	w = gtk_color_button_new_with_color(&color);
	gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 5, 6);
	g_signal_connect(w, "color-set", G_CALLBACK(preferences_change_color), app);
	g_object_set_data(G_OBJECT(w), "color-type", GINT_TO_POINTER(CFA_DESIGN_DRAG_COLOR_MANUAL));
    }
    
    return hbox;
}

static void preferences_applet_destroy_cb(GtkWidget *dialog, gpointer user_data)
{
    struct app_t *app = user_data;
    
    gtk_widget_destroy(app->dialog);
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
	    _("CPUFreq Applet Preferences"),
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
    
    w = preferences_make_design(app);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    
    w = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    
    w = gtk_button_new_with_label(_("Condition"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(preferences_condition_cb), app);
    
    w = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    
    w = gtk_check_button_new_with_label(_("Rotate Smoothly"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), cfa_preferences_get_smooth(app->prefs));
    g_signal_connect(G_OBJECT(w), "toggled", G_CALLBACK(preferences_smooth_cb), app);
    
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
