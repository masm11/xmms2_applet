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
 * $Id: cfacond.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include "cfacond.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

G_DEFINE_TYPE(CfaCond, cfa_cond, G_TYPE_OBJECT);

static void	cfa_cond_dispose	(GObject *object);
static void	cfa_cond_finalize	(GObject *object);

static void cfa_cond_class_init(CfaCondClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->dispose = cfa_cond_dispose;
    object_class->finalize = cfa_cond_finalize;
}

static void cfa_cond_init(CfaCond *cond)
{
    xlog_debug(1, "cfa_cond_init:");
    
    cond->battery = g_strdup("0-100");
    cond->load = g_strdup("0-100");
    cond->process = g_strdup("");
    cond->freq = g_strdup("0");
}

static void cfa_cond_dispose(GObject *object)
{
    CfaCond *cond;
    
    xlog_debug(1, "cfa_cond_dispose:");
    
    g_return_if_fail(CFA_IS_COND(object));
    cond = CFA_COND(object);
}

static void cfa_cond_finalize(GObject *object)
{
    CfaCond *cond;
    
    xlog_debug(1, "cfa_cond_finalize:");
    
    g_return_if_fail(CFA_IS_COND(object));
    cond = CFA_COND(object);
    
    if (cond->battery != NULL) {
	g_free(cond->battery);
	cond->battery = NULL;
    }
    if (cond->load != NULL) {
	g_free(cond->load);
	cond->load = NULL;
    }
    if (cond->process != NULL) {
	g_free(cond->process);
	cond->process = NULL;
    }
    if (cond->freq != NULL) {
	g_free(cond->freq);
	cond->freq = NULL;
    }
}

CfaCond *cfa_cond_new(void)
{
    CfaCond *cond;
    
    cond = g_object_new(CFA_TYPE_COND, NULL);
    
    return cond;
}

void cfa_cond_set_use_cpu(CfaCond *cond, gboolean use_cpu)
{
    cond->use_cpu = use_cpu;
}
void cfa_cond_set_cpu_id(CfaCond *cond, gint cpu_id)
{
    cond->cpu_id = cpu_id;
}
void cfa_cond_set_use_ac(CfaCond *cond, gboolean use_ac)
{
    cond->use_ac = use_ac;
}
void cfa_cond_set_ac(CfaCond *cond, gboolean ac)
{
    cond->ac = ac;
}
void cfa_cond_set_use_battery(CfaCond *cond, gboolean use_battery)
{
    cond->use_battery = use_battery;
}
void cfa_cond_set_battery(CfaCond *cond, const gchar *battery)
{
    if (cond->battery != NULL)
	g_free(cond->battery);
    cond->battery = g_strdup(battery);
}
void cfa_cond_set_use_load(CfaCond *cond, gboolean use_load)
{
    cond->use_load = use_load;
}
void cfa_cond_set_load(CfaCond *cond, const gchar *load)
{
    if (cond->load != NULL)
	g_free(cond->load);
    cond->load = g_strdup(load);
}
void cfa_cond_set_use_process(CfaCond *cond, gboolean use_process)
{
    cond->use_process = use_process;
}
void cfa_cond_set_process(CfaCond *cond, const gchar *process)
{
    if (cond->process != NULL)
	g_free(cond->process);
    cond->process = g_strdup(process);
}
void cfa_cond_set_freq(CfaCond *cond, const gchar *freq)
{
    if (cond->freq != NULL)
	g_free(cond->freq);
    cond->freq = g_strdup(freq);
}

gboolean cfa_cond_get_use_cpu(CfaCond *cond)
{
    return cond->use_cpu;
}
gint cfa_cond_get_cpu_id(CfaCond *cond)
{
    return cond->cpu_id;
}
gboolean cfa_cond_get_use_ac(CfaCond *cond)
{
    return cond->use_ac;
}
gboolean cfa_cond_get_ac(CfaCond *cond)
{
    return cond->ac;
}
gboolean cfa_cond_get_use_battery(CfaCond *cond)
{
    return cond->use_battery;
}
const gchar *cfa_cond_get_battery(CfaCond *cond)
{
    return cond->battery;
}
gboolean cfa_cond_get_use_load(CfaCond *cond)
{
    return cond->use_load;
}
const gchar *cfa_cond_get_load(CfaCond *cond)
{
    return cond->load;
}
gboolean cfa_cond_get_use_process(CfaCond *cond)
{
    return cond->use_process;
}
const gchar *cfa_cond_get_process(CfaCond *cond)
{
    return cond->process;
}
const gchar *cfa_cond_get_freq(CfaCond *cond)
{
    return cond->freq;
}

gchar *cfa_cond_to_string(CfaCond *cond)
{
    gchar **strv = g_new0(gchar *, 7);
    gint n = 0;
    gchar *s;
    
    if (cond->use_cpu)
	strv[n++] = g_strdup_printf("cpu %d", cond->cpu_id);
    
    if (cond->use_ac)
	strv[n++] = g_strdup_printf("ac %s", cond->ac ? "on" : "off");
    
    if (cond->use_battery)
	strv[n++] = g_strdup_printf("battery %s", cond->battery);
    
    if (cond->use_load)
	strv[n++] = g_strdup_printf("load %s", cond->load);
    
    if (cond->use_process)
	strv[n++] = g_strdup_printf("process '%s'", cond->process);
    
    strv[n++] = g_strdup_printf("%s", cond->freq);
    
    strv[n] = NULL;
    
    s = g_strjoinv(" ", strv);
    
    g_strfreev(strv);
    
    return s;
}

/*EOF*/
