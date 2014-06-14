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
 * $Id: cfameter.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_METER_H__INCLUDED
#define CFA_METER_H__INCLUDED

#include <common.h>
#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>
#include "cfapreferences.h"
#include <cfaflag.h>

G_BEGIN_DECLS

#define CFA_TYPE_METER			(cfa_meter_get_type())
#define CFA_METER(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), CFA_TYPE_METER, CfaMeter))
#define CFA_METER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_METER, CfaMeterClass))
#define CFA_IS_METER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), CFA_TYPE_METER))
#define CFA_IS_METER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_METER))
#define CFA_METER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), CFA_TYPE_METER, CfaMeterClass))


typedef struct _CfaMeter	CfaMeter;
typedef struct _CfaMeterClass	CfaMeterClass;

struct meter_text_t {
    PangoLayout *layout;
    int x, y;
};

struct _CfaMeter {
    GtkWidget widget;
    
    CfaPreferences *prefs;
    gulong prefs_sigs;
    
    CfaFlag *conn_status;
    gulong conn_status_sigs;
    
    guint timeout_id_moving;
    
    gboolean is_vert;
    
    gint cpu_id;
    gboolean show_cpu_id;
    
    gboolean is_auto;
    freq_t manual_freq;
    gboolean is_dragging, show_drag_hand;
    
    /* minimum, maximum, and current frequencies (kHz) */
    freq_t freq_min, freq_max, freq_cur;
    /* available frequencies */
    freq_t *freq_avail;
    
    /* direction of hands (1/64 degree) */
    /* goal and current directions */
    glong dir_goto, dir_cur;
    /* direction of dragging hand */
    glong dir_op;
    
    gint center_x, center_y;
    gint arc_radius;
    
    /* width of lines */
    gint arc_width;
    gint hand_width;
    
    GdkGC *gc_bg;
    GdkGC *gc_arc;
    GdkGC *gc_hand;
    GdkGC *gc_drag;
    GdkGC *gc_text;
    struct meter_text_t text_left;
    struct meter_text_t text_right;
    struct meter_text_t text_center;
};

struct _CfaMeterClass {
    GtkWidgetClass parent_class;
};


GType		cfa_meter_get_type		(void) G_GNUC_CONST;

GtkWidget	*cfa_meter_new			(void);

void		cfa_meter_set_prefs		(CfaMeter *meter,
						 CfaPreferences *prefs);
void		cfa_meter_set_conn_status	(CfaMeter *meter,
						 CfaFlag *conn_status);

void		cfa_meter_panel_size_changed	(CfaMeter *meter);

void		cfa_meter_set_vert		(CfaMeter *meter,
						 gboolean is_vert);

void		cfa_meter_set_freqs		(CfaMeter *meter,
						 freq_t min,
						 freq_t max,
						 freq_t cur,
						 const freq_t *avail);

G_END_DECLS

#endif	/* ifndef CFA_METER_H__INCLUDED */
