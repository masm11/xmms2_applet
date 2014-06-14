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
 * $Id: cfaintstack.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <cfaintstack.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

G_DEFINE_TYPE(CfaIntStack, cfa_int_stack, G_TYPE_OBJECT);

static void	cfa_int_stack_finalize	(GObject *object);

static void cfa_int_stack_class_init(CfaIntStackClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->finalize = cfa_int_stack_finalize;
}

static void cfa_int_stack_init(CfaIntStack *stk)
{
    xlog_debug(1, "cfa_int_stack_init:");
    
    stk->size = 2;
    stk->cur = 0;
    stk->buf = g_new(gint, stk->size);
}

static void cfa_int_stack_finalize(GObject *object)
{
    CfaIntStack *stk;
    
    xlog_debug(1, "cfa_int_stack_finalize:");
    
    g_return_if_fail(CFA_IS_INT_STACK(object));
    stk = CFA_INT_STACK(object);
    
    g_free(stk->buf);
    stk->buf = NULL;
    stk->size = 0;
    stk->cur = 0;
}

CfaIntStack *cfa_int_stack_new(void)
{
    CfaIntStack *stk;
    
    stk = g_object_new(CFA_TYPE_INT_STACK, NULL);
    
    return stk;
}

void cfa_int_stack_push(CfaIntStack *stk, gint value)
{
    g_return_if_fail(CFA_IS_INT_STACK(stk));
    
    if (stk->cur >= stk->size) {
	stk->size += stk->size / 2;
	stk->buf = g_renew(gint, stk->buf, stk->size);
    }
    
    stk->buf[stk->cur++] = value;
}

gint cfa_int_stack_peek(CfaIntStack *stk)
{
    g_return_val_if_fail(CFA_IS_INT_STACK(stk), 0);
    
    if (stk->size <= 0) {
	g_warning("cfa_int_stack_peek: no value stacked.");
	return 0;
    }
    
    return stk->buf[stk->cur - 1];
}

gint cfa_int_stack_pop(CfaIntStack *stk)
{
    g_return_val_if_fail(CFA_IS_INT_STACK(stk), 0);
    
    if (stk->size <= 0) {
	g_warning("cfa_int_stack_pop: no value stacked.");
	return 0;
    }
    
    return stk->buf[--stk->cur];
}
