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

#ifndef APPCONN_H__INCLUDED
#define APPCONN_H__INCLUDED

#include <common.h>
#include <glib-object.h>
#include <xmmsclient/xmmsclient.h>
#include "appconf.h"

G_BEGIN_DECLS

#define APP_TYPE_CONN                  (app_conn_get_type())
#define APP_CONN(obj)                  (G_TYPE_CHECK_INSTANCE_CAST((obj), APP_TYPE_CONN, AppConn))
#define APP_CONN_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST((klass), APP_TYPE_CONN, AppConnClass))
#define APP_IS_CONN(obj)               (G_TYPE_CHECK_INSTANCE_TYPE((obj), APP_TYPE_CONN))
#define APP_IS_CONN_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), APP_TYPE_CONN))
#define APP_CONN_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS((obj), APP_TYPE_CONN, AppConnClass))

typedef struct _AppConn       AppConn;
typedef struct _AppConnClass  AppConnClass;

struct _AppConn
{
    GObject object;
    
    AppConf *conf;
    gulong conf_sig_id1, conf_sig_id2;
    
    xmmsc_connection_t *conn;
    void *conn_data;
    guint timer_id;
    
    gint playstat;
    guint play_time;	// sec
    gchar *title;
};

struct _AppConnClass
{
    GObjectClass parent_class;
};

enum {
    APP_CONN_PLAY_STATUS_BROKEN = 0,
    APP_CONN_PLAY_STATUS_PLAY = 1,
    APP_CONN_PLAY_STATUS_STOP = 2,
    APP_CONN_PLAY_STATUS_PAUSE = 3,
};

GType app_conn_get_type  (void) G_GNUC_CONST;

AppConn *app_conn_new(AppConf *conf);
void app_conn_play_or_stop(AppConn *w);

G_END_DECLS

#endif /* ifndef APPCONN_H__INCLUDED */
