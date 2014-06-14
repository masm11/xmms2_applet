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
 * $Id: cfapreferences.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_PREFERENCES_H__INCLUDED
#define CFA_PREFERENCES_H__INCLUDED

#include <common.h>
#include <glib-object.h>
#include <gdk/gdk.h>
#include "cfacond.h"

G_BEGIN_DECLS

#define	CFA_TYPE_PREFERENCES			cfa_preferences_get_type()
#define CFA_PREFERENCES(preferences)		G_TYPE_CHECK_INSTANCE_CAST((preferences), CFA_TYPE_PREFERENCES, CfaPreferences)
#define CFA_PREFERENCES_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_PREFERENCES, CfaPreferencesClass)
#define CFA_IS_PREFERENCES(preferences)		G_TYPE_CHECK_INSTANCE_TYPE((preferences), CFA_TYPE_PREFERENCES)
#define CFA_IS_PREFERENCES_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_PREFERENCES)
#define	CFA_PREFERENCES_GET_CLASS(preferences)	G_TYPE_INSTANCE_GET_CLASS((preferences), CFA_TYPE_PREFERENCES, CfaPreferencesClass)

#define CFA_TYPE_PREFERENCES_DESIGN_NAME	cfa_preferences_design_name_get_type()


typedef struct _CfaPreferences CfaPreferences;
typedef struct _CfaPreferencesClass CfaPreferencesClass;

typedef enum {
    CFA_DESIGN_FONT_NAME_AUTO,
    
    CFA_DESIGN_BG_COLOR_AUTO,
    CFA_DESIGN_FONT_COLOR_AUTO,
    CFA_DESIGN_ARC_COLOR_AUTO,
    CFA_DESIGN_HAND_COLOR_AUTO,
    
    CFA_DESIGN_FONT_NAME_MANUAL,
    
    CFA_DESIGN_BG_COLOR_MANUAL,
    CFA_DESIGN_FONT_COLOR_MANUAL,
    CFA_DESIGN_ARC_COLOR_MANUAL,
    CFA_DESIGN_HAND_COLOR_MANUAL,
    CFA_DESIGN_DRAG_COLOR_MANUAL,
    
    CFA_DESIGN_NR
} CfaPreferencesDesignName;

struct _CfaPreferences {
    GObject object;
    
    gchar *font_name_auto;
    
    GdkColor bg_color_auto;
    GdkColor font_color_auto;
    GdkColor arc_color_auto;
    GdkColor hand_color_auto;
    gchar *bg_color_name_auto;
    gchar *font_color_name_auto;
    gchar *arc_color_name_auto;
    gchar *hand_color_name_auto;
    
    gchar *font_name_manual;
    
    GdkColor bg_color_manual;
    GdkColor font_color_manual;
    GdkColor arc_color_manual;
    GdkColor hand_color_manual;
    GdkColor drag_color_manual;
    gchar *bg_color_name_manual;
    gchar *font_color_name_manual;
    gchar *arc_color_name_manual;
    gchar *hand_color_name_manual;
    gchar *drag_color_name_manual;
    
    GList *cond_list;
    
    gboolean smooth;
};

struct _CfaPreferencesClass {
    GObjectClass parent_class;
};

GType		cfa_preferences_get_type	(void) G_GNUC_CONST;

CfaPreferences	*cfa_preferences_new		(GdkColormap *cmap);

void cfa_preferences_set_font_name(CfaPreferences *prefs, CfaPreferencesDesignName type, const gchar *name);
void cfa_preferences_set_color_name(CfaPreferences *prefs, CfaPreferencesDesignName type, GdkColormap *cmap, const gchar *name);
void cfa_preferences_set_color(CfaPreferences *prefs, CfaPreferencesDesignName type, const GdkColor *color);
const gchar *cfa_preferences_get_font_name(CfaPreferences *prefs, CfaPreferencesDesignName type);
const gchar *cfa_preferences_get_color_name(CfaPreferences *prefs, CfaPreferencesDesignName type);
void cfa_preferences_get_color(CfaPreferences *prefs, CfaPreferencesDesignName type, GdkColor *color);

void cfa_preferences_add_cond(CfaPreferences *prefs, CfaCond *cond);
void cfa_preferences_set_cond_list(CfaPreferences *prefs, GList *cond_list);
GList *cfa_preferences_get_cond_list(CfaPreferences *prefs);

gboolean cfa_preferences_get_smooth(CfaPreferences *prefs);
void cfa_preferences_set_smooth(CfaPreferences *prefs, gboolean smooth);

G_END_DECLS

#endif /* ifndef CFA_PREFERENCES_H__INCLUDED */
