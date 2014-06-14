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
 * $Id: cfaconn.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_CONN_H__INCLUDED
#define CFA_CONN_H__INCLUDED

#include <common.h>
#include <glib-object.h>
#include <cfaclientsocket.h>
#include <cfastate.h>

G_BEGIN_DECLS

#define	CFA_TYPE_CONN			cfa_conn_get_type()
#define CFA_CONN(conn)			G_TYPE_CHECK_INSTANCE_CAST((conn), CFA_TYPE_CONN, CfaConn)
#define CFA_CONN_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_CONN, CfaConnClass)
#define CFA_IS_CONN(conn)		G_TYPE_CHECK_INSTANCE_TYPE((conn), CFA_TYPE_CONN)
#define CFA_IS_CONN_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_CONN)
#define	CFA_CONN_GET_CLASS(conn)	G_TYPE_INSTANCE_GET_CLASS((conn), CFA_TYPE_CONN, CfaConnClass)

typedef struct _CfaConn CfaConn;
typedef struct _CfaConnClass CfaConnClass;

struct _CfaConn {
    GObject object;
    
    GSource *source;
    
    CfaClientSocket *sock;
    gboolean sock_closed;
    
    gboolean initializing;
    GMainLoop *loop;
    gboolean result;
    gchar *failed_mesg;
    
    CfaState *state;
    CfaState *state0;
    
    GList *other_strings;
};

struct _CfaConnClass {
    GObjectClass parent_class;
};

GType		cfa_conn_get_type	(void) G_GNUC_CONST;

CfaConn		*cfa_conn_new		(void);

void		cfa_conn_destroy	(CfaConn *conn);

void		cfa_conn_connect	(CfaConn *conn,
					 GError **err);
void		cfa_conn_connect_sys	(CfaConn *conn,
					 GError **err);

gboolean	cfa_conn_send		(CfaConn *conn,
					 const gchar *req,
					 GError **err);

CfaState	*cfa_conn_get_state	(CfaConn *conn);
gchar		*cfa_conn_get_line	(CfaConn *conn);

#define CFA_CONN_ERROR cfa_conn_error_quark()
enum CfaServerSocket {
    CFA_CONN_ERROR_FAILED,
};

G_END_DECLS

#endif /* ifndef CFA_CONN_H__INCLUDED */
