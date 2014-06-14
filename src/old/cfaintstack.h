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
 * $Id: cfaintstack.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_INT_STACK_H__INCLUDED
#define CFA_INT_STACK_H__INCLUDED

#include <common.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define	CFA_TYPE_INT_STACK			cfa_int_stack_get_type()
#define CFA_INT_STACK(int_stack)		G_TYPE_CHECK_INSTANCE_CAST((int_stack), CFA_TYPE_INT_STACK, CfaIntStack)
#define CFA_INT_STACK_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_INT_STACK, CfaIntStackClass)
#define CFA_IS_INT_STACK(int_stack)		G_TYPE_CHECK_INSTANCE_TYPE((int_stack), CFA_TYPE_INT_STACK)
#define CFA_IS_INT_STACK_CLASS(klass)		G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_INT_STACK)
#define	CFA_INT_STACK_GET_CLASS(int_stack)	G_TYPE_INSTANCE_GET_CLASS((int_stack), CFA_TYPE_INT_STACK, CfaIntStackClass)

typedef struct _CfaIntStack CfaIntStack;
typedef struct _CfaIntStackClass CfaIntStackClass;

struct _CfaIntStack {
    GObject object;
    
    gint *buf;
    gint size, cur;
};

struct _CfaIntStackClass {
    GObjectClass parent_class;
};

GType	cfa_int_stack_get_type	(void) G_GNUC_CONST;

CfaIntStack *cfa_int_stack_new(void);

void cfa_int_stack_push(CfaIntStack *stk, gint value);
gint cfa_int_stack_peek(CfaIntStack *stk);
gint cfa_int_stack_pop(CfaIntStack *stk);

G_END_DECLS

#endif /* ifndef CFA_INT_STACK_H__INCLUDED */
