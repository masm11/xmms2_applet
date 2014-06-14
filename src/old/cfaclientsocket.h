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
 * $Id: cfaclientsocket.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_CLIENTSOCKET_H__INCLUDED
#define CFA_CLIENTSOCKET_H__INCLUDED

#include <common.h>
#include <cfasocket.h>
#include <cfasocketreceivebuffer.h>
#include <cfasocketsendbuffer.h>

G_BEGIN_DECLS

#define	CFA_TYPE_CLIENT_SOCKET				cfa_client_socket_get_type()
#define CFA_CLIENT_SOCKET(client_socket)		G_TYPE_CHECK_INSTANCE_CAST((client_socket), CFA_TYPE_CLIENT_SOCKET, CfaClientSocket)
#define CFA_CLIENT_SOCKET_CLASS(klass)			G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_CLIENT_SOCKET, CfaClientSocketClass)
#define CFA_IS_CLIENT_SOCKET(client_socket)		G_TYPE_CHECK_INSTANCE_TYPE((client_socket), CFA_TYPE_CLIENT_SOCKET)
#define CFA_IS_CLIENT_SOCKET_CLASS(klass)		G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_CLIENT_SOCKET)
#define	CFA_CLIENT_SOCKET_GET_CLASS(client_socket)	G_TYPE_INSTANCE_GET_CLASS((client_socket), CFA_TYPE_CLIENT_SOCKET, CfaClientSocketClass)

typedef struct _CfaClientSocket CfaClientSocket;
typedef struct _CfaClientSocketClass CfaClientSocketClass;

struct cfa_client_socket_source_t;

struct _CfaClientSocket {
    CfaSocket socket;
    
    gchar *path;
    
    gboolean no_more_recv, no_more_send;
    CfaSocketReceiveBuffer *recv_buf;
    CfaSocketSendBuffer *send_buf;
    
    GSource *source;
    GPollFD pollfd;
};

struct _CfaClientSocketClass {
    CfaSocketClass parent_class;
};

GType		cfa_client_socket_get_type		(void) G_GNUC_CONST;
CfaClientSocket	*cfa_client_socket_new_with_connect	(const gchar *path,
							 GError **err);
CfaClientSocket	*cfa_client_socket_new_with_fd		(int fd);
void		cfa_client_socket_close			(CfaClientSocket *client_socket);
void		cfa_client_socket_destroy		(CfaClientSocket *client_socket);
gchar		*cfa_client_socket_recv_line		(CfaClientSocket *client_socket);
void		cfa_client_socket_send_line		(CfaClientSocket *client_socket,
							 const gchar *data);

GQuark		cfa_client_socket_error_quark		(void);

#define CFA_CLIENT_SOCKET_ERROR cfa_client_socket_error_quark()
enum CfaClientSocketError {
    CFA_CLIENT_SOCKET_ERROR_SOCKET,
    CFA_CLIENT_SOCKET_ERROR_CONNECT,
};

G_END_DECLS

#endif /* ifndef CFA_CLIENTSOCKET_H__INCLUDED */
