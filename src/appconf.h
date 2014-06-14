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

#ifndef APPCONF_H__INCLUDED
#define APPCONF_H__INCLUDED

#include <common.h>
#include <glib-object.h>
#include <panel-applet.h>

G_BEGIN_DECLS

#define APP_TYPE_CONF                  (app_conf_get_type())
#define APP_CONF(obj)                  (G_TYPE_CHECK_INSTANCE_CAST((obj), APP_TYPE_CONF, AppConf))
#define APP_CONF_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST((klass), APP_TYPE_CONF, AppConfClass))
#define APP_IS_CONF(obj)               (G_TYPE_CHECK_INSTANCE_TYPE((obj), APP_TYPE_CONF))
#define APP_IS_CONF_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), APP_TYPE_CONF))
#define APP_CONF_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS((obj), APP_TYPE_CONF, AppConfClass))

typedef struct _AppConf       AppConf;
typedef struct _AppConfClass  AppConfClass;

struct _AppConf
{
    GObject object;
    
    PanelApplet *applet;
    
    struct {
	guint dx;
	guint msec;
    } scroll;
    
    struct {
	gboolean specified;
	gchar *path;
    } path;
    
    gint size;
};

struct _AppConfClass
{
    GObjectClass parent_class;
};

GType app_conf_get_type  (void) G_GNUC_CONST;

AppConf *app_conf_new(PanelApplet *applet);

static inline gint app_conf_get_size(AppConf *w)
{
    gint val;
    g_object_get(w, "size", &val, NULL);
    return val;
}

static inline gboolean app_conf_get_path_specified(AppConf *w)
{
    gboolean val;
    g_object_get(w, "path-specified", &val, NULL);
    return val;
}

static inline gchar *app_conf_get_path(AppConf *w)	// Í×²òÊü
{
    gchar *val;
    g_object_get(w, "path", &val, NULL);
    return val;
}

static inline gint app_conf_get_scroll_dx(AppConf *w)
{
    gint val;
    g_object_get(w, "scroll-dx", &val, NULL);
    return val;
}

static inline gint app_conf_get_scroll_msec(AppConf *w)
{
    gint val;
    g_object_get(w, "scroll-msec", &val, NULL);
    return val;
}

G_END_DECLS

#endif /* ifndef APPCONF_H__INCLUDED */
