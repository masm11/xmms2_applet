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
 * $Id: cfacpustates.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_CPU_STATES_H__INCLUDED
#define CFA_CPU_STATES_H__INCLUDED

#include <common.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define	CFA_TYPE_CPU_STATES			cfa_cpu_states_get_type()
#define CFA_CPU_STATES(cpu_states)		G_TYPE_CHECK_INSTANCE_CAST((cpu_states), CFA_TYPE_CPU_STATES, CfaCPUStates)
#define CFA_CPU_STATES_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_CPU_STATES, CfaCPUStatesClass)
#define CFA_IS_CPU_STATES(cpu_states)		G_TYPE_CHECK_INSTANCE_TYPE((cpu_states), CFA_TYPE_CPU_STATES)
#define CFA_IS_CPU_STATES_CLASS(klass)		G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_CPU_STATES)
#define	CFA_CPU_STATES_GET_CLASS(cpu_states)	G_TYPE_INSTANCE_GET_CLASS((cpu_states), CFA_TYPE_CPU_STATES, CfaCPUStatesClass)

typedef struct _CfaCPUStates CfaCPUStates;
typedef struct _CfaCPUStatesClass CfaCPUStatesClass;

struct _CfaCPUStates {
    GObject object;
    
    gint nr;
    struct _CfaCPUStatesFreqs *freqs;
};

struct _CfaCPUStatesClass {
    GObjectClass parent_class;
};

GType		cfa_cpu_states_get_type		(void) G_GNUC_CONST;

CfaCPUStates	*cfa_cpu_states_new		(void);

void		cfa_cpu_states_set_nr		(CfaCPUStates *states,
						 gint nr);
void		cfa_cpu_states_set_freqs	(CfaCPUStates *states,
						 gint cpu_id,
						 freq_t min,
						 freq_t max,
						 freq_t cur,
						 freq_t *available_freqs);
gint		cfa_cpu_states_get_nr		(CfaCPUStates *states);
void		cfa_cpu_states_get_freqs	(CfaCPUStates *states,
						 gint cpu_id,
						 freq_t *cur,
						 freq_t *min,
						 freq_t *max,
						 const freq_t **avail);
gboolean	cfa_cpu_states_equals		(CfaCPUStates *states,
						 CfaCPUStates *states0);
GList		*cfa_cpu_states_to_strings	(CfaCPUStates *states);

G_END_DECLS

#endif /* ifndef CFA_CPU_STATES_H__INCLUDED */
