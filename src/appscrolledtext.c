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
#include "appscrolledtext.h"

G_DEFINE_TYPE(AppScrolledText, app_scrolled_text, GTK_TYPE_WIDGET);

enum {
    PROP_0,
    PROP_CONF,
    PROP_NR
};

static void app_scrolled_text_dispose(GObject *object);
static void app_scrolled_text_finalize(GObject *object);
static void app_scrolled_text_set_property(GObject *object,
	guint prop_id, const GValue *value, GParamSpec *pspec);
static void app_scrolled_text_get_property(GObject *object,
	guint prop_id, GValue *value, GParamSpec *pspec);
static void app_scrolled_text_scroll_changed_cb(AppConf *conf, GParamSpec *arg1, gpointer user_data);
static void app_scrolled_text_set_scroll_parameter(AppScrolledText *w);
static void app_scrolled_text_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void create_layout(AppScrolledText *w);
static void destroy_layout(AppScrolledText *w);
static gboolean app_scrolled_text_expose_event(GtkWidget *widget, GdkEventExpose *event);
static gboolean app_scrolled_text_timer_cb(gpointer data);

static void app_scrolled_text_init(AppScrolledText *self)
{
    self->conf = NULL;
    self->conf_sig_id1 = self->conf_sig_id2 = 0;
    
    self->scroll_dx = 1;
    self->scroll_msec = 25;
    
    self->timer_id = 0;
    
    self->width = 1;
    self->height = 1;
    
    self->dx = 0;
    self->text = g_strdup("");
    
    GTK_WIDGET_SET_FLAGS(self, GTK_NO_WINDOW);
    
    create_layout(self);
}

static void app_scrolled_text_class_init(AppScrolledTextClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    
    object_class->dispose = app_scrolled_text_dispose;
    object_class->finalize = app_scrolled_text_finalize;
    object_class->set_property = app_scrolled_text_set_property;
    object_class->get_property = app_scrolled_text_get_property;
    
    widget_class->size_request = app_scrolled_text_size_request;
    widget_class->expose_event = app_scrolled_text_expose_event;
    
    g_object_class_install_property(object_class,
	    PROP_CONF,
	    g_param_spec_pointer("conf",
		    "Conf",
		    "config",
		    G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void app_scrolled_text_dispose(GObject *object)
{
    AppScrolledText *w = APP_SCROLLED_TEXT(object);
    
    if (w->timer_id != 0) {
	g_source_remove(w->timer_id);
	w->timer_id = 0;
    }
    
    if (w->conf != NULL) {
	g_signal_handler_disconnect(w->conf, w->conf_sig_id1);
	g_signal_handler_disconnect(w->conf, w->conf_sig_id2);
	g_object_unref(w->conf);
	w->conf = NULL;
    }
    
    destroy_layout(w);
    
    (*G_OBJECT_CLASS(app_scrolled_text_parent_class)->dispose)(object);
}

static void app_scrolled_text_finalize(GObject *object)
{
    AppScrolledText *w = APP_SCROLLED_TEXT(object);
    
    if (w->text != NULL) {
	g_free(w->text);
	w->text = NULL;
    }
    
    (*G_OBJECT_CLASS(app_scrolled_text_parent_class)->finalize)(object);
}

static void app_scrolled_text_set_property(GObject *object,
	guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AppScrolledText *w = APP_SCROLLED_TEXT(object);
    
    switch (prop_id) {
    case PROP_CONF:
	w->conf = g_value_get_pointer(value);
	g_object_ref(w->conf);
	w->conf_sig_id1 = g_signal_connect(w->conf, "notify::scroll-dx",
		G_CALLBACK(app_scrolled_text_scroll_changed_cb), w);
	w->conf_sig_id2 = g_signal_connect(w->conf, "notify::scroll-msec",
		G_CALLBACK(app_scrolled_text_scroll_changed_cb), w);
	app_scrolled_text_set_scroll_parameter(w);
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static void app_scrolled_text_get_property(GObject *object,
	guint prop_id, GValue *value, GParamSpec *pspec)
{
    AppScrolledText *w = APP_SCROLLED_TEXT(object);
    
    switch (prop_id) {
    case PROP_CONF:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	break;
    }
}

static void app_scrolled_text_scroll_changed_cb(AppConf *conf, GParamSpec *arg1, gpointer user_data)
{
    AppScrolledText *w = user_data;
    
    app_scrolled_text_set_scroll_parameter(w);
}

static void app_scrolled_text_set_scroll_parameter(AppScrolledText *w)
{
    if (w->timer_id != 0) {
	g_source_remove(w->timer_id);
	w->timer_id = 0;
    }
    
    w->scroll_dx = app_conf_get_scroll_dx(w->conf);
    w->scroll_msec = app_conf_get_scroll_msec(w->conf);
    
    w->timer_id = g_timeout_add(w->scroll_msec, app_scrolled_text_timer_cb, w);
}

static void app_scrolled_text_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
    AppScrolledText *w = APP_SCROLLED_TEXT(widget);
    
    requisition->width = w->width;
    requisition->height = w->height;
}

static void create_layout(AppScrolledText *w)
{
    w->layout = gtk_widget_create_pango_layout(GTK_WIDGET(w), w->text);
    pango_layout_get_pixel_size(w->layout, &w->width, &w->height);
    w->dx = 0;
}

static void destroy_layout(AppScrolledText *w)
{
    if (w->layout != NULL) {
	g_object_unref(w->layout);
	w->layout = NULL;
    }
}

static gboolean app_scrolled_text_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    AppScrolledText *w = APP_SCROLLED_TEXT(widget);
    
    if (GTK_WIDGET_VISIBLE(widget)) {
	gtk_paint_layout(widget->style,
		widget->window,
		GTK_WIDGET_STATE(widget),
		FALSE,
		&event->area,
		widget,
		"label",
		widget->allocation.x + widget->allocation.width + w->dx,
		widget->allocation.y,
		w->layout);
    }
    
    return FALSE;
}

static gboolean app_scrolled_text_timer_cb(gpointer data)
{
    AppScrolledText *w = data;
    GtkWidget *widget = GTK_WIDGET(w);
    
    if ((w->dx -= w->scroll_dx) <= -(widget->allocation.width + w->width))
	w->dx = 0;
    
    gtk_widget_queue_draw(widget);
    
    return TRUE;
}

void app_scrolled_text_set_text(AppScrolledText *w, const gchar *text)
{
    if (w->text != NULL)
	g_free(w->text);
    w->text = g_strdup(text);
    
    destroy_layout(w);
    create_layout(w);
}

GtkWidget *app_scrolled_text_new(AppConf *conf)
{
    GtkWidget *w;
    
    w = g_object_new(APP_TYPE_SCROLLED_TEXT,
	    "conf", conf,
	    NULL);
    
    return w;
}

/*EOF*/
