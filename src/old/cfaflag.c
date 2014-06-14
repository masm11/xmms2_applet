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
 * $Id: cfaflag.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include "cfaflag.h"
#include <stdio.h>
#include <stdlib.h>

G_DEFINE_TYPE(CfaFlag, cfa_flag, G_TYPE_OBJECT);

enum {
    PROP_0,
    PROP_VALUE,
};

enum {
    SIG_CHANGED,
    SIG_NR
};

static void	cfa_flag_set_property	(GObject *object,
					 guint property_id,
					 const GValue *value,
					 GParamSpec *pspec);
static void	cfa_flag_get_property	(GObject *object,
					 guint property_id,
					 GValue *value,
					 GParamSpec *pspec);

static guint signals[SIG_NR] = { 0 };

static void cfa_flag_class_init(CfaFlagClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->set_property = cfa_flag_set_property;
    object_class->get_property = cfa_flag_get_property;
    
    g_object_class_install_property(object_class,
	    PROP_VALUE,
	    g_param_spec_boolean("value",
		    "Value",
		    "Boolean value",
		    FALSE,
		    G_PARAM_READWRITE));
    
    signals[SIG_CHANGED] = g_signal_new("changed",
	    G_OBJECT_CLASS_TYPE(object_class),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
}

static void cfa_flag_init(CfaFlag *flag)
{
    xlog_debug(1, "cfa_flag_init:");
    
    flag->value = FALSE;
}

static void cfa_flag_set_property(
	GObject *object, guint property_id,
	const GValue *value, GParamSpec *pspec)
{
    CfaFlag *flag;
    
    g_return_if_fail(CFA_IS_FLAG(object));
    
    flag = CFA_FLAG(object);
    
    switch (property_id) {
    case PROP_VALUE:
	cfa_flag_set(flag, g_value_get_boolean(value));
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
	break;
    }
}

static void cfa_flag_get_property(
	GObject *object, guint property_id,
	GValue *value, GParamSpec *pspec)
{
    CfaFlag *flag;
    
    g_return_if_fail(CFA_IS_FLAG(object));
    
    flag = CFA_FLAG(object);
    
    switch (property_id) {
    case PROP_VALUE:
	g_value_set_boolean(value, flag->value);
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
	break;
    }
}

/**
 * cfa_flag_new:
 * 
 * Creates a new flag, whose initial value is %FALSE.
 * 
 * Returns: a new flag.
 */
CfaFlag *cfa_flag_new(void)
{
    CfaFlag *prefs;
    
    prefs = g_object_new(CFA_TYPE_FLAG, NULL);
    
    return prefs;
}

/**
 * cfa_flag_set:
 * @flag: a flag.
 * @value: a value.
 *
 * Set a value of the flag.
 */
void cfa_flag_set(CfaFlag *flag, gboolean value)
{
    if (flag->value != value) {
	flag->value = value;
	g_signal_emit(flag, signals[SIG_CHANGED], 0);
    }
}

/**
 * cfa_flag_get:
 * @flag: a flag.
 *
 * Get a value of the flag.
 *
 * Returns: value of the flag.
 */
gboolean cfa_flag_get(CfaFlag *flag)
{
    return flag->value;
}

/*EOF*/
