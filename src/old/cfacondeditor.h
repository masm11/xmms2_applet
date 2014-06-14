/* CPUFreqApplet
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
 * $Id: cfacondeditor.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_COND_EDITOR_H__INCLUDED
#define CFA_COND_EDITOR_H__INCLUDED

#include <common.h>
#include <gtk/gtk.h>
#include "cfacond.h"

G_BEGIN_DECLS

#define CFA_TYPE_COND_EDITOR		cfa_cond_editor_get_type()
#define CFA_COND_EDITOR(obj)		G_TYPE_CHECK_INSTANCE_CAST ((obj), CFA_TYPE_COND_EDITOR, CfaCondEditor)
#define CFA_COND_EDITOR_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST ((klass), CFA_TYPE_COND_EDITOR, CfaCondEditorClass)
#define CFA_IS_COND_EDITOR(obj)		G_TYPE_CHECK_INSTANCE_TYPE ((obj), CFA_TYPE_COND_EDITOR)
#define CFA_IS_COND_EDITOR_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE ((klass), CFA_TYPE_COND_EDITOR)
#define CFA_COND_EDITOR_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS ((obj), CFA_TYPE_COND_EDITOR, CfaCondEditorClass)

typedef struct _CfaCondEditor       CfaCondEditor;
typedef struct _CfaCondEditorClass  CfaCondEditorClass;

struct _CfaCondEditor {
    GtkHBox hbox;
    
    GtkWidget *view;
    GtkListStore *store;
};

struct _CfaCondEditorClass {
    GtkVBoxClass parent_class;
};

GType		cfa_cond_editor_get_type	(void) G_GNUC_CONST;
GtkWidget	*cfa_cond_editor_new		(void);

void		cfa_cond_editor_set_cond_list	(CfaCondEditor *editor,
						 GList *cond_list);
GList		*cfa_cond_editor_get_cond_list	(CfaCondEditor *editor);

void		cfa_cond_editor_test		(CfaCondEditor *editor);

G_END_DECLS

#endif	/* ifndef CFA_COND_EDITOR_H__INCLUDED */
