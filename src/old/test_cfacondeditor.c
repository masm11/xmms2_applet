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
 * $Id: test_cfacondeditor.c 287 2005-06-25 12:03:54Z masm $
 */

#include <common.h>
#include <gtk/gtk.h>
#include "cfacondeditor.h"

int main(int argc, char **argv)
{
    GtkWidget *top, *w;
    
    debug_init(1);
    
    gtk_init(&argc, &argv);
    
    top = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    w = cfa_cond_editor_new();
    cfa_cond_editor_test(CFA_COND_EDITOR(w));
    gtk_container_add(GTK_CONTAINER(top), w);
    
    gtk_widget_show_all(top);
    gtk_main();
    
    return 0;
}

/*EOF*/
