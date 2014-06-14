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

#ifndef APP_H__INCLUDED
#define APP_H__INCLUDED

#include <common.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <panel-applet.h>

#include "appconn.h"
#include "appconf.h"

#include <xmmsclient/xmmsclient.h>

struct app_t {
    PanelApplet *applet;
    
    AppConf *conf;
    gulong conf_sig_id;
    
    AppConn *conn;
    
    GtkWidget *title_stext;
    GtkWidget *play_time_label;
    GtkWidget *play_status_image;
    
    GtkWidget *dialog;
};

#endif	/* ifndef APP_H__INCLUDED */
