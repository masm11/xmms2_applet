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
 * $Id: cfacond.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_COND_H__INCLUDED
#define CFA_COND_H__INCLUDED

#include <common.h>
#include <glib-object.h>
#include <cfaclientsocket.h>
#include <cfastate.h>

G_BEGIN_DECLS

#define	CFA_TYPE_COND			cfa_cond_get_type()
#define CFA_COND(cond)			G_TYPE_CHECK_INSTANCE_CAST((cond), CFA_TYPE_COND, CfaCond)
#define CFA_COND_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_COND, CfaCondClass)
#define CFA_IS_COND(cond)		G_TYPE_CHECK_INSTANCE_TYPE((cond), CFA_TYPE_COND)
#define CFA_IS_COND_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_COND)
#define	CFA_COND_GET_CLASS(cond)	G_TYPE_INSTANCE_GET_CLASS((cond), CFA_TYPE_COND, CfaCondClass)

typedef struct _CfaCond CfaCond;
typedef struct _CfaCondClass CfaCondClass;

struct _CfaCond {
    GObject object;
    
    gboolean use_cpu;
    gint cpu_id;
    gboolean use_ac;
    gboolean ac;
    gboolean use_battery;
    gchar *battery;
    gboolean use_load;
    gchar *load;
    gboolean use_process;
    gchar *process;
    gchar *freq;
};

struct _CfaCondClass {
    GObjectClass parent_class;
};

GType		cfa_cond_get_type	(void) G_GNUC_CONST;

CfaCond		*cfa_cond_new		(void);

void		cfa_cond_set_use_cpu		(CfaCond *cond,
						 gboolean use_cpu);
void		cfa_cond_set_cpu_id		(CfaCond *cond,
						 gint cpu_id);
void		cfa_cond_set_use_ac		(CfaCond *cond,
						 gboolean use_ac);
void		cfa_cond_set_ac			(CfaCond *cond,
						 gboolean ac);
void		cfa_cond_set_use_battery	(CfaCond *cond,
						 gboolean use_battery);
void		cfa_cond_set_battery		(CfaCond *cond,
						 const gchar *battery);
void		cfa_cond_set_use_load		(CfaCond *cond,
						 gboolean use_load);
void		cfa_cond_set_load		(CfaCond *cond,
						 const gchar *load);
void		cfa_cond_set_use_process	(CfaCond *cond,
						 gboolean use_process);
void		cfa_cond_set_process		(CfaCond *cond,
						 const gchar *process);
void		cfa_cond_set_freq		(CfaCond *cond,
						 const gchar *freq);

gboolean	cfa_cond_get_use_cpu		(CfaCond *cond);
gint		cfa_cond_get_cpu_id		(CfaCond *cond);
gboolean	cfa_cond_get_use_ac		(CfaCond *cond);
gboolean	cfa_cond_get_ac			(CfaCond *cond);
gboolean	cfa_cond_get_use_battery	(CfaCond *cond);
const gchar	*cfa_cond_get_battery		(CfaCond *cond);
gboolean	cfa_cond_get_use_load		(CfaCond *cond);
const gchar	*cfa_cond_get_load		(CfaCond *cond);
gboolean	cfa_cond_get_use_process	(CfaCond *cond);
const gchar	*cfa_cond_get_process		(CfaCond *cond);
const gchar	*cfa_cond_get_freq		(CfaCond *cond);

gchar		*cfa_cond_to_string		(CfaCond *cond);

G_END_DECLS

#endif /* ifndef CFA_COND_H__INCLUDED */
