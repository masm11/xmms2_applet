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
 * $Id: cfacpustates.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <freqstr.h>
#include <cfacpustates.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

G_DEFINE_TYPE(CfaCPUStates, cfa_cpu_states, G_TYPE_OBJECT);

typedef struct _CfaCPUStatesFreqs {
    freq_t cur, min, max;
    freq_t *avail;
} CfaCPUStatesFreqs;

static void	cfa_cpu_states_finalize		(GObject *object);

static gchar	*make_available_freqs_string	(freq_t *avail);

static void cfa_cpu_states_class_init(CfaCPUStatesClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->finalize = cfa_cpu_states_finalize;
}

static void cfa_cpu_states_init(CfaCPUStates *states)
{
    xlog_debug(1, "cfa_cpu_states_init:");
    
    states->nr = 1;
    states->freqs = g_new(CfaCPUStatesFreqs, states->nr);
    states->freqs[0].cur = -1;
    states->freqs[0].min = -1;
    states->freqs[0].max = -1;
    states->freqs[0].avail = g_new(freq_t, 1);
    states->freqs[0].avail[0] = -1;
}

static void cfa_cpu_states_finalize(GObject *object)
{
    CfaCPUStates *states;
    gint i;
    
    xlog_debug(1, "cfa_cpu_states_finalize:");
    
    g_return_if_fail(CFA_IS_CPU_STATES(object));
    states = CFA_CPU_STATES(object);
    
    for (i = 0; i < states->nr; i++) {
	if (states->freqs[i].avail != NULL) {
	    g_free(states->freqs[i].avail);
	    states->freqs[i].avail = NULL;
	}
    }
    
    g_free(states->freqs);
    states->freqs = NULL;
    states->nr = 0;
}

/**
 * cfa_cpu_states_new:
 *
 * Creates a new CPU states, which contains the states of all the CPUs.
 *
 * Returns: a new CPU states.
 */
CfaCPUStates *cfa_cpu_states_new(void)
{
    CfaCPUStates *states;
    
    states = g_object_new(CFA_TYPE_CPU_STATES, NULL);
    
    return states;
}

/**
 * cfa_cpu_states_set_nr:
 * @states:
 * @nr: the number of CPUs.
 *
 * Set the number of CPUs in the states.
 */
void cfa_cpu_states_set_nr(CfaCPUStates *states, gint nr)
{
    gint i;
    
    if (nr > states->nr) {
	states->freqs = g_renew(CfaCPUStatesFreqs, states->freqs, nr);
	for (i = states->nr; i < nr; i++) {
	    states->freqs[i].cur = -1;
	    states->freqs[i].min = -1;
	    states->freqs[i].max = -1;
	    states->freqs[i].avail = g_new(freq_t, 1);
	    states->freqs[i].avail[0] = -1;
	}
	states->nr = nr;
    } else if (nr < states->nr) {
	for (i = nr; i < states->nr; i++) {
	    free(states->freqs[i].avail);
	}
	states->freqs = g_renew(CfaCPUStatesFreqs, states->freqs, nr);
	states->nr = nr;
    }
}

/**
 * cfa_cpu_states_set_freqs:
 * @states:
 * @cpu_id: CPU Id.
 * @min: minimum frequency of the CPU.
 * @max: maximum frequency of the CPU.
 * @cur: current frequency of the CPU.
 * @available_freqs: available frequencies of the CPU.
 *
 * Set the frequencies of the specified CPU in the states.
 *
 * The instance owns the available_freqs array. You shouldn't free or write to it.
 */
void cfa_cpu_states_set_freqs(CfaCPUStates *states,
	gint cpu_id, freq_t min, freq_t max, freq_t cur, freq_t *available_freqs)
{
    g_return_if_fail(cpu_id >= 0 && cpu_id < states->nr);
    
    states->freqs[cpu_id].min = min;
    states->freqs[cpu_id].max = max;
    states->freqs[cpu_id].cur = cur;
    if (states->freqs[cpu_id].avail != NULL)
	g_free(states->freqs[cpu_id].avail);
    states->freqs[cpu_id].avail = available_freqs;
}

/**
 * cfa_cpu_states_get_nr:
 * @states:
 *
 * Get the number of CPUs from the states.
 * 
 * Returns: the number of CPUs.
 */
gint cfa_cpu_states_get_nr(CfaCPUStates *states)
{
    return states->nr;
}

/**
 * cfa_cpu_states_get_freqs:
 * @states:
 * @cpu_id: CPU Id.
 * @cur: pointer where stores current frequency of the CPU.
 * @min: pointer where stores minimum frequency of the CPU.
 * @max: pointer where stores maximum frequency of the CPU.
 * @avail: pointer where stores available frequencies of the CPU.
 *
 * Get the frequencies of the specified CPU from the states.
 *
 * The array of avail is owned by this instance. You shouldn't free or write to it.
 */
void cfa_cpu_states_get_freqs(CfaCPUStates *states,
	gint cpu_id, freq_t *cur, freq_t *min, freq_t *max, const freq_t **avail)
{
    g_return_if_fail(cpu_id >= 0 && cpu_id < states->nr);
    
    *cur = states->freqs[cpu_id].cur;
    *min = states->freqs[cpu_id].min;
    *max = states->freqs[cpu_id].max;
    *avail = states->freqs[cpu_id].avail;
}

/**
 * cfa_cpu_states_equals:
 * @states:
 * @states0:
 *
 * Compares two states.
 *
 * Returns: %TRUE if two states are identical.
 */
gboolean cfa_cpu_states_equals(CfaCPUStates *states, CfaCPUStates *states0)
{
    gint i, j;
    
    if (states->nr != states0->nr)
	return FALSE;
    
    for (i = 0; i < states->nr; i++) {
	if (states->freqs[i].cur != states0->freqs[i].cur)
	    return FALSE;
	if (states->freqs[i].min != states0->freqs[i].min)
	    return FALSE;
	if (states->freqs[i].max != states0->freqs[i].max)
	    return FALSE;
	
	for (j = 0; ; j++) {
	    if (states->freqs[i].avail[j] != states0->freqs[i].avail[j])
		return FALSE;
	    if (states->freqs[i].avail[j] != -1)
		break;
	}
    }
    
    return TRUE;
}

/**
 * cfa_cpu_states_to_strings:
 * @states:
 *
 * Creates a list of string describing the states.
 *
 * You own the list.
 *
 * Returns: list of strings.
 */
GList *cfa_cpu_states_to_strings(CfaCPUStates *states)
{
    GList *list = NULL;
    gint i;
    
    list = g_list_append(list,
	    g_strdup_printf("cpu nr %d", states->nr));
    
    for (i = 0; i < states->nr; i++) {
	gchar *s_cur = cfa_freqstr_to_string(states->freqs[i].cur);
	gchar *s_min = cfa_freqstr_to_string(states->freqs[i].min);
	gchar *s_max = cfa_freqstr_to_string(states->freqs[i].max);
	gchar *s_av = make_available_freqs_string(states->freqs[i].avail);
	
	list = g_list_append(list,
		g_strdup_printf("cpu %d %s %s %s %s", i, s_cur, s_min, s_max, s_av));
	
	g_free(s_cur);
	g_free(s_min);
	g_free(s_max);
	g_free(s_av);
    }
    
    return list;
}

static gchar *make_available_freqs_string(freq_t *avail)
{
    gint i;
    gchar *str;
    
    str = g_strdup("");
    
    for (i = 0; avail[i] != -1; i++) {
	gchar *s = cfa_freqstr_to_string(avail[i]);
	gchar *t = g_strconcat(str, s, " ", NULL);
	g_free(s);
	g_free(str);
	str = t;
    }
    
    return str;
}
