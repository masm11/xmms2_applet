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
 * $Id: cfastate.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_STATE_H__INCLUDED
#define CFA_STATE_H__INCLUDED

#include <common.h>
#include <glib-object.h>
#include <cfaclientsocket.h>
#include <cfacpustates.h>
#include <cfastringtokenizer.h>

G_BEGIN_DECLS

#define	CFA_TYPE_STATE			cfa_state_get_type()
#define CFA_STATE(state)		G_TYPE_CHECK_INSTANCE_CAST((state), CFA_TYPE_STATE, CfaState)
#define CFA_STATE_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_STATE, CfaStateClass)
#define CFA_IS_STATE(state)		G_TYPE_CHECK_INSTANCE_TYPE((state), CFA_TYPE_STATE)
#define CFA_IS_STATE_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_STATE)
#define	CFA_STATE_GET_CLASS(state)	G_TYPE_INSTANCE_GET_CLASS((state), CFA_TYPE_STATE, CfaStateClass)

typedef struct _CfaState CfaState;
typedef struct _CfaStateClass CfaStateClass;

struct _CfaState {
    GObject object;
    
    CfaCPUStates *cpu_states;
};

struct _CfaStateClass {
    GObjectClass parent_class;
};

GType		cfa_state_get_type	(void) G_GNUC_CONST;

CfaState	*cfa_state_new		(void);

void		cfa_state_add		(CfaState *state,
					 CfaStringTokenizer *stk);

G_END_DECLS

#endif /* ifndef CFA_STATE_H__INCLUDED */
