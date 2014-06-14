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

#ifndef APPSCROLLEDTEXT_H__INCLUDED
#define APPSCROLLEDTEXT_H__INCLUDED

#include <common.h>

#include <gtk/gtkwidget.h>
#include "appconf.h"


G_BEGIN_DECLS

#define APP_TYPE_SCROLLED_TEXT			app_scrolled_text_get_type()
#define APP_SCROLLED_TEXT(obj)			G_TYPE_CHECK_INSTANCE_CAST((obj), APP_TYPE_SCROLLED_TEXT, AppScrolledText)
#define APP_SCROLLED_TEXT_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), APP_TYPE_SCROLLED_TEXT, AppScrolledTextClass)
#define APP_IS_SCROLLED_TEXT(obj)		G_TYPE_CHECK_INSTANCE_TYPE((obj), APP_TYPE_SCROLLED_TEXT)
#define APP_IS_SCROLLED_TEXT_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE((klass), APP_TYPE_SCROLLED_TEXT)
#define APP_SCROLLED_TEXT_GET_CLASS(obj)        G_TYPE_INSTANCE_GET_CLASS((obj), APP_TYPE_SCROLLED_TEXT, AppScrolledTextClass)


typedef struct _AppScrolledText AppScrolledText;
typedef struct _AppScrolledTextClass AppScrolledTextClass;

struct _AppScrolledText
{
    GtkWidget widget;
    
    AppConf *conf;
    gulong conf_sig_id1, conf_sig_id2;
    
    gint scroll_dx, scroll_msec;
    
    guint timer_id;
    
    gint width, height;
    gint dx;
    gchar *text;
    PangoLayout *layout;
};

struct _AppScrolledTextClass
{
    GtkWidgetClass parent_class;
};


GType	   app_scrolled_text_get_type	      (void) G_GNUC_CONST;
GtkWidget *app_scrolled_text_new	      (AppConf *conf);
void       app_scrolled_text_set_text	      (AppScrolledText *w,
					       const gchar *text);

G_END_DECLS

#endif	/* ifndef APPSCROLLEDTEXT_H__INCLUDED */
