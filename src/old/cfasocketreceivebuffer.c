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
 * $Id: cfasocketreceivebuffer.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <cfasocketreceivebuffer.h>
#include <free_string_list.h>
#include <stdio.h>
#include <string.h>

G_DEFINE_TYPE(CfaSocketReceiveBuffer, cfa_socket_receive_buffer, G_TYPE_OBJECT);

static void	cfa_socket_receive_buffer_finalize	(GObject *object);

static void cfa_socket_receive_buffer_class_init(CfaSocketReceiveBufferClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->finalize = cfa_socket_receive_buffer_finalize;
}

static void cfa_socket_receive_buffer_init(CfaSocketReceiveBuffer *sock)
{
    xlog_debug(1, "cfa_socket_receive_buffer_init:");
    
    sock->datsiz = 0;
    sock->buflen = 1024;
    sock->buf = g_new(gchar, sock->buflen);
    sock->buf[0] = '\0';
}

static void cfa_socket_receive_buffer_finalize(GObject *object)
{
    CfaSocketReceiveBuffer *sock;
    
    xlog_debug(1, "cfa_socket_receive_buffer_finalize:");
    g_return_if_fail(CFA_IS_SOCKET_RECEIVE_BUFFER(object));
    
    sock = CFA_SOCKET_RECEIVE_BUFFER(object);
    
    sock->buflen = 0;
    sock->datsiz = 0;
    g_free(sock->buf);
    sock->buf = NULL;
    
    cfa_free_string_list(sock->list);
    sock->list = NULL;
}

/**
 * cfa_socket_receive_buffer_new:
 *
 * Creates a new receive buffer.
 *
 * Returns: a new receive buffer.
 */
CfaSocketReceiveBuffer *cfa_socket_receive_buffer_new(void)
{
    return (CfaSocketReceiveBuffer *) g_object_new(CFA_TYPE_SOCKET_RECEIVE_BUFFER, NULL);
}

/**
 * cfa_socket_receive_buffer_append:
 * @sock: a receive buffer.
 * @dat: string data.
 *
 * Puts the string data to the buffer.
 *
 * You own the string data.
 */
void cfa_socket_receive_buffer_append(CfaSocketReceiveBuffer *sock, const gchar *dat)
{
    guint len = strlen(dat);
    
    g_return_if_fail(CFA_IS_SOCKET_RECEIVE_BUFFER(sock));
    
    while (sock->datsiz + len + 1 > sock->buflen) {
	sock->buflen += sock->buflen / 2;
	sock->buf = g_renew(gchar, sock->buf, sock->buflen);
    }
    
    strcpy(sock->buf + sock->datsiz, dat);
    sock->datsiz += len;
    
    
    while (1) {
	gchar *p;
	gchar *str;
	guint l;
	
	p = strchr(sock->buf, '\n');
	if (p == NULL)
	    break;
	p++;
	
	l = p - sock->buf;
	str = g_new(gchar, l + 1);
	memcpy(str, sock->buf, l);
	str[l] = '\0';
	
	if (sock->datsiz == l) {
	    sock->datsiz = 0;
	    sock->buf[0] = '\0';
	} else {
	    memmove(sock->buf, sock->buf + l, sock->datsiz + 1 - l);
	    sock->datsiz -= l;
	}
	
	sock->list = g_list_append(sock->list, str);
    }
}

/**
 * cfa_socket_receive_buffer_has_lines:
 * @sock: a receive buffer.
 *
 * Check whether the buffer has string lines.
 *
 * Returns: %TRUE if the buffer has string lines.
 */
gboolean cfa_socket_receive_buffer_has_lines(CfaSocketReceiveBuffer *sock)
{
    g_return_val_if_fail(CFA_IS_SOCKET_RECEIVE_BUFFER(sock), FALSE);
    
    return sock->list != NULL;
}

/**
 * cfa_socket_receive_buffer_get_line:
 * @sock: a receive buffer.
 *
 * Get a line containing the line terminator from the receive buffer.
 *
 * You own the returned string.
 *
 * Returns: A string, or %NULL if no line.
 */
gchar *cfa_socket_receive_buffer_get_line(CfaSocketReceiveBuffer *sock)
{
    g_return_val_if_fail(CFA_IS_SOCKET_RECEIVE_BUFFER(sock), NULL);
    
    if (sock->list == NULL)
	return NULL;
    else {
	gchar *str = sock->list->data;
	sock->list = g_list_remove(sock->list, str);
	return str;
    }
}
