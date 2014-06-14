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
 * $Id: cfacondeditordialog.c 287 2005-06-25 12:03:54Z masm $
 */

#include <common.h>
#include <glib.h>
#include "cfacondeditordialog.h"

G_DEFINE_TYPE(CfaCondEditorDialog, cfa_cond_editor_dialog, GTK_TYPE_DIALOG);

static void cfa_cond_editor_dialog_class_init(CfaCondEditorDialogClass *klass)
{
}

static void cfa_cond_editor_dialog_init(CfaCondEditorDialog *cedialog)
{
    GtkWidget *action_area_button_box, *frame;  
    
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(cedialog)->vbox), frame);
    gtk_container_set_border_width(GTK_CONTAINER(frame), 10); 
    gtk_widget_show(frame); 
    
    cedialog->cond_editor = cfa_cond_editor_new();
    gtk_container_add(GTK_CONTAINER(frame), cedialog->cond_editor);
    gtk_widget_show(cedialog->cond_editor);
    
    action_area_button_box = GTK_DIALOG(cedialog)->action_area;
    
    cedialog->cancel_button = gtk_dialog_add_button(
	    GTK_DIALOG(cedialog),
	    GTK_STOCK_CANCEL,
	    GTK_RESPONSE_CANCEL);
    
    cedialog->ok_button = gtk_dialog_add_button(
	    GTK_DIALOG(cedialog),
	    GTK_STOCK_OK,
	    GTK_RESPONSE_OK);
    
    gtk_widget_grab_default(cedialog->ok_button);
}

GtkWidget *cfa_cond_editor_dialog_new(GtkWidget *parent, const gchar *title, GList *cond_list)
{
    CfaCondEditorDialog *cedialog;
    
    cedialog = g_object_new(CFA_TYPE_COND_EDITOR_DIALOG, NULL);
    
    gtk_window_set_title(GTK_WINDOW(cedialog), title);
    gtk_window_set_transient_for(GTK_WINDOW(cedialog), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(cedialog), TRUE);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(cedialog), TRUE);
    
    cfa_cond_editor_set_cond_list(CFA_COND_EDITOR(cedialog->cond_editor), cond_list);
    
    return GTK_WIDGET(cedialog);
}

GList *cfa_cond_editor_dialog_get_cond_list(CfaCondEditorDialog *cedialog)
{
    return cfa_cond_editor_get_cond_list(CFA_COND_EDITOR(cedialog->cond_editor));
}
