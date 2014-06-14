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
 * $Id: cfasocketreceivebuffer.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_SOCKET_RECEIVE_BUFFER_H__INCLUDED
#define CFA_SOCKET_RECEIVE_BUFFER_H__INCLUDED

#include <common.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define	CFA_TYPE_SOCKET_RECEIVE_BUFFER			cfa_socket_receive_buffer_get_type()
#define CFA_SOCKET_RECEIVE_BUFFER(socket)		G_TYPE_CHECK_INSTANCE_CAST((socket), CFA_TYPE_SOCKET_RECEIVE_BUFFER, CfaSocketReceiveBuffer)
#define CFA_SOCKET_RECEIVE_BUFFER_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_SOCKET_RECEIVE_BUFFER, CfaSocketReceiveBufferClass)
#define CFA_IS_SOCKET_RECEIVE_BUFFER(socket)		G_TYPE_CHECK_INSTANCE_TYPE((socket), CFA_TYPE_SOCKET_RECEIVE_BUFFER)
#define CFA_IS_SOCKET_RECEIVE_BUFFER_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_SOCKET_RECEIVE_BUFFER)
#define	CFA_SOCKET_RECEIVE_BUFFER_GET_CLASS(socket)	G_TYPE_INSTANCE_GET_CLASS((socket), CFA_TYPE_SOCKET_RECEIVE_BUFFER, CfaSocketReceiveBufferClass)

typedef struct _CfaSocketReceiveBuffer CfaSocketReceiveBuffer;
typedef struct _CfaSocketReceiveBufferClass CfaSocketReceiveBufferClass;

struct _CfaSocketReceiveBuffer {
    GObject object;
    
    GList *list;
    gchar *buf;
    guint datsiz, buflen;
};

struct _CfaSocketReceiveBufferClass {
    GObjectClass parent_class;
};

GType			cfa_socket_receive_buffer_get_type	(void) G_GNUC_CONST;
CfaSocketReceiveBuffer	*cfa_socket_receive_buffer_new		(void);
void			cfa_socket_receive_buffer_append	(CfaSocketReceiveBuffer *sock,
								 const gchar *dat);
gboolean		cfa_socket_receive_buffer_has_lines	(CfaSocketReceiveBuffer *sock);
gchar			*cfa_socket_receive_buffer_get_line	(CfaSocketReceiveBuffer *sock);

G_END_DECLS

#endif /* ifndef CFA_SOCKET_RECEIVE_BUFFER_H__INCLUDED */
