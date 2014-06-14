/* CPUFreq Applet
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
 * $Id: cfapreferences.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include "cfapreferences.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <freqstr.h>
#include <sys/types.h>

G_DEFINE_TYPE(CfaPreferences, cfa_preferences, G_TYPE_OBJECT);

GType cfa_preferences_design_name_get_type(void)
{
    static GType etype = 0;
    if (etype == 0) {
	static const GEnumValue values[] = {
#include "cfapreftype.h"
	    { 0, NULL, NULL }
	};
	g_assert(sizeof values / sizeof values[0] == CFA_DESIGN_NR + 1);
	etype = g_enum_register_static ("CfaPreferencesDesignName", values);
    }
    return etype;
}


enum {
    SIG_DESIGN_CHANGED,
    SIG_COND_LIST_CHANGED,
    SIG_SMOOTH_CHANGED,
    SIG_NR
};

static void	cfa_preferences_dispose	(GObject *object);
static void	cfa_preferences_finalize(GObject *object);

static guint signals[SIG_NR] = { 0 };

static void cfa_preferences_class_init(CfaPreferencesClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->dispose = cfa_preferences_dispose;
    object_class->finalize = cfa_preferences_finalize;
    
    signals[SIG_DESIGN_CHANGED] = g_signal_new ("design-changed",
	    G_OBJECT_CLASS_TYPE(object_class),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__ENUM,
	    G_TYPE_NONE, 1, CFA_TYPE_PREFERENCES_DESIGN_NAME);
    
    signals[SIG_COND_LIST_CHANGED] = g_signal_new ("cond-list-changed",
	    G_OBJECT_CLASS_TYPE(object_class),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
    
    signals[SIG_SMOOTH_CHANGED] = g_signal_new ("smooth-changed",
	    G_OBJECT_CLASS_TYPE(object_class),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
}

static void cfa_preferences_init(CfaPreferences *preferences)
{
    xlog_debug(1, "cfa_preferences_init:");
}

static void cfa_preferences_dispose(GObject *object)
{
    CfaPreferences *prefs;
    
    xlog_debug(1, "cfa_preferences_dispose:");
    
    g_return_if_fail(CFA_IS_PREFERENCES(object));
    prefs = CFA_PREFERENCES(object);
    
    while (prefs->cond_list != NULL) {
	CfaCond *cond = prefs->cond_list->data;
	g_object_unref(cond);
	prefs->cond_list = g_list_delete_link(prefs->cond_list, prefs->cond_list);
    }
}

static void cfa_preferences_finalize(GObject *object)
{
    CfaPreferences *prefs;
    
    xlog_debug(1, "cfa_preferences_finalize:");
    
    g_return_if_fail(CFA_IS_PREFERENCES(object));
    prefs = CFA_PREFERENCES(object);
    
    if (prefs->font_name_auto != NULL) {
	g_free(prefs->font_name_auto);
	prefs->font_name_auto = NULL;
    }
    
    if (prefs->bg_color_name_auto != NULL) {
	g_free(prefs->bg_color_name_auto);
	prefs->bg_color_name_auto = NULL;
    }
    if (prefs->font_color_name_auto != NULL) {
	g_free(prefs->font_color_name_auto);
	prefs->font_color_name_auto = NULL;
    }
    if (prefs->arc_color_name_auto != NULL) {
	g_free(prefs->arc_color_name_auto);
	prefs->arc_color_name_auto = NULL;
    }
    if (prefs->hand_color_name_auto != NULL) {
	g_free(prefs->hand_color_name_auto);
	prefs->hand_color_name_auto = NULL;
    }
    if (prefs->bg_color_name_manual != NULL) {
	g_free(prefs->bg_color_name_manual);
	prefs->bg_color_name_manual = NULL;
    }
    if (prefs->font_color_name_manual != NULL) {
	g_free(prefs->font_color_name_manual);
	prefs->font_color_name_manual = NULL;
    }
    if (prefs->arc_color_name_manual != NULL) {
	g_free(prefs->arc_color_name_manual);
	prefs->arc_color_name_manual = NULL;
    }
    if (prefs->hand_color_name_manual != NULL) {
	g_free(prefs->hand_color_name_manual);
	prefs->hand_color_name_manual = NULL;
    }
    xlog_debug(1, "cfa_preferences_finalize: done.");
}

CfaPreferences *cfa_preferences_new(GdkColormap *cmap)
{
    CfaPreferences *prefs;
    
    prefs = g_object_new(CFA_TYPE_PREFERENCES, NULL);
    
    cfa_preferences_set_font_name(prefs, CFA_DESIGN_FONT_NAME_AUTO, "sans 8");
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_BG_COLOR_AUTO, cmap, NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_FONT_COLOR_AUTO, cmap, NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_ARC_COLOR_AUTO, cmap, NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_HAND_COLOR_AUTO, cmap, NULL);
    
    cfa_preferences_set_font_name(prefs, CFA_DESIGN_FONT_NAME_MANUAL, "sans 8");
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_BG_COLOR_MANUAL, cmap, NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_FONT_COLOR_MANUAL, cmap, NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_ARC_COLOR_MANUAL, cmap, NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_HAND_COLOR_MANUAL, cmap, NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_DRAG_COLOR_MANUAL, cmap, NULL);
    
    return prefs;
}

static gboolean color_from_string(GdkColormap *cmap, GdkColor *color, const gchar *str)
{
    guint r, g, b;
    
    if (str == NULL)
	return FALSE;
    
    if (sscanf(str, "%x:%x:%x", &r, &g, &b) != 3)
	return FALSE;
    
    color->red = r;
    color->green = g;
    color->blue = b;
    
    if (!gdk_colormap_alloc_color(cmap, color, FALSE, TRUE))
	return FALSE;
    
    return TRUE;
}

static gchar *color_to_string(const GdkColor *color)
{
    return g_strdup_printf("%04x:%04x:%04x",
	    color->red, color->green, color->blue);
}

void cfa_preferences_set_font_name(CfaPreferences *prefs, CfaPreferencesDesignName type, const gchar *name)
{
    if (name == NULL)
	name = "sans 8";
    
    switch (type) {
    case CFA_DESIGN_FONT_NAME_AUTO:
	if (prefs->font_name_auto != NULL)
	    g_free(prefs->font_name_auto);
	prefs->font_name_auto = g_strdup(name);
	break;
	
    case CFA_DESIGN_FONT_NAME_MANUAL:
	if (prefs->font_name_manual != NULL)
	    g_free(prefs->font_name_manual);
	prefs->font_name_manual = g_strdup(name);
	break;
	
    default:
	xlog_debug(1, "cfa_preferences_set_font_name: bad type: %d.", type);
	return;
    }
    
    xlog_debug(1, "cfa_preferences_set_font_name: %p,%d,%p", prefs, type, name);
    g_signal_emit(prefs, signals[SIG_DESIGN_CHANGED], 0, type);
}

void cfa_preferences_set_color_name(CfaPreferences *prefs, CfaPreferencesDesignName type, GdkColormap *cmap, const gchar *name)
{
    GdkColor color;
    
    if (!color_from_string(cmap, &color, name)) {
	switch (type) {
	case CFA_DESIGN_BG_COLOR_AUTO:
	case CFA_DESIGN_BG_COLOR_MANUAL:
	    color_from_string(cmap, &color, "0000:0000:0000");
	    break;
	    
	case CFA_DESIGN_FONT_COLOR_AUTO:
	case CFA_DESIGN_FONT_COLOR_MANUAL:
	    color_from_string(cmap, &color, "ffff:ffff:ffff");
	    break;
	    
	case CFA_DESIGN_ARC_COLOR_AUTO:
	case CFA_DESIGN_ARC_COLOR_MANUAL:
	    color_from_string(cmap, &color, "ffff:0000:0000");
	    break;
	    
	case CFA_DESIGN_HAND_COLOR_AUTO:
	    color_from_string(cmap, &color, "0000:ffff:0000");
	    break;
	    
	case CFA_DESIGN_HAND_COLOR_MANUAL:
	    color_from_string(cmap, &color, "0000:0000:ffff");
	    break;
	    
	case CFA_DESIGN_DRAG_COLOR_MANUAL:
	    color_from_string(cmap, &color, "ffff:ffff:ffff");
	    break;
	    
	default:
	    xlog_debug(1, "cfa_preferences_set_color_name: bad type: %d.", type);
	    return;
	}
    }
    
    cfa_preferences_set_color(prefs, type, &color);
}

void cfa_preferences_set_color(CfaPreferences *prefs, CfaPreferencesDesignName type, const GdkColor *color)
{
    switch (type) {
    case CFA_DESIGN_BG_COLOR_AUTO:
	prefs->bg_color_auto = *color;
	prefs->bg_color_name_auto = color_to_string(color);
	break;
	
    case CFA_DESIGN_FONT_COLOR_AUTO:
	prefs->font_color_auto = *color;
	prefs->font_color_name_auto = color_to_string(color);
	break;
	
    case CFA_DESIGN_ARC_COLOR_AUTO:
	prefs->arc_color_auto = *color;
	prefs->arc_color_name_auto = color_to_string(color);
	break;
	
    case CFA_DESIGN_HAND_COLOR_AUTO:
	prefs->hand_color_auto = *color;
	prefs->hand_color_name_auto = color_to_string(color);
	break;
	
    case CFA_DESIGN_BG_COLOR_MANUAL:
	prefs->bg_color_manual = *color;
	prefs->bg_color_name_manual = color_to_string(color);
	break;
	
    case CFA_DESIGN_FONT_COLOR_MANUAL:
	prefs->font_color_manual = *color;
	prefs->font_color_name_manual = color_to_string(color);
	break;
	
    case CFA_DESIGN_ARC_COLOR_MANUAL:
	prefs->arc_color_manual = *color;
	prefs->arc_color_name_manual = color_to_string(color);
	break;
	
    case CFA_DESIGN_HAND_COLOR_MANUAL:
	prefs->hand_color_manual = *color;
	prefs->hand_color_name_manual = color_to_string(color);
	break;
	
    case CFA_DESIGN_DRAG_COLOR_MANUAL:
	prefs->drag_color_manual = *color;
	prefs->drag_color_name_manual = color_to_string(color);
	break;
	
    default:
	xlog_debug(1, "cfa_preferences_set_color: bad type %d.", type);
	return;
    }
    
    g_signal_emit(prefs, signals[SIG_DESIGN_CHANGED], 0, type);
}

const gchar *cfa_preferences_get_font_name(CfaPreferences *prefs, CfaPreferencesDesignName type)
{
    switch (type) {
    case CFA_DESIGN_FONT_NAME_AUTO:
	return prefs->font_name_auto;
	
    case CFA_DESIGN_FONT_NAME_MANUAL:
	return prefs->font_name_manual;
	
    default:
	xlog_debug(1, "cfa_preferences_get_font_name: bad type %d.", type);
	return NULL;
    }
}

const gchar *cfa_preferences_get_color_name(CfaPreferences *prefs, CfaPreferencesDesignName type)
{
    switch (type) {
    case CFA_DESIGN_BG_COLOR_AUTO:
	return prefs->bg_color_name_auto;
	
    case CFA_DESIGN_FONT_COLOR_AUTO:
	return prefs->font_color_name_auto;
	
    case CFA_DESIGN_ARC_COLOR_AUTO:
	return prefs->arc_color_name_auto;
	
    case CFA_DESIGN_HAND_COLOR_AUTO:
	return prefs->hand_color_name_auto;
	
    case CFA_DESIGN_BG_COLOR_MANUAL:
	return prefs->bg_color_name_manual;
	
    case CFA_DESIGN_FONT_COLOR_MANUAL:
	return prefs->font_color_name_manual;
	
    case CFA_DESIGN_ARC_COLOR_MANUAL:
	return prefs->arc_color_name_manual;
	
    case CFA_DESIGN_HAND_COLOR_MANUAL:
	return prefs->hand_color_name_manual;
	
    case CFA_DESIGN_DRAG_COLOR_MANUAL:
	return prefs->drag_color_name_manual;
	
    default:
	xlog_debug(1, "cfa_preferences_get_color_name: bad type %d.", type);
	return NULL;
    }
}

void cfa_preferences_get_color(CfaPreferences *prefs, CfaPreferencesDesignName type, GdkColor *color)
{
    switch (type) {
    case CFA_DESIGN_BG_COLOR_AUTO:
	*color = prefs->bg_color_auto;
	break;
	
    case CFA_DESIGN_FONT_COLOR_AUTO:
	*color = prefs->font_color_auto;
	break;
	
    case CFA_DESIGN_ARC_COLOR_AUTO:
	*color = prefs->arc_color_auto;
	break;
	
    case CFA_DESIGN_HAND_COLOR_AUTO:
	*color = prefs->hand_color_auto;
	break;
	
    case CFA_DESIGN_BG_COLOR_MANUAL:
	*color = prefs->bg_color_manual;
	break;
	
    case CFA_DESIGN_FONT_COLOR_MANUAL:
	*color = prefs->font_color_manual;
	break;
	
    case CFA_DESIGN_ARC_COLOR_MANUAL:
	*color = prefs->arc_color_manual;
	break;
	
    case CFA_DESIGN_HAND_COLOR_MANUAL:
	*color = prefs->hand_color_manual;
	break;
	
    case CFA_DESIGN_DRAG_COLOR_MANUAL:
	*color = prefs->drag_color_manual;
	break;
	
    default:
	xlog_debug(1, "cfa_preferences_get_color: bad type %d.", type);
	color->red = 0;
	color->green = 0;
	color->blue = 0;
	break;
    }
}

void cfa_preferences_add_cond(CfaPreferences *prefs, CfaCond *cond)
{
    prefs->cond_list = g_list_append(prefs->cond_list, cond);
    
    g_signal_emit(prefs, signals[SIG_COND_LIST_CHANGED], 0);
}

void cfa_preferences_set_cond_list(CfaPreferences *prefs, GList *cond_list)
{
    while (prefs->cond_list != NULL) {
	CfaCond *cond = prefs->cond_list->data;
	g_object_unref(cond);
	prefs->cond_list = g_list_delete_link(prefs->cond_list, prefs->cond_list);
    }
    
    prefs->cond_list = cond_list;
    
    g_signal_emit(prefs, signals[SIG_COND_LIST_CHANGED], 0);
}

GList *cfa_preferences_get_cond_list(CfaPreferences *prefs)
{
    return prefs->cond_list;
}

gboolean cfa_preferences_get_smooth(CfaPreferences *prefs)
{
    return prefs->smooth;
}

void cfa_preferences_set_smooth(CfaPreferences *prefs, gboolean smooth)
{
    if (prefs->smooth != smooth) {
	prefs->smooth = smooth;
	g_signal_emit(prefs, signals[SIG_SMOOTH_CHANGED], 0);
    }
}

/*EOF*/
