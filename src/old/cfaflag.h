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
 * $Id: cfaflag.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_FLAG_H__INCLUDED
#define CFA_FLAG_H__INCLUDED

#include <common.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define	CFA_TYPE_FLAG			cfa_flag_get_type()
#define CFA_FLAG(flag)			G_TYPE_CHECK_INSTANCE_CAST((flag), CFA_TYPE_FLAG, CfaFlag)
#define CFA_FLAG_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_FLAG, CfaFlagClass)
#define CFA_IS_FLAG(flag)		G_TYPE_CHECK_INSTANCE_TYPE((flag), CFA_TYPE_FLAG)
#define CFA_IS_FLAG_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_FLAG)
#define	CFA_FLAG_GET_CLASS(flag)	G_TYPE_INSTANCE_GET_CLASS((flag), CFA_TYPE_FLAG, CfaFlagClass)

#define CFA_TYPE_FLAG_DESIGN_NAME	cfa_flag_design_name_get_type()


typedef struct _CfaFlag CfaFlag;
typedef struct _CfaFlagClass CfaFlagClass;

struct _CfaFlag {
    GObject object;
    
    gboolean value;
};

struct _CfaFlagClass {
    GObjectClass parent_class;
};

GType		cfa_flag_get_type	(void) G_GNUC_CONST;

CfaFlag		*cfa_flag_new		(void);

void		cfa_flag_set		(CfaFlag *flag, gboolean value);
gboolean	cfa_flag_get		(CfaFlag *flag);

G_END_DECLS

#endif /* ifndef CFA_FLAG_H__INCLUDED */
