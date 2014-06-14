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
 * $Id: cfasocket.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <cfasocket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

enum {
    PROP_0,
    PROP_FD,
};

G_DEFINE_ABSTRACT_TYPE(CfaSocket, cfa_socket, G_TYPE_OBJECT);

static void	cfa_socket_set_property	(GObject *object,
					 guint property_id,
					 const GValue *value,
					 GParamSpec *pspec);
static void	cfa_socket_get_property	(GObject *object,
					 guint property_id,
					 GValue *value,
					 GParamSpec *pspec);
static void	cfa_socket_finalize	(GObject *object);

static void cfa_socket_class_init(CfaSocketClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->finalize = cfa_socket_finalize;
    object_class->set_property = cfa_socket_set_property;
    object_class->get_property = cfa_socket_get_property;
    
    g_object_class_install_property(object_class,
	    PROP_FD,
	    g_param_spec_int("fd",
		    "fd",
		    "File descriptor",
		    -1, ((guint) -1) >> 1, -1,
		    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void cfa_socket_init(CfaSocket *sock)
{
    xlog_debug(1, "cfa_socket_init:");
    
    sock->fd = -1;
}

static void cfa_socket_set_property(
	GObject *object, guint property_id,
	const GValue *value, GParamSpec *pspec)
{
    CfaSocket *sock;
    int fd;
    
    xlog_debug(1, "cfa_socket_set_property:");
    g_return_if_fail(CFA_IS_SOCKET(object));
    
    sock = CFA_SOCKET(object);
    
    switch (property_id) {
    case PROP_FD:
	fd = g_value_get_int(value);
	xlog_debug(1, "cfa_socket_set_property: %d->%d.", sock->fd, fd);
	sock->fd = fd;
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
	break;
    }
}

static void cfa_socket_get_property(
	GObject *object, guint property_id,
	GValue *value, GParamSpec *pspec)
{
    CfaSocket *sock;
    
    xlog_debug(1, "cfa_socket_get_property:");
    g_return_if_fail(CFA_IS_SOCKET(object));
    
    sock = CFA_SOCKET(object);
    
    switch (property_id) {
    case PROP_FD:
	g_value_set_int(value, sock->fd);
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
	break;
    }
}

static void cfa_socket_finalize(GObject *object)
{
    CfaSocket *sock;
    
    xlog_debug(1, "cfa_socket_finalize:");
    g_return_if_fail(CFA_IS_SOCKET(object));
    
    sock = CFA_SOCKET(object);
    
    if (sock->fd != -1) {
	close(sock->fd);
	sock->fd = -1;
    }
}
