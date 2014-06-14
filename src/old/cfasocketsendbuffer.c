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
 * $Id: cfasocketsendbuffer.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <cfasocketsendbuffer.h>
#include <stdio.h>
#include <string.h>

G_DEFINE_TYPE(CfaSocketSendBuffer, cfa_socket_send_buffer, G_TYPE_OBJECT);

static void	cfa_socket_send_buffer_finalize		(GObject *object);

static void cfa_socket_send_buffer_class_init(CfaSocketSendBufferClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->finalize = cfa_socket_send_buffer_finalize;
}

static void cfa_socket_send_buffer_init(CfaSocketSendBuffer *sock)
{
    xlog_debug(1, "cfa_socket_send_buffer_init:");
    
    sock->datsiz = 0;
    sock->buflen = 1024;
    sock->buf = g_new(gchar, sock->buflen);
    sock->buf[0] = '\0';
}

static void cfa_socket_send_buffer_finalize(GObject *object)
{
    CfaSocketSendBuffer *sock;
    
    xlog_debug(1, "cfa_socket_send_buffer_finalize:");
    g_return_if_fail(CFA_IS_SOCKET_SEND_BUFFER(object));
    
    sock = CFA_SOCKET_SEND_BUFFER(object);
    
    sock->buflen = 0;
    sock->datsiz = 0;
    g_free(sock->buf);
    sock->buf = NULL;
}

/**
 * cfa_socket_send_buffer_new:
 *
 * Creates a new send buffer.
 * 
 * Returns: a new send buffer.
 */
CfaSocketSendBuffer *cfa_socket_send_buffer_new(void)
{
    return (CfaSocketSendBuffer *) g_object_new(CFA_TYPE_SOCKET_SEND_BUFFER, NULL);
}

/**
 * cfa_socket_send_buffer_append:
 * @sock: a send buffer.
 * @dat: string data.
 *
 * Puts the string data to the buffer.
 * 
 * You own the string data.
 */
void cfa_socket_send_buffer_append(CfaSocketSendBuffer *sock, const gchar *dat)
{
    guint len = strlen(dat);
    
    g_return_if_fail(CFA_IS_SOCKET_SEND_BUFFER(sock));
    
    while (sock->datsiz + len + 1 > sock->buflen) {
	sock->buflen += sock->buflen / 2;
	sock->buf = g_renew(gchar, sock->buf, sock->buflen);
    }
    
    strcpy(sock->buf + sock->datsiz, dat);
    sock->datsiz += len;
}

/**
 * cfa_socket_send_buffer_has_data:
 * @sock: a send buffer.
 *
 * Check whether the buffer has string data.
 * 
 * Returns: %TRUE if the buffer has string data.
 */
gboolean cfa_socket_send_buffer_has_data(CfaSocketSendBuffer *sock)
{
    g_return_val_if_fail(CFA_IS_SOCKET_SEND_BUFFER(sock), FALSE);
    
    return sock->datsiz > 0;
}

/**
 * cfa_socket_send_buffer_get_data_ptr:
 * @sock: a send buffer.
 * @size: pointer whether stores data length.
 *
 * Get the pointer to the buffered string data.
 *
 * The returned string is owned by the buffer. You shouldn't write to or free it.
 *
 * Returns: pointer to the string.
 */
const gchar *cfa_socket_send_buffer_get_data_ptr(CfaSocketSendBuffer *sock, guint *size)
{
    g_return_val_if_fail(CFA_IS_SOCKET_SEND_BUFFER(sock), NULL);
    
    *size = sock->datsiz;
    return sock->buf;
}

/**
 * cfa_socket_send_buffer_consume:
 * @sock: a send buffer.
 * @size: length.
 *
 * Delete the specified bytes of string from the head of the send buffer.
 */
void cfa_socket_send_buffer_consume(CfaSocketSendBuffer *sock, guint size)
{
    g_return_if_fail(CFA_IS_SOCKET_SEND_BUFFER(sock));
    
    if (size > sock->datsiz) {
	xlog_debug(1, "cfa_socket_send_buffer_consume: %d>%d.", size, sock->datsiz);
	size = sock->datsiz;
    }
    
    if (size == sock->datsiz) {
	sock->datsiz = 0;
    } else {
	memmove(sock->buf, sock->buf + size, sock->datsiz - size);
	sock->datsiz -= size;
    }
}

/*EOF*/
