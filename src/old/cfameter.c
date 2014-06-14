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
 * $Id: cfameter.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <gtk/gtkcontainer.h>
#include <glib/gprintf.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "cfamarshal.h"
#include "cfameter.h"

G_DEFINE_TYPE(CfaMeter, cfa_meter, GTK_TYPE_WIDGET);

enum {
    SIG_CHANGED,
    SIG_NR
};

static void cfa_meter_realize      (GtkWidget    *widget);
static void cfa_meter_destroy	(GtkObject *object);
static void cfa_meter_finalize	(GObject *gobject);

static void design_changed_cb(CfaPreferences *prefs, CfaPreferencesDesignName type, gpointer user_data);
static void conn_status_changed_cb(CfaFlag *flag, gpointer user_data);

static void moving_setup(CfaMeter *meter);
static gboolean moving_timeout_handler(gpointer data);

static void connect_signal_with_prefs(CfaMeter *meter);
static void disconnect_signal_with_prefs(CfaMeter *meter);
static void connect_signal_with_conn_status(CfaMeter *meter);
static void disconnect_signal_with_conn_status(CfaMeter *meter);

static gboolean cfa_meter_button_press_event(GtkWidget *widget, GdkEventButton *event);
static gboolean cfa_meter_motion_notify_event(GtkWidget *widget, GdkEventMotion *event);
static gboolean cfa_meter_button_release_event(GtkWidget *widget, GdkEventButton *event);
static gboolean cfa_meter_expose_event(GtkWidget *widget, GdkEventExpose *event);
static void update_manual_freq(CfaMeter *meter, gint dx, gint dy);
static gboolean stop_drag_hand(gpointer data);
static void cfa_meter_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void cfa_meter_size_allocate(GtkWidget *widget, GtkAllocation *allocation);

static gint angle_freq_to_dir(CfaMeter *meter, freq_t cur);
static freq_t angle_dxy_to_freq(CfaMeter *meter, gint dx, gint dy);

static void create(CfaMeter *meter);
static void create_bg(CfaMeter *meter);
static void create_arc(CfaMeter *meter);
static void create_hand(CfaMeter *meter);
static void create_drag(CfaMeter *meter);
static void create_text(CfaMeter *meter);
static void create_freq_text(CfaMeter *meter, struct meter_text_t *text,
	int cx, int cy, glong hz);
static void draw(CfaMeter *meter);
static void draw_bg(CfaMeter *meter);
static void draw_arc(CfaMeter *meter);
static void draw_hand(CfaMeter *meter);
static void draw_drag(CfaMeter *meter);
static void draw_text(CfaMeter *meter);
static void text_create(CfaMeter *meter, struct meter_text_t *text,
	int cx, int cy, char *str);
static void text_draw(CfaMeter *meter, struct meter_text_t *text);

static GtkWidgetClass *parent_class = NULL;

static guint signals[SIG_NR] = { 0 };

#define CFA_DESIGN_FONT_NAME(meter)	((meter)->is_auto ? CFA_DESIGN_FONT_NAME_AUTO :  CFA_DESIGN_FONT_NAME_MANUAL)
#define CFA_DESIGN_BG_COLOR(meter)	((meter)->is_auto ? CFA_DESIGN_BG_COLOR_AUTO :  CFA_DESIGN_BG_COLOR_MANUAL)
#define CFA_DESIGN_FONT_COLOR(meter)	((meter)->is_auto ? CFA_DESIGN_FONT_COLOR_AUTO :  CFA_DESIGN_FONT_COLOR_MANUAL)
#define CFA_DESIGN_ARC_COLOR(meter)	((meter)->is_auto ? CFA_DESIGN_ARC_COLOR_AUTO :  CFA_DESIGN_ARC_COLOR_MANUAL)
#define CFA_DESIGN_HAND_COLOR(meter)	((meter)->is_auto ? CFA_DESIGN_HAND_COLOR_AUTO :  CFA_DESIGN_HAND_COLOR_MANUAL)

/*
 * 手動の外部操作:
 *   resize
 *   set_vert
 *   set_color
 *   set_font
 *   set_arc_width
 *   set_hand_width
 *   set_cpu_id(gint id, gboolean show_id)
 * 
 * タイマで自動で内部:
 *   set_freq ←タイマ
 *   set_freqs 設定できる周波数をセット。
 * 
 * タイマ:
 *   針を動かす時は 100ms くらいで。
 *   静かな時は 500ms くらいで。
 * 
 * resize --+--> text_left.x, y
 *          +--> text_right.x, y
 *          +--> text_center.x, y
 *          +--> center_x,y
 *          +--> arc_radius
 * set_vert
 * set_color
 * set_font
 * set_arc_width
 * set_hand_width
 * set_cpu_id
 * set_range --+--> text_left.layout
 *             +--> text_right.layout
 * set_cur --+--> text_center.layout
 *           +--> text_center.hand
 * 
 * テキストの表示は、位置をキャッシュしない。
 * 中央の位置だけ持つ。
 * 
 * hand,
 * hand_tmp
 * 
 * act_hand_color_auto
 * act_hand_color_manual
 * tmp_hand_color
 * font
 * arc_width
 * hand_width
 * is_vert
 * 
 * 色:
 *   円弧
 *   実周波数針(auto時)
 *   実周波数針(manual時)
 *   仮周波数針
 *   文字
 * 
 * 周波数の範囲が不明の場合(min=max=-1)
 *   arc だけ表示。他は表示しない。
 * 
 * smp: 1つのappで全CPUに対応。
 *   どの cpu を表示するかは設定で。システムモニタのように。
 *   左上隅に cpu id を表示。
 *   表示してる CPU が1つだけで、cpu#0 の場合、
 *   cpu id を表示しない。
 * 
 * 実周波数の針は補間してスムーズに動かす。
 * 
 */

static void cfa_meter_class_init(CfaMeterClass *class)
{
    GObjectClass *gobject_class;
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    
    parent_class = g_type_class_peek_parent(class);
    
    gobject_class = G_OBJECT_CLASS(class);
    object_class = GTK_OBJECT_CLASS(class);
    widget_class = GTK_WIDGET_CLASS(class);
    
    gobject_class->finalize = cfa_meter_finalize;
    
    object_class->destroy = cfa_meter_destroy;
    
    widget_class->realize = cfa_meter_realize;
    widget_class->size_request = cfa_meter_size_request;
    widget_class->size_allocate = cfa_meter_size_allocate;
    widget_class->expose_event = cfa_meter_expose_event;
    widget_class->button_press_event = cfa_meter_button_press_event;
    widget_class->motion_notify_event = cfa_meter_motion_notify_event;
    widget_class->button_release_event = cfa_meter_button_release_event;
    
    signals[SIG_CHANGED] = g_signal_new("changed",
	    G_OBJECT_CLASS_TYPE(object_class),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    cfa_cclosure_marshal_VOID__BOOLEAN_UINT64,
	    G_TYPE_NONE, 2, G_TYPE_BOOLEAN, G_TYPE_UINT64);
}

static void cfa_meter_init(CfaMeter *meter)
{
    meter->is_auto = TRUE;
    
    meter->freq_min = 0;
    meter->freq_max = 1000000000;
    meter->freq_cur = 100;
    
    meter->prefs = cfa_preferences_new(gtk_widget_get_colormap(GTK_WIDGET(meter)));
    meter->conn_status = cfa_flag_new();
    
    connect_signal_with_prefs(meter);
    connect_signal_with_conn_status(meter);
}

static void cfa_meter_realize(GtkWidget *widget)
{
    CfaMeter *meter;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GdkColor back;
    gulong event_mask;
    
    g_return_if_fail(CFA_IS_METER(widget));
    
    GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);
    meter = CFA_METER(widget);
    
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.visual = gtk_widget_get_visual(widget);
    attributes.colormap = gtk_widget_get_colormap(widget);
    event_mask = GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON1_MOTION_MASK;
    attributes.event_mask = gtk_widget_get_events(widget) | event_mask;
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    
    widget->window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
    gdk_window_set_user_data(widget->window, widget);
    
    widget->style = gtk_style_attach(widget->style, widget->window);
    
    back.red = back.green = back.blue = 0xffff * 0.6;
    gdk_rgb_find_color(gtk_widget_get_colormap(widget), &back);
    gdk_window_set_background(widget->window, &back);
    
    create(meter);
}

static void cfa_meter_destroy(GtkObject *object)
{
    CfaMeter *meter = CFA_METER(object);
    
    if (meter->prefs != NULL) {
	disconnect_signal_with_prefs(meter);
	g_object_unref(meter->prefs);
    }
    
    if (meter->conn_status != NULL) {
	disconnect_signal_with_conn_status(meter);
	g_object_unref(meter->conn_status);
    }
    
    (*GTK_OBJECT_CLASS(cfa_meter_parent_class)->destroy)(object);
}

static void cfa_meter_finalize(GObject *gobject)
{
    CfaMeter *meter = CFA_METER(gobject);
    
    if (meter->freq_avail != NULL) {
	g_free(meter->freq_avail);
	meter->freq_avail = NULL;
    }
    
    (*G_OBJECT_CLASS(cfa_meter_parent_class)->finalize)(gobject);
}

GtkWidget* cfa_meter_new(void)
{
    CfaMeter *meter;
    
    meter = g_object_new(CFA_TYPE_METER, NULL);
    
    return GTK_WIDGET(meter);
}

void cfa_meter_set_prefs(CfaMeter *meter, CfaPreferences *prefs)
{
    if (meter->prefs != NULL) {
	disconnect_signal_with_prefs(meter);
	g_object_unref(meter->prefs);
    }
    
    meter->prefs = prefs;
    g_object_ref(meter->prefs);
    connect_signal_with_prefs(meter);
}

void cfa_meter_set_conn_status(CfaMeter *meter, CfaFlag *conn_status)
{
    if (meter->conn_status != NULL) {
	disconnect_signal_with_conn_status(meter);
	g_object_unref(meter->conn_status);
    }
    
    meter->conn_status = conn_status;
    g_object_ref(meter->conn_status);
    connect_signal_with_conn_status(meter);
}

void cfa_meter_set_vert(CfaMeter *meter, gboolean is_vert)
{
    meter->is_vert = is_vert;
    create(meter);
}

void cfa_meter_set_cpu_id(CfaMeter *meter, gint cpu_id)
{
    meter->cpu_id = cpu_id;
    gtk_widget_queue_draw(&meter->widget);
}

void cfa_meter_set_show_cpu_id(CfaMeter *meter, gboolean show)
{
    meter->show_cpu_id = show;
    gtk_widget_queue_draw(&meter->widget);
}

void cfa_meter_panel_size_changed(CfaMeter *meter)
{
}

static void connect_signal_with_prefs(CfaMeter *meter)
{
    meter->prefs_sigs = g_signal_connect(meter->prefs, "design-changed", G_CALLBACK(design_changed_cb), meter);
}

static void disconnect_signal_with_prefs(CfaMeter *meter)
{
    g_signal_handler_disconnect(meter->prefs, meter->prefs_sigs);
}

static void connect_signal_with_conn_status(CfaMeter *meter)
{
    meter->conn_status_sigs = g_signal_connect(meter->conn_status, "changed", G_CALLBACK(conn_status_changed_cb), meter);
}

static void disconnect_signal_with_conn_status(CfaMeter *meter)
{
    g_signal_handler_disconnect(meter->conn_status, meter->conn_status_sigs);
}

static void design_changed_cb(CfaPreferences *prefs, CfaPreferencesDesignName type, gpointer user_data)
{
    CfaMeter *meter = user_data;
    xlog_debug(1, "cfameter: design_changed_cb: %p,%d,%p", prefs, type, user_data);
    create(meter);
}

static void conn_status_changed_cb(CfaFlag *flag, gpointer user_data)
{
    CfaMeter *meter = user_data;
    create(meter);
}

static void moving_setup(CfaMeter *meter)
{
    if (meter->timeout_id_moving != 0)
	return;
    
    meter->timeout_id_moving = g_timeout_add(50, moving_timeout_handler, meter);
}

static gboolean moving_timeout_handler(gpointer data)
{
    CfaMeter *meter = data;
    
    if (meter->dir_cur != meter->dir_goto) {
	if (cfa_preferences_get_smooth(meter->prefs)) {
	    glong dif = (meter->dir_goto - meter->dir_cur) / 4;
	    if (dif != 0)
		meter->dir_cur += dif;
	    else
		meter->dir_cur = meter->dir_goto;
	} else {
	    meter->dir_cur = meter->dir_goto;
	}
	gtk_widget_queue_draw(&meter->widget);
    }
    
    if (meter->dir_cur != meter->dir_goto)
	return TRUE;
    meter->timeout_id_moving = 0;
    return FALSE;
}

// static void cfa_meter_set_freq_avails(CfaMeter *meter, ...);

static gboolean cfa_meter_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    draw(CFA_METER(widget));
    
    return FALSE;
}

static gboolean cfa_meter_button_press_event(GtkWidget *widget, GdkEventButton *event)
{
    CfaMeter *meter = CFA_METER(widget);
    xlog_debug(1, "cfa_meter_button_press_event: event->type=%d", event->type);
    if (event->button == 1 && event->type == GDK_BUTTON_PRESS) {
	xlog_debug(1, "cfa_meter_button_press_event: is single.");
	if (!meter->is_auto) {
	    meter->is_dragging = TRUE;
	    meter->show_drag_hand = TRUE;
	    
	    update_manual_freq(meter, event->x - meter->center_x, event->y - meter->center_y);
	    
	    gtk_widget_queue_draw(&meter->widget);
	}
    } else if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) {
	xlog_debug(1, "cfa_meter_button_press_event: is double-click");
	meter->is_auto = !meter->is_auto;
	meter->manual_freq = meter->freq_cur;
	create(meter);
	g_signal_emit(meter, signals[SIG_CHANGED], 0, meter->is_auto, meter->manual_freq);
    }
    
    return FALSE;
}

static gboolean cfa_meter_motion_notify_event(GtkWidget *widget, GdkEventMotion *event)
{
    CfaMeter *meter = CFA_METER(widget);
    
    if (meter->is_dragging && event->type == GDK_MOTION_NOTIFY) {
	xlog_debug(1, "cfa_meter_motion_notify_event: %f, %f", event->x, event->y);
	
	update_manual_freq(meter, event->x - meter->center_x, event->y - meter->center_y);
	
	gtk_widget_queue_draw(&meter->widget);
    }
    
    return FALSE;
}

static gboolean cfa_meter_button_release_event(GtkWidget *widget, GdkEventButton *event)
{
    CfaMeter *meter = CFA_METER(widget);
    xlog_debug(1, "cfa_meter_button_release_event: event->type=%d", event->type);
    
    if (meter->is_dragging && event->button == 1 && event->type == GDK_BUTTON_RELEASE) {
	xlog_debug(1, "cfa_meter_button_release_event: stop drag.");
	update_manual_freq(meter, event->x - meter->center_x, event->y - meter->center_y);
	meter->is_dragging = FALSE;
	gtk_widget_queue_draw(&meter->widget);
	
	g_signal_emit(meter, signals[SIG_CHANGED], 0, meter->is_auto, meter->manual_freq);
	
	// ドラッグの針を消すのは、ここから1秒後。
	g_timeout_add(1000, stop_drag_hand, meter);	// fixme: remove もすべきだろ。
    }
    return FALSE;
}

static void update_manual_freq(CfaMeter *meter, gint dx, gint dy)
{
    freq_t freq;
    
    xlog_debug(1, "cfa_meter_motion_notify_event: %d, %d", dx, dy);
    
    freq = angle_dxy_to_freq(meter, dx, dy);
    xlog_debug(1, "cfa_meter_motion_notify_event: freq=%lld", freq);
    
    meter->manual_freq = freq;
    meter->dir_op = angle_freq_to_dir(meter, meter->manual_freq);
}

static gboolean stop_drag_hand(gpointer data)
{
    CfaMeter *meter = data;
    
    if (!meter->is_dragging && meter->show_drag_hand) {
	meter->show_drag_hand = FALSE;
	gtk_widget_queue_draw(&meter->widget);
    }
    
    return FALSE;
}

static void cfa_meter_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
    CfaMeter *meter = CFA_METER(widget);
    if (meter->is_vert) {
	requisition->width = widget->allocation.width;
	requisition->height = widget->allocation.width;
    } else {
	requisition->width = widget->allocation.height;
	requisition->height = widget->allocation.height;
    }
}

static void cfa_meter_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
    CfaMeter *meter = CFA_METER(widget);
    
    if (widget->allocation.width != allocation->width ||
	    widget->allocation.height != allocation->height ||
	    widget->allocation.x != allocation->x ||
	    widget->allocation.y != allocation->y) {
	widget->allocation = *allocation;
	
	create(meter);
	
	if (GTK_WIDGET_REALIZED(widget)) {
	    gdk_window_move_resize(widget->window,
		    allocation->x, allocation->y,
		    allocation->width, allocation->height);
	}
    }
}

void cfa_meter_set_freqs(CfaMeter *meter, freq_t min, freq_t max, freq_t cur, const freq_t *avail)
{
    xlog_debug(1, "cfa_meter_set_freqs: %lld,%lld,%lld.", min, max, cur);
    
    if (meter->freq_min != min ||
	    meter->freq_max != max ||
	    meter->freq_cur != cur) {
	gint i;
	freq_t *av;
	xlog_debug(1, "cfa_meter_set: new value.");
	
	for (i = 0; avail[i] != -1; i++)
	    /*NOP*/ ;
	av = g_new(freq_t, i + 1);
	for (i = 0; avail[i] != -1; i++)
	    av[i] = avail[i];
	av[i] = -1;
	
	if (meter->freq_avail != NULL)
	    g_free(meter->freq_avail);
	
	meter->freq_min = min;
	meter->freq_max = max;
	meter->freq_cur = cur;
	meter->freq_avail = av;
	
	create(meter);
	
	meter->dir_goto = angle_freq_to_dir(meter, cur);
	moving_setup(meter);
    }
}

static gint angle_freq_to_dir(CfaMeter *meter, freq_t cur)
{
    return (-30 - 210.0) * 64 * (cur - meter->freq_min) / (meter->freq_max - meter->freq_min) + 210 * 64;
}

static freq_t angle_dxy_to_freq(CfaMeter *meter, gint dx, gint dy)
{
    gint dir = atan2(-dy, dx) * 180 * 64 / M_PI;
    
    if (dir < -90 * 64)
	dir += 360 * 64;
    xlog_debug(1, "angle_dxy_to_freq: dx=%d, dy=%d.", dx, dy);
    xlog_debug(1, "angle_dxy_to_freq: dir=%d", dir);
    freq_t f = (meter->freq_max - meter->freq_min) * (dir - 210.0 * 64) / (-30 * 64 - 210 * 64) + meter->freq_min;
    xlog_debug(1, "angle_dxy_to_freq: f=%lld", f);
    if (f < meter->freq_min)
	f = meter->freq_min;
    if (f > meter->freq_max)
	f = meter->freq_max;
    
    if (meter->freq_avail != NULL && meter->freq_avail[0] != -1) {
	gint i, n;
	n = 0;
	for (i = 1; meter->freq_avail[i] != -1; i++) {
	    glong dif_i, dif_n;
	    dif_i = meter->freq_avail[i] - f;
	    dif_n = meter->freq_avail[n] - f;
	    if (dif_i < 0)
		dif_i = -dif_i;
	    if (dif_n < 0)
		dif_n = -dif_n;
	    if (dif_i < dif_n)
		n = i;
	}
	f = meter->freq_avail[n];
    }
    
    return f;
}

static void create(CfaMeter *meter)
{
    GtkWidget *widget = &meter->widget;
    meter->center_x = widget->allocation.width / 2;
    meter->center_y = widget->allocation.height / 2;
    if (meter->center_x >= meter->center_y)
	meter->arc_radius = meter->center_y * 0.9;
    else
	meter->arc_radius = meter->center_x * 0.9;
    
    create_bg(meter);
    create_arc(meter);
    create_hand(meter);
    create_drag(meter);
    create_text(meter);
    moving_setup(meter);
    
    gtk_widget_queue_draw(&meter->widget);
}

static void create_bg(CfaMeter *meter)
{
    GdkGC *gc;
    GdkColor color;
    
    cfa_preferences_get_color(meter->prefs, CFA_DESIGN_BG_COLOR(meter), &color);
    
    gc = gdk_gc_new(meter->widget.window);
    gdk_gc_set_foreground(gc, &color);
    gdk_gc_set_function(gc, GDK_COPY);
    
    gdk_gc_unref(meter->gc_bg);
    meter->gc_bg = gc;
}

static void create_arc(CfaMeter *meter)
{
    GdkGC *gc;
    GdkColor color;
    
    cfa_preferences_get_color(meter->prefs, CFA_DESIGN_ARC_COLOR(meter), &color);
    
    gc = gdk_gc_new(meter->widget.window);
    gdk_gc_set_foreground(gc, &color);
    gdk_gc_set_function(gc, GDK_COPY);
    gdk_gc_set_line_attributes(gc, 2, GDK_LINE_SOLID, GDK_CAP_PROJECTING, GDK_JOIN_MITER);
    
    gdk_gc_unref(meter->gc_arc);
    meter->gc_arc = gc;
}

static void create_hand(CfaMeter *meter)
{
    GdkGC *gc;
    GdkColor color;
    
    cfa_preferences_get_color(meter->prefs, CFA_DESIGN_HAND_COLOR(meter), &color);
    
    gc = gdk_gc_new(meter->widget.window);
    gdk_gc_set_foreground(gc, &color);
    gdk_gc_set_function(gc, GDK_COPY);
    gdk_gc_set_line_attributes(gc, 2, GDK_LINE_SOLID, GDK_CAP_PROJECTING, GDK_JOIN_MITER);
    
    if (meter->gc_hand != NULL)
	gdk_gc_unref(meter->gc_hand);
    meter->gc_hand = gc;
}

static void create_drag(CfaMeter *meter)
{
    GdkGC *gc;
    GdkColor color;
    
    cfa_preferences_get_color(meter->prefs, CFA_DESIGN_DRAG_COLOR_MANUAL, &color);
    
    gc = gdk_gc_new(meter->widget.window);
    gdk_gc_set_foreground(gc, &color);
    gdk_gc_set_function(gc, GDK_COPY);
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID, GDK_CAP_PROJECTING, GDK_JOIN_MITER);
    
    if (meter->gc_drag != NULL)
	gdk_gc_unref(meter->gc_drag);
    meter->gc_drag = gc;
}

static void create_text(CfaMeter *meter)
{
    GdkGC *gc;
    PangoFontDescription *pfd;
    GdkColor color;
    
    cfa_preferences_get_color(meter->prefs, CFA_DESIGN_FONT_COLOR(meter), &color);
    
    gc = gdk_gc_new(meter->widget.window);
    gdk_gc_set_foreground(gc, &color);
    gdk_gc_set_function(gc, GDK_COPY);
    
    if (meter->gc_text != NULL)
	gdk_gc_unref(meter->gc_text);
    meter->gc_text = gc;
    
    pfd = pango_font_description_from_string(cfa_preferences_get_font_name(meter->prefs, CFA_DESIGN_FONT_NAME(meter)));
    gtk_widget_modify_font(&meter->widget, pfd);
    pango_font_description_free(pfd);
    
    create_freq_text(meter, &meter->text_left,
	    meter->center_x + meter->arc_radius * cos(210 * M_PI / 180),
	    meter->center_y - meter->arc_radius * sin(210 * M_PI / 180) + meter->arc_radius / 4,
	    meter->freq_min);
    create_freq_text(meter, &meter->text_right,
	    meter->center_x + meter->arc_radius * cos(-30 * M_PI / 180),
	    meter->center_y - meter->arc_radius * sin(-30 * M_PI / 180) + meter->arc_radius / 4,
	    meter->freq_max);
    create_freq_text(meter, &meter->text_center,
	    meter->center_x,
	    meter->center_y + meter->arc_radius * 0.4,
	    meter->freq_cur);
}

static void create_freq_text(CfaMeter *meter, struct meter_text_t *text,
	int cx, int cy, glong hz)
{
    char buf[128];
    char geta[8];
    gdouble num;
    
    if (hz < 0)
	hz = 0;
    
    if (hz < 1000) {
	strcpy(geta, "");
	num = hz;
    } else if (hz < 1000000) {
	strcpy(geta, "K");
	num = hz / 1000.0;
    } else if (hz < 1000000000) {
	strcpy(geta, "M");
	num = hz / 1000000.0;
    } else {
	strcpy(geta, "G");
	num = hz / 1000000000.0;
    }
    
    if (num == 0) {
	g_sprintf(buf, "%d%s", (gint) num, geta);
    } else if (num < 10.0) {
	g_sprintf(buf, "%.2f%s", num, geta);
    } else if (num < 100.0) {
	g_sprintf(buf, "%.1f%s", num, geta);
    } else {
	g_sprintf(buf, "%d%s", (gint) num, geta);
    }
    
    text_create(meter, text, cx, cy, buf);
}

static void draw(CfaMeter *meter)
{
    if (meter->widget.window != NULL) {
	gdk_window_clear(meter->widget.window);
	draw_bg(meter);
	draw_arc(meter);
	if (meter->conn_status != NULL && cfa_flag_get(meter->conn_status)) {
	    draw_text(meter);
	    draw_hand(meter);
	    if (meter->show_drag_hand)
		draw_drag(meter);
	}
    }
    
    if (meter->widget.allocation.width != meter->widget.allocation.height)
	gtk_widget_queue_resize(&meter->widget);
}

static void draw_bg(CfaMeter *meter)
{
    gdk_draw_rectangle(meter->widget.window,
	    meter->gc_bg, TRUE,
	    0,
	    0,
	    meter->widget.allocation.width,
	    meter->widget.allocation.height);
}

static void draw_arc(CfaMeter *meter)
{
    gdk_draw_arc(meter->widget.window,
	    meter->gc_arc, FALSE,
	    meter->center_x - meter->arc_radius,
	    meter->center_y - meter->arc_radius,
	    meter->arc_radius * 2,
	    meter->arc_radius * 2,
	    -30 * 64, (210 - -30) * 64);
}

static void draw_hand(CfaMeter *meter)
{
    gint dx, dy;
    
    dx =  meter->arc_radius * 1.05 * cos(meter->dir_cur * M_PI / 64 / 180);
    dy = -meter->arc_radius * 1.05 * sin(meter->dir_cur * M_PI / 64 / 180);
    
    gdk_draw_line(meter->widget.window,
	    meter->gc_hand, 
	    meter->center_x,
	    meter->center_y,
	    meter->center_x + dx,
	    meter->center_y + dy);
}

static void draw_drag(CfaMeter *meter)
{
    gint dx, dy;
    
    dx =  meter->arc_radius * 1.05 * cos(meter->dir_op * M_PI / 64 / 180);
    dy = -meter->arc_radius * 1.05 * sin(meter->dir_op * M_PI / 64 / 180);
    
    gdk_draw_line(meter->widget.window,
	    meter->gc_drag, 
	    meter->center_x,
	    meter->center_y,
	    meter->center_x + dx,
	    meter->center_y + dy);
}

static void draw_text(CfaMeter *meter)
{
    text_draw(meter, &meter->text_left);
    text_draw(meter, &meter->text_right);
    text_draw(meter, &meter->text_center);
}

static void text_create(CfaMeter *meter, struct meter_text_t *text,
	int cx, int cy, char *str)
{
    PangoLayout *layout;
    int tw, th;
    int x, y;
    
    layout = gtk_widget_create_pango_layout(&meter->widget, str);
    pango_layout_get_pixel_size(layout, &tw, &th);
    
    x = cx - tw / 2;
    y = cy - th / 2;
    
    if (x + tw > meter->widget.allocation.width)
	x = meter->widget.allocation.width - tw;
    if (y + th > meter->widget.allocation.height)
	y = meter->widget.allocation.height - th;
    
    if (x < 0)
	x = 0;
    if (y < 0)
	y = 0;
    xlog_debug(1, "text_create: %d %d %d %d %d %d", cx, cy, tw, th, x, y);
    
    if (text->layout != NULL)
	g_object_unref(text->layout);
    text->layout = layout;
    text->x = x;
    text->y = y;
}


static void text_draw(CfaMeter *meter, struct meter_text_t *text)
{
    gdk_draw_layout(meter->widget.window,
	    meter->gc_text,
	    text->x, text->y, text->layout);
}

/*EOF*/
