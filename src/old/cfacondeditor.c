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
 * $Id: cfacondeditor.c 287 2005-06-25 12:03:54Z masm $
 */

#include <common.h>
#include <gtk/gtk.h>
#include "cfacondeditor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

G_DEFINE_TYPE(CfaCondEditor, cfa_cond_editor, GTK_TYPE_HBOX);

static GtkWidget *make_list(CfaCondEditor *editor);
static GtkWidget *make_buttons(CfaCondEditor *editor);
static void	cfa_cond_editor_destroy			(GtkObject *object);
static void	cfa_cond_editor_finalize		(GObject *object);
static void	cfa_cond_editor_realize			(GtkWidget *widget);
static void	cfa_cond_editor_unrealize		(GtkWidget *widget);

static void cfa_cond_editor_new_cb(GtkButton *widget, gpointer user_data);
static void cfa_cond_editor_delete_cb(GtkButton *widget, gpointer user_data);

static void	cfa_cond_editor_cell_toggled_cb		(GtkCellRendererToggle *cell,
							 gchar *path_string,
							 gpointer user_data);
static void	cfa_cond_editor_cell_text_edited_cb	(GtkCellRendererToggle *cell,
							 gchar *path_string,
							 gchar *text,
							 gpointer user_data);
static void	cfa_cond_editor_cell_bool_edited_cb	(GtkCellRendererToggle *cell,
							 gchar *path_string,
							 gchar *text,
							 gpointer user_data);
static void	cfa_cond_editor_cell_int_edited_cb	(GtkCellRendererToggle *cell,
							 gchar *path_string,
							 gchar *text,
							 gpointer user_data);

enum {
    COL_USE_CPU,
    COL_CPU,
    COL_USE_AC,
    COL_AC,
    COL_USE_BATTERY,
    COL_BATTERY,
    COL_USE_LOAD,
    COL_LOAD,
    COL_USE_PROCESS,
    COL_PROCESS,
    COL_FREQ,
    COL_TRUE,
    COL_NR
};

static void cfa_cond_editor_class_init(CfaCondEditorClass *klass)
{
    GObjectClass *gobject_class;
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    gobject_class = G_OBJECT_CLASS(klass);
    object_class = GTK_OBJECT_CLASS(klass);
    widget_class = GTK_WIDGET_CLASS(klass);
    
    object_class->destroy = cfa_cond_editor_destroy;
    gobject_class->finalize = cfa_cond_editor_finalize;
    
    widget_class->realize = cfa_cond_editor_realize;
    widget_class->unrealize = cfa_cond_editor_unrealize;
}

static void cfa_cond_editor_init(CfaCondEditor *editor)
{
    GtkWidget *w;
    
    gtk_box_set_spacing(GTK_BOX(editor), 5);
    
    gtk_widget_push_composite_child();
    
    w = make_list(editor);
    gtk_widget_show_all(w);
    gtk_box_pack_start(GTK_BOX(editor), w, TRUE, TRUE, 0);
    
    w = make_buttons(editor);
    gtk_widget_show_all(w);
    gtk_box_pack_start(GTK_BOX(editor), w, FALSE, FALSE, 0);
    
    gtk_widget_pop_composite_child();
}

static GtkWidget *make_list(CfaCondEditor *editor)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    editor->store = gtk_list_store_new(COL_NR,
	    G_TYPE_BOOLEAN, G_TYPE_INT,		// cpu
	    G_TYPE_BOOLEAN, G_TYPE_BOOLEAN,	// ac
	    G_TYPE_BOOLEAN, G_TYPE_STRING,	// battery
	    G_TYPE_BOOLEAN, G_TYPE_STRING,	// load
	    G_TYPE_BOOLEAN, G_TYPE_STRING,	// process
	    G_TYPE_STRING,			// freq
	    G_TYPE_BOOLEAN);			// true
    
    editor->view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(editor->store));
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(editor->view), TRUE);
    
    renderer = gtk_cell_renderer_toggle_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_USE_CPU));
    g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(cfa_cond_editor_cell_toggled_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    "", renderer,
	    "active", COL_USE_CPU,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_text_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_CPU));
    g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cfa_cond_editor_cell_int_edited_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    _("CPU"), renderer,
	    "editable", COL_USE_CPU,
	    "text", COL_CPU,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_toggle_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_USE_AC));
    g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(cfa_cond_editor_cell_toggled_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    "", renderer,
	    "active", COL_USE_AC,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_text_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_AC));
    g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cfa_cond_editor_cell_bool_edited_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    _("AC"), renderer,
	    "editable", COL_USE_AC,
	    "text", COL_AC,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_toggle_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_USE_BATTERY));
    g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(cfa_cond_editor_cell_toggled_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    "", renderer,
	    "active", COL_USE_BATTERY,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_text_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_BATTERY));
    g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cfa_cond_editor_cell_text_edited_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    _("Battery"), renderer,
	    "editable", COL_USE_BATTERY,
	    "text", COL_BATTERY,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_toggle_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_USE_LOAD));
    g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(cfa_cond_editor_cell_toggled_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    "", renderer,
	    "active", COL_USE_LOAD,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_text_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_LOAD));
    g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cfa_cond_editor_cell_text_edited_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    _("Load"), renderer,
	    "editable", COL_USE_LOAD,
	    "text", COL_LOAD,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_toggle_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_USE_PROCESS));
    g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(cfa_cond_editor_cell_toggled_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    "", renderer,
	    "active", COL_USE_PROCESS,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_text_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_PROCESS));
    g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cfa_cond_editor_cell_text_edited_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    _("Process"), renderer,
	    "editable", COL_USE_PROCESS,
	    "text", COL_PROCESS,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    renderer = gtk_cell_renderer_text_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(COL_FREQ));
    g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cfa_cond_editor_cell_text_edited_cb), editor);
    column = gtk_tree_view_column_new_with_attributes(
	    _("Frequency"), renderer,
	    "editable", COL_TRUE,
	    "text", COL_FREQ,
	    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->view), column);
    
    return editor->view;
}

static GtkWidget *make_buttons(CfaCondEditor *editor)
{
    GtkWidget *vbox, *w;
    
    vbox = gtk_vbox_new(FALSE, 5);
    
    w = gtk_button_new_with_label(_("New"));
    gtk_box_pack_start(GTK_BOX(vbox), w, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(cfa_cond_editor_new_cb), editor);
    
    w = gtk_button_new_with_label(_("Delete"));
    gtk_box_pack_start(GTK_BOX(vbox), w, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(cfa_cond_editor_delete_cb), editor);
    
    return vbox;
}

static void cfa_cond_editor_destroy(GtkObject *object)
{
    CfaCondEditor *editor = CFA_COND_EDITOR(object);
    
    if (editor->view != NULL) {
	gtk_widget_destroy(editor->view);
	editor->view = NULL;
    }
    
    if (editor->store != NULL) {
	g_object_unref(G_OBJECT(editor->store));
	editor->store = NULL;
    }
    
    GTK_OBJECT_CLASS(cfa_cond_editor_parent_class)->destroy(object);
}

static void cfa_cond_editor_finalize(GObject *object)
{
    G_OBJECT_CLASS(cfa_cond_editor_parent_class)->finalize(object);
}

static void cfa_cond_editor_realize(GtkWidget *widget)
{
    GTK_WIDGET_CLASS(cfa_cond_editor_parent_class)->realize(widget);
}

static void cfa_cond_editor_unrealize(GtkWidget *widget)
{
    GTK_WIDGET_CLASS(cfa_cond_editor_parent_class)->unrealize(widget);
}

static void cfa_cond_editor_new_cb(GtkButton *widget, gpointer user_data)
{
    CfaCondEditor *editor = user_data;
    GtkTreeSelection *sel;
    GtkTreeIter iter;
    
    gtk_list_store_append(editor->store, &iter);
    
    gtk_list_store_set(editor->store, &iter,
	    COL_USE_CPU, FALSE,
	    COL_CPU, 0,
	    COL_USE_AC, FALSE,
	    COL_AC, FALSE,
	    COL_USE_BATTERY, FALSE,
	    COL_BATTERY, "0-100",
	    COL_USE_LOAD, FALSE,
	    COL_LOAD, "0-100",
	    COL_USE_PROCESS, FALSE,
	    COL_PROCESS, "",
	    COL_FREQ, "1G",
	    COL_TRUE, TRUE,
	    -1);
    
    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(editor->view));
    gtk_tree_selection_select_iter(sel, &iter);
}

static void cfa_cond_editor_delete_cb(GtkButton *widget, gpointer user_data)
{
    CfaCondEditor *editor = user_data;
    GtkTreeSelection *sel;
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(editor->view));
    if (gtk_tree_selection_get_selected(sel, &model, &iter))
	gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
}

static void cfa_cond_editor_cell_toggled_cb(
	GtkCellRendererToggle *cell, gchar *path_string, gpointer user_data)
{
    CfaCondEditor *editor = user_data;
    GtkTreeModel *model = GTK_TREE_MODEL(editor->store);
    GtkTreeIter iter;
    gint col;
    gboolean value;
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    
    col = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    gtk_tree_model_get(model, &iter, col, &value, -1);
    gtk_list_store_set(editor->store, &iter, col, !value, -1);
}

static void cfa_cond_editor_cell_text_edited_cb(
	GtkCellRendererToggle *cell, gchar *path_string, gchar *text, gpointer user_data)
{
    CfaCondEditor *editor = user_data;
    GtkTreeModel *model = GTK_TREE_MODEL(editor->store);
    GtkTreeIter iter;
    gint col;
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    
    col = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    gtk_list_store_set(editor->store, &iter, col, text, -1);
}

static void cfa_cond_editor_cell_bool_edited_cb(
	GtkCellRendererToggle *cell, gchar *path_string, gchar *text, gpointer user_data)
{
    CfaCondEditor *editor = user_data;
    GtkTreeModel *model = GTK_TREE_MODEL(editor->store);
    GtkTreeIter iter;
    gint col;
    gboolean val;
    
    if (g_strcasecmp(text, "true") == 0)
	val = TRUE;
    else if (g_strcasecmp(text, "on") == 0)
	val = TRUE;
    else if (g_strcasecmp(text, "false") == 0)
	val = FALSE;
    else if (g_strcasecmp(text, "off") == 0)
	val = FALSE;
    else
	return;
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    
    col = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    gtk_list_store_set(editor->store, &iter, col, val, -1);
}

static void cfa_cond_editor_cell_int_edited_cb(
	GtkCellRendererToggle *cell, gchar *path_string, gchar *text, gpointer user_data)
{
    CfaCondEditor *editor = user_data;
    GtkTreeModel *model = GTK_TREE_MODEL(editor->store);
    GtkTreeIter iter;
    gint col;
    gchar *ep;
    gint val;
    
    val = strtol(text, &ep, 0);
    if (ep != NULL && *ep != '\0')
	return;
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    
    col = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    gtk_list_store_set(editor->store, &iter, col, val, -1);
}

void cfa_cond_editor_append_cond(CfaCondEditor *editor, CfaCond *cond)
{
    GtkTreeIter iter;
    
    gtk_list_store_append(editor->store, &iter);
    
    gtk_list_store_set(editor->store, &iter,
	    COL_USE_CPU, cfa_cond_get_use_cpu(cond),
	    COL_CPU, cfa_cond_get_cpu_id(cond),
	    COL_USE_AC, cfa_cond_get_use_ac(cond),
	    COL_AC, cfa_cond_get_ac(cond),
	    COL_USE_BATTERY, cfa_cond_get_use_battery(cond),
	    COL_BATTERY, cfa_cond_get_battery(cond),
	    COL_USE_LOAD, cfa_cond_get_use_load(cond),
	    COL_LOAD, cfa_cond_get_load(cond),
	    COL_USE_PROCESS, cfa_cond_get_use_process(cond),
	    COL_PROCESS, cfa_cond_get_process(cond),
	    COL_FREQ, cfa_cond_get_freq(cond),
	    COL_TRUE, TRUE,
	    -1);
}

GtkWidget *cfa_cond_editor_new(void)
{
    CfaCondEditor *editor;
    
    editor = g_object_new(CFA_TYPE_COND_EDITOR, NULL);
    
    return GTK_WIDGET(editor);
}

void cfa_cond_editor_set_cond_list(CfaCondEditor *editor, GList *cond_list)
{
    GList *lp;
    for (lp = cond_list; lp != NULL; lp = g_list_next(lp)) {
	CfaCond *cond = lp->data;
	cfa_cond_editor_append_cond(editor, cond);
    }
}

GList *cfa_cond_editor_get_cond_list(CfaCondEditor *editor)
{
    GtkTreeIter iter;
    GtkTreeModel *model = GTK_TREE_MODEL(editor->store);
    GList *list = NULL;
    
    if (gtk_tree_model_get_iter_first(model, &iter)) {
	do {
	    CfaCond *cond = cfa_cond_new();
	    gboolean use_cpu;
	    gint cpu_id;
	    gboolean use_ac;
	    gboolean ac;
	    gboolean use_battery;
	    gchar *battery;
	    gboolean use_load;
	    gchar *load;
	    gboolean use_process;
	    gchar *process;
	    gchar *freq;
	    
	    gtk_tree_model_get(model, &iter,
		    COL_USE_CPU, &use_cpu,
		    COL_CPU, &cpu_id,
		    COL_USE_AC, &use_ac,
		    COL_AC, &ac,
		    COL_USE_BATTERY, &use_battery,
		    COL_BATTERY, &battery,
		    COL_USE_LOAD, &use_load,
		    COL_LOAD, &load,
		    COL_USE_PROCESS, &use_process,
		    COL_PROCESS, &process,
		    COL_FREQ, &freq,
		    -1);
	    
	    cfa_cond_set_use_cpu(cond, use_cpu);
	    cfa_cond_set_cpu_id(cond, cpu_id);
	    cfa_cond_set_use_ac(cond, use_ac);
	    cfa_cond_set_ac(cond, ac);
	    cfa_cond_set_use_battery(cond, use_battery);
	    cfa_cond_set_battery(cond, battery);
	    cfa_cond_set_use_load(cond, use_load);
	    cfa_cond_set_load(cond, load);
	    cfa_cond_set_use_process(cond, use_process);
	    cfa_cond_set_process(cond, process);
	    cfa_cond_set_freq(cond, freq);
	    
	    g_free(battery);
	    g_free(load);
	    g_free(process);
	    g_free(freq);
	    
	    list = g_list_append(list, cond);
	} while (gtk_tree_model_iter_next(model, &iter));
    }
    
    return list;
}

void cfa_cond_editor_test(CfaCondEditor *editor)
{
    CfaCond *cond;
    
    cond = cfa_cond_new();
    
    cfa_cond_set_use_cpu(cond, TRUE);
    cfa_cond_set_cpu_id(cond, 1);
    cfa_cond_set_use_ac(cond, TRUE);
    cfa_cond_set_ac(cond, TRUE);
    cfa_cond_set_use_battery(cond, TRUE);
    cfa_cond_set_battery(cond, "0-30");
    cfa_cond_set_use_load(cond, TRUE);
    cfa_cond_set_load(cond, "50-100");
    cfa_cond_set_use_process(cond, TRUE);
    cfa_cond_set_process(cond, "make");
    
    cfa_cond_set_freq(cond, "1.2G");
    cfa_cond_editor_append_cond(editor, cond);
    
#if 1
    cfa_cond_set_freq(cond, "600M");
    cfa_cond_editor_append_cond(editor, cond);
#endif
    
    g_object_unref(cond);
}

/*EOF*/
