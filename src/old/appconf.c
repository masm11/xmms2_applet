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
 * $Id: appconf.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <glib.h>
#include <panel-applet.h>
#include <panel-applet-gconf.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include "cfacond.h"
#include "cfapreferences.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "app.h"
#include "appconf.h"
// #include "i18n-support.h"

static void make_cond_key(gchar *buf, gint bufsiz, gint i, gchar *key)
{
    snprintf(buf, bufsiz, "cond%d_%s", i, key);
}

void appconf_load(struct app_t *app)
{
    PanelApplet *applet = app->applet;
    CfaPreferences *prefs;
    gchar *s;
    gint nconds, i;
    GdkColormap *cmap = gtk_widget_get_colormap(GTK_WIDGET(applet));
    
    prefs = cfa_preferences_new(cmap);
    
    s = panel_applet_gconf_get_string(applet, "font_auto", NULL);
    cfa_preferences_set_font_name(prefs, CFA_DESIGN_FONT_NAME_AUTO, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet,"bg_color_auto", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_BG_COLOR_AUTO, cmap, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet,"font_color_auto", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_FONT_COLOR_AUTO, cmap, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet, "arc_color_auto", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_ARC_COLOR_AUTO, cmap, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet, "hand_color_auto", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_HAND_COLOR_AUTO, cmap, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet, "font_manual", NULL);
    cfa_preferences_set_font_name(prefs, CFA_DESIGN_FONT_NAME_MANUAL, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet,"bg_color_manual", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_BG_COLOR_MANUAL, cmap, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet,"font_color_manual", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_FONT_COLOR_MANUAL, cmap, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet, "arc_color_manual", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_ARC_COLOR_MANUAL, cmap, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet, "hand_color_manual", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_HAND_COLOR_MANUAL, cmap, s);
    g_free(s);
    
    s = panel_applet_gconf_get_string(applet, "drag_color_manual", NULL);
    cfa_preferences_set_color_name(prefs, CFA_DESIGN_DRAG_COLOR_MANUAL, cmap, s);
    g_free(s);
    
    nconds = panel_applet_gconf_get_int(applet, "nconds", NULL);
    
    for (i = 0; i < nconds; i++) {
	gchar key[64];
	CfaCond *cond = cfa_cond_new();
	gboolean b;
	gint n;
	
	make_cond_key(key, sizeof key, i, "use_cpu");
	b = panel_applet_gconf_get_bool(applet, key, NULL);
	cfa_cond_set_use_cpu(cond, b);
	
	make_cond_key(key, sizeof key, i, "cpu_id");
	n = panel_applet_gconf_get_int(applet, key, NULL);
	cfa_cond_set_cpu_id(cond, n);
	
	make_cond_key(key, sizeof key, i, "use_ac");
	b = panel_applet_gconf_get_bool(applet, key, NULL);
	cfa_cond_set_use_ac(cond, b);
	
	make_cond_key(key, sizeof key, i, "ac");
	b = panel_applet_gconf_get_bool(applet, key, NULL);
	cfa_cond_set_ac(cond, b);
	
	make_cond_key(key, sizeof key, i, "use_battery");
	b = panel_applet_gconf_get_bool(applet, key, NULL);
	cfa_cond_set_use_battery(cond, b);
	
	make_cond_key(key, sizeof key, i, "battery");
	s = panel_applet_gconf_get_string(applet, key, NULL);
	if (s == NULL)
	    s = g_strdup("0-100");
	cfa_cond_set_battery(cond, s);
	g_free(s);
	
	make_cond_key(key, sizeof key, i, "use_load");
	b = panel_applet_gconf_get_bool(applet, key, NULL);
	cfa_cond_set_use_load(cond, b);
	
	make_cond_key(key, sizeof key, i, "load");
	s = panel_applet_gconf_get_string(applet, key, NULL);
	if (s == NULL)
	    s = g_strdup("0-100");
	cfa_cond_set_load(cond, s);
	g_free(s);
	
	make_cond_key(key, sizeof key, i, "use_process");
	b = panel_applet_gconf_get_bool(applet, key, NULL);
	cfa_cond_set_use_process(cond, b);
	
	make_cond_key(key, sizeof key, i, "process");
	s = panel_applet_gconf_get_string(applet, key, NULL);
	if (s == NULL)
	    s = g_strdup("");
	cfa_cond_set_process(cond, s);
	g_free(s);
	
	make_cond_key(key, sizeof key, i, "freq");
	s = panel_applet_gconf_get_string(applet, key, NULL);
	if (s == NULL)
	    s = g_strdup("0");
	cfa_cond_set_freq(cond, s);
	g_free(s);
	
	cfa_preferences_add_cond(prefs, cond);
    }
    
    cfa_preferences_set_smooth(prefs,
	    panel_applet_gconf_get_bool(applet, "smooth", NULL));
    
    xlog_debug(1, "appconf_load successful.");
    app->prefs = prefs;
}

void appconf_save(struct app_t *app)
{
    CfaPreferences *prefs = app->prefs;
    PanelApplet *applet = app->applet;
    GList *cond_list, *lp;
    gint i;
    
    panel_applet_gconf_set_string(applet, "font_auto",
	    cfa_preferences_get_font_name(prefs, CFA_DESIGN_FONT_NAME_AUTO), NULL);
    
    panel_applet_gconf_set_string(applet, "bg_color_auto",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_BG_COLOR_AUTO), NULL);
    panel_applet_gconf_set_string(applet, "font_color_auto",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_FONT_COLOR_AUTO), NULL);
    panel_applet_gconf_set_string(applet, "arc_color_auto",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_ARC_COLOR_AUTO), NULL);
    panel_applet_gconf_set_string(applet, "hand_color_auto",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_HAND_COLOR_AUTO), NULL);
    
    panel_applet_gconf_set_string(applet, "font_manual",
	    cfa_preferences_get_font_name(prefs, CFA_DESIGN_FONT_NAME_MANUAL), NULL);
    
    panel_applet_gconf_set_string(applet, "bg_color_manual",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_BG_COLOR_MANUAL), NULL);
    panel_applet_gconf_set_string(applet, "font_color_manual",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_FONT_COLOR_MANUAL), NULL);
    panel_applet_gconf_set_string(applet, "arc_color_manual",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_ARC_COLOR_MANUAL), NULL);
    panel_applet_gconf_set_string(applet, "hand_color_manual",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_HAND_COLOR_MANUAL), NULL);
    panel_applet_gconf_set_string(applet, "drag_color_manual",
	    cfa_preferences_get_color_name(prefs, CFA_DESIGN_DRAG_COLOR_MANUAL), NULL);
    
    cond_list = cfa_preferences_get_cond_list(prefs);
    
    panel_applet_gconf_set_int(applet, "nconds", g_list_length(cond_list), NULL);
    
    for (lp = cond_list, i = 0; lp != NULL; lp = g_list_next(lp), i++) {
	gchar key[64];
	CfaCond *cond = lp->data;
	
	make_cond_key(key, sizeof key, i, "use_cpu");
	panel_applet_gconf_set_bool(applet, key,
		cfa_cond_get_use_cpu(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "cpu_id");
	panel_applet_gconf_set_int(applet, key,
		cfa_cond_get_cpu_id(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "use_ac");
	panel_applet_gconf_set_bool(applet, key,
		cfa_cond_get_use_ac(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "ac");
	panel_applet_gconf_set_bool(applet, key,
		cfa_cond_get_ac(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "use_battery");
	panel_applet_gconf_set_bool(applet, key,
		cfa_cond_get_use_battery(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "battery");
	panel_applet_gconf_set_string(applet, key,
		cfa_cond_get_battery(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "use_load");
	panel_applet_gconf_set_bool(applet, key,
		cfa_cond_get_use_load(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "load");
	panel_applet_gconf_set_string(applet, key,
		cfa_cond_get_load(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "use_process");
	panel_applet_gconf_set_bool(applet, key,
		cfa_cond_get_use_process(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "process");
	panel_applet_gconf_set_string(applet, key,
		cfa_cond_get_process(cond), NULL);
	
	make_cond_key(key, sizeof key, i, "freq");
	panel_applet_gconf_set_string(applet, key,
		cfa_cond_get_freq(cond), NULL);
    }
    
    panel_applet_gconf_set_bool(applet, "smooth",
	    cfa_preferences_get_smooth(prefs), NULL);
}

void appconf_init(void)
{
}
