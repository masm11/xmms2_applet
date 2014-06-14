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
 * $Id: cfaclientsocket.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cpufreq/connection.h>
#include <cpufreq/mem.h>
#include <cfaclientsocket.h>

enum {
    SIG_HAS_LINES,
    SIG_CLOSED,
    SIG_NR
};

G_DEFINE_TYPE(CfaClientSocket, cfa_client_socket, CFA_TYPE_SOCKET);

static void	cfa_client_socket_dispose	(GObject *object);
static void	cfa_client_socket_finalize	(GObject *object);
static void	free_resources			(CfaClientSocket *client_socket);

static GSource	*cfa_client_socket_source_new	(CfaClientSocket *client_socket);

static guint signals[SIG_NR] = { 0 };

GQuark cfa_client_socket_error_quark(void)
{
    static GQuark quark = 0;
    if (quark == 0)
	quark = g_quark_from_static_string("cfa-client-socket-error-quark");
    return quark;
}

static void cfa_client_socket_class_init(CfaClientSocketClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->dispose = cfa_client_socket_dispose;
    object_class->finalize = cfa_client_socket_finalize;
    
    signals[SIG_HAS_LINES] = g_signal_new ("has-lines",
	    G_OBJECT_CLASS_TYPE(object_class),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
    
    signals[SIG_CLOSED] = g_signal_new ("closed",
	    G_OBJECT_CLASS_TYPE(object_class),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
}

static void cfa_client_socket_init(CfaClientSocket *client_socket)
{
    xlog_debug(1, "cfa_client_socket_init:");
    
    client_socket->recv_buf = cfa_socket_receive_buffer_new();
    client_socket->send_buf = cfa_socket_send_buffer_new();
    
    client_socket->source = cfa_client_socket_source_new(client_socket);
    xlog_debug(1, "cfa_client_socket_init: src.ref_cnt=%d.", client_socket->source->ref_count);
    client_socket->pollfd.fd = -1;
    g_source_set_can_recurse(client_socket->source, TRUE);
    xlog_debug(1, "cfa_client_socket_init: src.ref_cnt=%d.", client_socket->source->ref_count);
    g_source_add_poll(client_socket->source, &client_socket->pollfd);
    xlog_debug(1, "cfa_client_socket_init: src.ref_cnt=%d.", client_socket->source->ref_count);
    g_source_attach(client_socket->source, NULL);
    xlog_debug(1, "cfa_client_socket_init: src.ref_cnt=%d.", client_socket->source->ref_count);
}

static void cfa_client_socket_dispose(GObject *object)
{
    xlog_debug(1, "cfa_client_socket_dispose:");
    g_return_if_fail(CFA_IS_CLIENT_SOCKET(object));
    
    free_resources(CFA_CLIENT_SOCKET(object));
    
    (*G_OBJECT_CLASS(cfa_client_socket_parent_class)->dispose)(object);
}

static void cfa_client_socket_finalize(GObject *object)
{
    CfaClientSocket *client_socket;
    
    xlog_debug(1, "cfa_client_socket_finalize:");
    
    g_return_if_fail(CFA_IS_CLIENT_SOCKET(object));
    
    client_socket = CFA_CLIENT_SOCKET(object);
    
    if (client_socket->path != NULL) {
	g_free(client_socket->path);
	client_socket->path = NULL;
    }
    
    (*G_OBJECT_CLASS(cfa_client_socket_parent_class)->finalize)(object);
}

static void free_resources(CfaClientSocket *client_socket)
{
    if (client_socket->source != NULL) {
	xlog_debug(1, "free_resources: src.ref_cnt=%d.", client_socket->source->ref_count);
	g_source_destroy(client_socket->source);
	xlog_debug(1, "free_resources: src.ref_cnt=%d.", client_socket->source->ref_count);
	g_source_unref(client_socket->source);
	xlog_debug(1, "free_resources: src.ref_cnt=%d.", client_socket->source->ref_count);
	
	client_socket->source = NULL;
    }
    
    if (client_socket->recv_buf != NULL) {
	g_object_unref(client_socket->recv_buf);
	client_socket->recv_buf = NULL;
    }
    
    if (client_socket->send_buf != NULL) {
	g_object_unref(client_socket->send_buf);
	client_socket->send_buf = NULL;
    }
}

/**
 * cfa_client_socket_new_with_connect:
 * @path: path of unix domain socket.
 * @err: pointer where returns error.
 * 
 * Creates a new connected socket.
 * 
 * Returns: a socket.
 */
CfaClientSocket *cfa_client_socket_new_with_connect(const gchar *path, GError **err)
{
    CfaClientSocket *client_socket;
    char *errmsg = NULL;
    
    xlog_debug(1, "cfa_client_socket_new_with_connect: %s", path);
    
    int s = connection_make(0, 1, &errmsg);
    
    if (s == -1) {
	g_set_error(err, CFA_CLIENT_SOCKET_ERROR,
		CFA_CLIENT_SOCKET_ERROR_SOCKET,
		"%s", errmsg);
	xfree(errmsg);
	return NULL;
    }
    xlog_debug(1, "cfa_client_socket_new_with_connect: %d", s);
    
    client_socket = g_object_new(CFA_TYPE_CLIENT_SOCKET,
	    "fd", s,
	    NULL);
    
    xlog_debug(1, "cfa_client_socket_new_with_connect: ref_cnt=%d.", client_socket->source->ref_count);
    
    return client_socket;
}

/**
 * cfa_client_socket_new_with_fd:
 * @fd: file descriptor of connected socket.
 * 
 * Creates a new socket.
 * 
 * Returns: a socket.
 */
CfaClientSocket *cfa_client_socket_new_with_fd(int fd)
{
    CfaClientSocket *client_socket;
    
    client_socket = g_object_new(CFA_TYPE_CLIENT_SOCKET,
	    "fd", fd,
	    NULL);
    
    return client_socket;
}

/**
 * cfa_client_socket_close:
 * @client_socket: a socket.
 * 
 * Close the socket.
 */
void cfa_client_socket_close(CfaClientSocket *client_socket)
{
    xlog_debug(1, "cfa_client_socket_close:");
    
    g_return_if_fail(CFA_IS_CLIENT_SOCKET(client_socket));
    
    xlog_debug(1, "cfa_client_socket_close: src.ref_cnt=%d.", client_socket->source->ref_count);
    
    client_socket->no_more_send = TRUE;
}

/**
 * cfa_client_socket_destroy:
 * @client_socket: a socket.
 * 
 * Destroy and free the socket.
 */
void cfa_client_socket_destroy(CfaClientSocket *client_socket)
{
    xlog_debug(1, "cfa_client_socket_destroy:");
    
    g_return_if_fail(CFA_IS_CLIENT_SOCKET(client_socket));
    
    free_resources(client_socket);
    
    g_object_unref(client_socket);
}

/**
 * cfa_client_socket_recv_line:
 * @client_socket: a socket.
 * 
 * Get a string from the socket. It is splitted by line terminator.
 * 
 * You own the returned string.
 * 
 * Returns: string containing a line terminator, or %NULL if no string.
 */
gchar *cfa_client_socket_recv_line(CfaClientSocket *client_socket)
{
    g_return_val_if_fail(CFA_IS_CLIENT_SOCKET(client_socket), NULL);
    
    return cfa_socket_receive_buffer_get_line(client_socket->recv_buf);
}

/**
 * cfa_client_socket_send_line:
 * @client_socket: a socket.
 * @data: a string.
 * 
 * Send a string to the socket. It is queued, and sent in later.
 * 
 * You own the data.
 */
void cfa_client_socket_send_line(CfaClientSocket *client_socket, const gchar *data)
{
    g_return_if_fail(CFA_IS_CLIENT_SOCKET(client_socket));
    
    if (client_socket->no_more_send) {
	g_warning("cfa_client_socket_send_line: already shutdown.");
	return;
    }
    
    cfa_socket_send_buffer_append(client_socket->send_buf, data);
}

/**
 * cfa_client_socket_send_shutdown:
 * @client_socket: a socket.
 * 
 * Shutdown the socket's output.
 */
void cfa_client_socket_send_shutdown(CfaClientSocket *client_socket)
{
    g_return_if_fail(CFA_IS_CLIENT_SOCKET(client_socket));
    
    client_socket->no_more_send = TRUE;
    if (!cfa_socket_send_buffer_has_data(client_socket->send_buf)) {
	shutdown(client_socket->socket.fd, SHUT_WR);
    } else {
	// shutdown later in cfa_client_socket_source_dispatch().
    }
}

/****************************************************************/

struct cfa_client_socket_source_t {
    GSource source;
    
    CfaClientSocket *client_socket;
};

static gboolean	cfa_client_socket_source_prepare	(GSource *source,
							 gint *timeout_);
static gboolean	cfa_client_socket_source_check		(GSource *source);
static gboolean	cfa_client_socket_source_dispatch	(GSource *source,
							 GSourceFunc callback,
							 gpointer user_data);
static void	cfa_client_socket_source_finalize	(GSource *source);

static GSourceFuncs cfa_client_socket_source_funcs = {
    cfa_client_socket_source_prepare,
    cfa_client_socket_source_check,
    cfa_client_socket_source_dispatch,
    cfa_client_socket_source_finalize,
};

static GSource *cfa_client_socket_source_new(CfaClientSocket *client_socket)
{
    GSource *source = g_source_new(
	    &cfa_client_socket_source_funcs,
	    sizeof(struct cfa_client_socket_source_t));
    struct cfa_client_socket_source_t *src = (struct cfa_client_socket_source_t *) source;
    xlog_debug(1, "cfa_client_socket_source_new:");
    
    src->client_socket = client_socket;
    g_object_ref(client_socket);
    
    return source;
}

static gboolean cfa_client_socket_source_prepare(GSource *source, gint *timeout_)
{
    struct cfa_client_socket_source_t *src = (struct cfa_client_socket_source_t *) source;
    CfaClientSocket *client_socket = src->client_socket;
    
    xlog_debug(1, "cfa_client_socket_source_prepare:");
    xlog_debug(1, "cfa_client_socket_source_prepare: ref_cnt=%d.", source->ref_count);
    
    client_socket->pollfd.fd = -1;
    client_socket->pollfd.events = 0;
    client_socket->pollfd.revents = 0;
    
    if (!client_socket->no_more_recv) {
	client_socket->pollfd.events |= G_IO_IN;
	xlog_debug(1, "cfa_client_socket_source_prepare: fd=%d", client_socket->socket.fd);
    } else {
	xlog_debug(1, "cfa_client_socket_source_prepare: none.");
    }
    
    if (cfa_socket_send_buffer_has_data(client_socket->send_buf))
	client_socket->pollfd.events |= G_IO_OUT;
    
    if (client_socket->pollfd.events)
	client_socket->pollfd.fd = client_socket->socket.fd;
    
    *timeout_ = -1;
    
    if (cfa_socket_receive_buffer_has_lines(client_socket->recv_buf))
	return TRUE;
    if (client_socket->no_more_recv)
	return TRUE;
    if (client_socket->no_more_send && !cfa_socket_send_buffer_has_data(client_socket->send_buf))
	return TRUE;
    
    return FALSE;
}

static gboolean cfa_client_socket_source_check(GSource *source)
{
    struct cfa_client_socket_source_t *src = (struct cfa_client_socket_source_t *) source;
    CfaClientSocket *client_socket = src->client_socket;
	
    xlog_debug(1, "cfa_client_socket_source_check:");
    
    if (cfa_socket_receive_buffer_has_lines(client_socket->recv_buf))
	return TRUE;
    if (client_socket->no_more_recv)
	return TRUE;		// 閉じられた。
    if (client_socket->no_more_send && !cfa_socket_send_buffer_has_data(client_socket->send_buf))
	return TRUE;
    
    xlog_debug(1, "cfa_client_socket_source_check:");
    return !!(client_socket->pollfd.revents & (G_IO_IN | G_IO_OUT));
}

static gboolean cfa_client_socket_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
    struct cfa_client_socket_source_t *src = (struct cfa_client_socket_source_t *) source;
    CfaClientSocket *client_socket = src->client_socket;
    
    xlog_debug(1, "cfa_client_socket_source_dispatch:%p,%p", callback, user_data);
    xlog_debug(1, "cfa_client_socket_source_dispatch:fd=%d.", client_socket->socket.fd);
    
    if (client_socket->pollfd.revents & G_IO_IN) {
	gchar buf[1024];
	gint len;
	
	xlog_debug(1, "cfa_client_socket_source_dispatch: read:");
	len = read(client_socket->socket.fd, buf, sizeof buf - 1);
	xlog_debug(1, "cfa_client_socket_source_dispatch: read: len=%d.", len);
	
	if (len == -1) {
	    if (errno == EINTR)
		;
	    else if (errno == ECONNRESET) {
		xlog_debug(1, "cfa_client_socket_source_dispatch: reset.");
		client_socket->no_more_recv = TRUE;
	    } else {
		g_warning("read: %s", g_strerror(errno));
	    }
	} else if (len == 0) {
	    xlog_debug(1, "cfa_client_socket_source_dispatch: closed.");
	    client_socket->no_more_recv = TRUE;
	} else {
	    buf[len] = '\0';
	    cfa_socket_receive_buffer_append(client_socket->recv_buf, buf);
	}
    }
    
    if (client_socket->pollfd.revents & G_IO_OUT) {
	const gchar *buf;
	guint bufsiz;
	gint len;
	
	buf = cfa_socket_send_buffer_get_data_ptr(client_socket->send_buf, &bufsiz);
	xlog_debug(1, "cfa_client_socket_source_dispatch: write:");
	len = write(client_socket->socket.fd, buf, bufsiz);
	xlog_debug(1, "cfa_client_socket_source_dispatch: write: len=%d.", len);
	
	if (len == -1) {
	    if (errno == EINTR)
		;
	    else if (errno == EPIPE)
		;
	    else {
		g_warning("write: %s", g_strerror(errno));
	    }
	} else if (len == 0) {
	    xlog_debug(1, "cfa_client_socket_source_dispatch: eof.");
	    cfa_socket_send_buffer_consume(client_socket->send_buf, bufsiz);
	} else {
	    cfa_socket_send_buffer_consume(client_socket->send_buf, len);
	    if (client_socket->no_more_send) {
		if (!cfa_socket_send_buffer_has_data(client_socket->send_buf)) {
		    shutdown(client_socket->socket.fd, SHUT_WR);
		}
	    }
	}
    }
    
    if (client_socket->no_more_send) {
	xlog_debug(1, "cfa_client_socket_source_dispatch: no more send.");
	if (!cfa_socket_send_buffer_has_data(client_socket->send_buf)) {
	    xlog_debug(1, "cfa_client_socket_source_dispatch: destroy.");
	    xlog_debug(1, "cfa_client_socket_source_dispatch: ref_cnt=%d.", source->ref_count);
	    cfa_client_socket_destroy(client_socket);
	    xlog_debug(1, "cfa_client_socket_source_dispatch: ref_cnt=%d.", source->ref_count);
	    return FALSE;
	}
	
	return TRUE;
    }
    
    if (cfa_socket_receive_buffer_has_lines(client_socket->recv_buf)) {
	g_signal_emit(client_socket, signals[SIG_HAS_LINES], 0);
    } else {
	if (client_socket->no_more_recv) {
	    g_signal_emit(client_socket, signals[SIG_CLOSED], 0);
	}
    }
    
    return TRUE;
}

static void cfa_client_socket_source_finalize(GSource *source)
{
    struct cfa_client_socket_source_t *src = (struct cfa_client_socket_source_t *) source;
    xlog_debug(1, "cfa_client_socket_source_finalize:");
    
    if (src->client_socket != NULL) {
	g_object_unref(src->client_socket);
	src->client_socket = NULL;
    }
}

/*EOF*/
