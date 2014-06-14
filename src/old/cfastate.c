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
 * $Id: cfastate.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include "cfastate.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <freqstr.h>
#include <sys/types.h>

G_DEFINE_TYPE(CfaState, cfa_state, G_TYPE_OBJECT);

static void	cfa_state_dispose	(GObject *object);

static void do_cpu(CfaState *state, CfaStringTokenizer *stk);

static void cfa_state_class_init(CfaStateClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->dispose = cfa_state_dispose;
}

static void cfa_state_init(CfaState *state)
{
    xlog_debug(1, "cfa_state_init:");
}

static void cfa_state_dispose(GObject *object)
{
    CfaState *state;
    
    xlog_debug(1, "cfa_state_dispose:");
    
    g_return_if_fail(CFA_IS_STATE(object));
    state = CFA_STATE(object);
    
    if (state->cpu_states != NULL) {
	g_object_unref(state->cpu_states);
	state->cpu_states = NULL;
    }
}

/**
 * cfa_state_new:
 *
 * Creates a new state, which contains CPU states, and a builtin, a sys, and a user settings.
 * 
 * You can use the objects in the state.
 * 
 * Returns: a new state.
 */
CfaState *cfa_state_new(void)
{
    CfaState *state;
    
    state = g_object_new(CFA_TYPE_STATE, NULL);
    
    state->cpu_states = cfa_cpu_states_new();
    
    return state;
}

/**
 * cfa_state_add:
 * @state: a state.
 * @stk: tokens.
 *
 * Parses the list of strings, and set it in the state.
 */
void cfa_state_add(CfaState *state, CfaStringTokenizer *stk)
{
    gchar *layer = cfa_string_tokenizer_next(stk);
    if (layer == NULL) {
	xlog_error("cfa_state_add: no tokens.");
	return;
    }
    
    if (strcmp(layer, "cpu") == 0) {
	do_cpu(state, stk);
    } else if (strcmp(layer, "builtin") == 0) {
    } else if (strcmp(layer, "sys") == 0) {
    } else if (strcmp(layer, "user") == 0) {
    } else {
	xlog_error("cfa_state_add: bad layer.");
    }
}

static void do_cpu(CfaState *state, CfaStringTokenizer *stk)
{
    gchar *cpu_id_str = cfa_string_tokenizer_next(stk);
    
    if (cpu_id_str == NULL) {
	xlog_error("do_cpu: no cpu_id.");
	return;
    }
    
    if (strcmp(cpu_id_str, "nr") == 0) {
	gchar *nr_str, *ep;
	gint nr;
	nr_str = cfa_string_tokenizer_next(stk);
	if (nr_str == NULL) {
	    xlog_error("do_cpu: no cpu count.");
	    return;
	}
	nr = strtol(nr_str, &ep, 0);
	if (ep != 0 && *ep != '\0') {
	    xlog_error("do_cpu: bad cpu count.");
	    return;
	}
	cfa_cpu_states_set_nr(state->cpu_states, nr);
    } else {
	gchar *cur_str = NULL, *min_str = NULL, *max_str = NULL;
	
	cur_str = cfa_string_tokenizer_next(stk);
	min_str = cfa_string_tokenizer_next(stk);
	max_str = cfa_string_tokenizer_next(stk);
	
	if (max_str != NULL) {
	    gchar *ep;
	    gint cpu_id;
	    freq_t cur, min, max, *avail;
	    gint navail, i;
	    
	    cpu_id = strtol(cpu_id_str, &ep, 0);
	    if (ep != NULL && *ep != '\0') {
		xlog_error("do_cpu: bad cpu_id.");
		return;
	    }
	    
	    cur = cfa_freqstr_to_value(cur_str);
	    if (cur == -1) {
		xlog_error("do_cpu: bad freq.");
		return;
	    }
	    
	    min = cfa_freqstr_to_value(min_str);
	    if (min == -1) {
		xlog_error("do_cpu: bad min.");
		return;
	    }
	    
	    max = cfa_freqstr_to_value(max_str);
	    if (max == -1) {
		xlog_error("do_cpu: bad max.");
		return;
	    }
	    
	    navail = cfa_string_tokenizer_count(stk);
	    avail = g_new(freq_t, navail + 1);
	    for (i = 0; i < navail; i++) {
		freq_t a = cfa_freqstr_to_value(cfa_string_tokenizer_next(stk));
		if (a == -1) {
		    xlog_error("do_cpu: bad freqs.");
		    return;
		}
		avail[i] = a;
	    }
	    avail[i] = -1;
	    
	    cfa_cpu_states_set_freqs(state->cpu_states, cpu_id, min, max, cur, avail);
	} else {
	    xlog_error("do_cpu: few freqs.");
	    return;
	}
    }
}

/*EOF*/
