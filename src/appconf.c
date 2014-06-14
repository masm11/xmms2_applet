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
#include <string.h>
#include <glib.h>
#include <panel-applet.h>
#include <panel-applet-gconf.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include "appconf.h"

G_DEFINE_TYPE(AppConf, app_conf, G_TYPE_OBJECT);

enum {
    SIG_SCROLL_CHANGED,
    SIG_PATH_CHANGED,
    SIG_SIZE_CHANGED,
    SIG_NR
};

enum {
    PROP_0,
    PROP_APPLET,
    PROP_SCROLL_DX,
    PROP_SCROLL_MSEC,
    PROP_PATH_SPECIFIED,
    PROP_PATH,
    PROP_SIZE,
    PROP_NR
};

static void app_conf_dispose(GObject *object);
static void app_conf_finalize(GObject *object);
static void app_conf_set_property(GObject *object,
	guint prop_id, const GValue *value, GParamSpec *pspec);
static void app_conf_get_property(GObject *object,
	guint prop_id, GValue *value, GParamSpec *pspec);
static void appconf_load(AppConf *w);

static guint signals[SIG_NR] = { 0 };

static void app_conf_init(AppConf *self)
{
    self->applet = NULL;
    
    self->scroll.dx = 1;
    self->scroll.msec = 25;
    
    self->path.specified = TRUE;
    self->path.path = g_strdup("");
    
    self->size = 48;
}

static void app_conf_class_init(AppConfClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    
    object_class->dispose = app_conf_dispose;
    object_class->finalize = app_conf_finalize;
    object_class->set_property = app_conf_set_property;
    object_class->get_property = app_conf_get_property;
    
    signals[SIG_SCROLL_CHANGED] = g_signal_new("scroll-changed",
	    G_OBJECT_CLASS_TYPE(klass),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
    
    signals[SIG_PATH_CHANGED] = g_signal_new("path-changed",
	    G_OBJECT_CLASS_TYPE(klass),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
    
    signals[SIG_SIZE_CHANGED] = g_signal_new("size-changed",
	    G_OBJECT_CLASS_TYPE(klass),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
    
    g_object_class_install_property(object_class,
	    PROP_APPLET,
	    g_param_spec_pointer("applet",
		    "Applet",
		    "applet",
		    G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
    
    g_object_class_install_property(object_class,
	    PROP_SCROLL_DX,
	    g_param_spec_int("scroll-dx",
		    "Scroll-dx",
		    "scroll dx",
		    1, 100, 1,
		    G_PARAM_READWRITE));
    
    g_object_class_install_property(object_class,
	    PROP_SCROLL_MSEC,
	    g_param_spec_int("scroll-msec",
		    "Scroll-msec",
		    "scroll msec",
		    1, 1000, 25,
		    G_PARAM_READWRITE));
    
    g_object_class_install_property(object_class,
	    PROP_PATH_SPECIFIED,
	    g_param_spec_boolean("path-specified",
		    "Path-specified",
		    "path specified",
		    FALSE,
		    G_PARAM_READWRITE));
    
    g_object_class_install_property(object_class,
	    PROP_PATH,
	    g_param_spec_string("path",
		    "Path",
		    "path",
		    "",
		    G_PARAM_READWRITE));
    
    g_object_class_install_property(object_class,
	    PROP_SIZE,
	    g_param_spec_int("size",
		    "Size",
		    "size",
		    1, 1000, 48,
		    G_PARAM_READWRITE));
}

static void app_conf_dispose(GObject *object)
{
    AppConf *w = APP_CONF(object);
    
    (*G_OBJECT_CLASS(app_conf_parent_class)->dispose)(object);
}

static void app_conf_finalize(GObject *object)
{
    AppConf *w = APP_CONF(object);
    
    if (w->path.path != NULL) {
	g_free(w->path.path);
	w->path.path = NULL;
    }
    
    (*G_OBJECT_CLASS(app_conf_parent_class)->finalize)(object);
}

static void app_conf_set_property(GObject *object,
	guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AppConf *w = APP_CONF(object);
    
    switch (prop_id) {
    case PROP_APPLET:
	w->applet = g_value_get_pointer(value);
	break;
	
    case PROP_SCROLL_DX:
	w->scroll.dx = g_value_get_int(value);
	panel_applet_gconf_set_int(w->applet, "scroll_dx", w->scroll.dx, NULL);
	break;
	
    case PROP_SCROLL_MSEC:
	w->scroll.msec = g_value_get_int(value);
	panel_applet_gconf_set_int(w->applet, "scroll_msec", w->scroll.msec, NULL);
	break;
	
    case PROP_PATH_SPECIFIED:
	w->path.specified = g_value_get_boolean(value);
	panel_applet_gconf_set_bool(w->applet, "path_specified", w->path.specified, NULL);
	break;
	
    case PROP_PATH:
	if (w->path.path != NULL)
	    g_free(w->path.path);
	w->path.path = g_value_dup_string(value);
	panel_applet_gconf_set_string(w->applet, "path", w->path.path, NULL);
	break;
	
    case PROP_SIZE:
	w->size = g_value_get_int(value);
	panel_applet_gconf_set_int(w->applet, "size", w->size, NULL);
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static void app_conf_get_property(GObject *object,
	guint prop_id, GValue *value, GParamSpec *pspec)
{
    AppConf *w = APP_CONF(object);
    
    switch (prop_id) {
    case PROP_APPLET:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	break;
	
    case PROP_SCROLL_DX:
	g_value_set_int(value, w->scroll.dx);
	break;
	
    case PROP_SCROLL_MSEC:
	g_value_set_int(value, w->scroll.msec);
	break;
	
    case PROP_PATH_SPECIFIED:
	g_value_set_boolean(value, w->path.specified);
	break;
	
    case PROP_PATH:
	g_value_set_string(value, w->path.path);
	break;
	
    case PROP_SIZE:
	g_value_set_int(value, w->size);
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	break;
    }
}

AppConf *app_conf_new(PanelApplet *applet)
{
    AppConf *w;
    
    w = g_object_new(APP_TYPE_CONF,
	    "applet", applet,
	    NULL);
    
    appconf_load(w);
    
    return w;
}

/****************/

static void appconf_load(AppConf *w)
{
    gint scroll_dx;
    gint scroll_msec;
    gboolean path_specified;
    gchar *path;
    gint size;
    
    scroll_dx = panel_applet_gconf_get_int(w->applet, "scroll_dx", NULL);
    if (scroll_dx < 1)
	scroll_dx = 1;
    
    scroll_msec = panel_applet_gconf_get_int(w->applet, "scroll_msec", NULL);
    if (scroll_msec < 1)
	scroll_msec = 25;
    
    path_specified = panel_applet_gconf_get_bool(w->applet, "path_specified", NULL);
    
    path = panel_applet_gconf_get_string(w->applet, "path", NULL);
    if (path == NULL)
	path = g_strdup("");
    
    size = panel_applet_gconf_get_int(w->applet, "size", NULL);
    if (size < 1)
	size = 48;
    
    g_object_set(G_OBJECT(w),
	    "scroll-dx", scroll_dx,
	    "scroll-msec", scroll_msec,
	    "path-specified", path_specified,
	    "path", path,
	    "size", size,
	    NULL);
    
    g_free(path);
}

/*EOF*/
