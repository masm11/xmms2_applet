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
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <libgnomeui/gnome-about.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "app.h"
#include "about.h"

static const gchar *authors[] = {
    "Yuuki Harano <masm@flowernet.gr.jp>",
    NULL
};

static const gchar *documenters[] = {
    NULL
};

static const gchar *translator_credits = NULL;

void about(BonoboUIComponent *uic, gpointer data, const gchar *verbname)
{
    static GtkWidget *w = NULL;
    struct app_t *app = data;
    
    if (w != NULL) {
	gtk_window_set_screen(GTK_WINDOW(w),
		gtk_widget_get_screen(GTK_WIDGET(app->applet)));
	gtk_window_present(GTK_WINDOW(w));
	return;
    }
    
    w = gnome_about_new(
	    "XMMS2 Applet", VERSION,
	    "(C) 2005-2006 Yuuki Harano",
	    _("Released under the GNU General Public License.\n\n"
	    "A simple XMMS2 applet."),
	    authors,
	    documenters,
	    translator_credits,
	    NULL);
    
    gtk_window_set_screen(GTK_WINDOW(w),
	    gtk_widget_get_screen(GTK_WIDGET(app->applet)));
    gtk_window_set_wmclass(GTK_WINDOW(w), "xMMS2Applet", "XMMS2Applet");
    g_signal_connect(G_OBJECT(w), "destroy",
	    G_CALLBACK(gtk_widget_destroyed), &w);
    
    gtk_widget_show(w);
}
