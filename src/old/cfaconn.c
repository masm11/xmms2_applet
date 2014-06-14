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
 * $Id: cfaconn.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include "cfaconn.h"
#include <free_string_list.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

G_DEFINE_TYPE(CfaConn, cfa_conn, G_TYPE_OBJECT);

enum {
    SIG_HAS_LINES,
    SIG_HAS_STATES,
    SIG_CLOSED,
    SIG_NR
};

static void	cfa_conn_dispose	(GObject *object);

static void	free_resources		(CfaConn *conn);

static void	cfa_conn_cb_has_lines	(CfaClientSocket *sock,
					 gpointer data);
static void	cfa_conn_cb_closed	(CfaClientSocket *sock,
					 gpointer data);

static GSource	*cfa_conn_source_new	(CfaConn *conn);

static guint signals[SIG_NR] = { 0 };

GQuark cfa_conn_error_quark(void)
{
    static GQuark quark = 0;
    if (quark == 0)
	quark = g_quark_from_static_string("cfa-conn-error-quark");
    return quark;
}

static void cfa_conn_class_init(CfaConnClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->dispose = cfa_conn_dispose;
    
    signals[SIG_HAS_LINES] = g_signal_new ("has-lines",
	    G_OBJECT_CLASS_TYPE(object_class),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);
    
    signals[SIG_HAS_STATES] = g_signal_new ("has-states",
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

static void cfa_conn_init(CfaConn *conn)
{
    xlog_debug(1, "cfa_conn_init:");
}

static void cfa_conn_dispose(GObject *object)
{
    CfaConn *conn;
    
    xlog_debug(1, "cfa_conn_dispose:");
    
    g_return_if_fail(CFA_IS_CONN(object));
    conn = CFA_CONN(object);
    
    free_resources(conn);
}

/**
 * cfa_conn_new:
 *
 * Creates a new connection to the daemon.
 * It is not connected yet. You need to connect by cfa_conn_connect() or cfa_conn_connect_sys().
 * 
 * Returns: a new connection.
 */
CfaConn *cfa_conn_new(void)
{
    CfaConn *conn;
    
    conn = g_object_new(CFA_TYPE_CONN, NULL);
    
    return conn;
}

/**
 * cfa_conn_destroy:
 * @conn: a connection.
 *
 * Destroy the connection.
 */
void cfa_conn_destroy(CfaConn *conn)
{
    xlog_debug(1, "cfa_conn_destroy:");
    
    g_return_if_fail(CFA_IS_CONN(conn));
    
    free_resources(conn);
    
    g_object_unref(conn);
}

static void free_resources(CfaConn *conn)
{
    if (conn->source != NULL) {
	xlog_debug(1, "free_resources: src.ref_cnt=%d.", conn->source->ref_count);
	g_source_destroy(conn->source);
	xlog_debug(1, "free_resources: src.ref_cnt=%d.", conn->source->ref_count);
	g_source_unref(conn->source);
	xlog_debug(1, "free_resources: src.ref_cnt=%d.", conn->source->ref_count);
	
	conn->source = NULL;
    }
    
    if (conn->sock != NULL) {
	cfa_client_socket_destroy(conn->sock);
	conn->sock = NULL;
    }
    
    if (conn->failed_mesg) {
	g_free(conn->failed_mesg);
	conn->failed_mesg = NULL;
    }
    
    if (conn->state != NULL) {
	g_object_unref(conn->state);
	conn->state = NULL;
    }
    
    if (conn->state0 != NULL) {
	g_object_unref(conn->state0);
	conn->state0 = NULL;
    }
    
    if (conn->other_strings != NULL) {
	cfa_free_string_list(conn->other_strings);
	conn->other_strings = NULL;
    }
}

/**
 * cfa_conn_connect:
 * @conn: a connection.
 * @err: pointer where stores an error.
 *
 * Connects to the daemon with the normal privilege.
 */
void cfa_conn_connect(CfaConn *conn, GError **err)
{
    conn->sock = cfa_client_socket_new_with_connect(CPUFREQD_PATH_USER, err);
    
    if (*err != NULL)
	return;
    
    g_signal_connect(G_OBJECT(conn->sock), "has-lines", G_CALLBACK(cfa_conn_cb_has_lines), conn);
    g_signal_connect(G_OBJECT(conn->sock), "closed", G_CALLBACK(cfa_conn_cb_closed), conn);
    
    conn->source = cfa_conn_source_new(conn);
    g_source_set_can_recurse(conn->source, TRUE);
    g_source_attach(conn->source, NULL);
}

/**
 * cfa_conn_connect_sys:
 * @conn: a connection.
 * @err: pointer where stores an error.
 *
 * Connects to the daemon with the system privilege.
 */
void cfa_conn_connect_sys(CfaConn *conn, GError **err)
{
    conn->sock = cfa_client_socket_new_with_connect(CPUFREQD_PATH_SYS, err);
    
    if (*err != NULL)
	return;
    
    g_signal_connect(G_OBJECT(conn->sock), "has-lines", G_CALLBACK(cfa_conn_cb_has_lines), conn);
    g_signal_connect(G_OBJECT(conn->sock), "closed", G_CALLBACK(cfa_conn_cb_closed), conn);
    
    conn->source = cfa_conn_source_new(conn);
    g_source_set_can_recurse(conn->source, TRUE);
    g_source_attach(conn->source, NULL);
}

/**
 * cfa_conn_send:
 * @conn: a connection.
 * @req: a request string.
 * @err: pointer where stores an error.
 *
 * Sends the request, waits for the response, and returns the result status.
 * 
 * Returns: %TRUE if the request is succeeded, or %FALSE if failed.
 */
gboolean cfa_conn_send(CfaConn *conn, const gchar *req, GError **err)
{
    xlog_debug(1, "cfa_conn_send: %s", req);
    
    cfa_client_socket_send_line(conn->sock, req);
    
    conn->loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(conn->loop);
    g_main_loop_unref(conn->loop);
    conn->loop = NULL;
    
    if (!conn->result) {
	g_set_error(err, CFA_CONN_ERROR,
		CFA_CONN_ERROR_FAILED,
		"%s", conn->failed_mesg);
    }
    
    xlog_debug(1, "cfa_conn_send: done.%d.", conn->result);
    return conn->result;
}

static void cfa_conn_cb_has_lines(CfaClientSocket *sock, gpointer data)
{
    CfaConn *conn = data;
    gchar *p;
    xlog_debug(1, "has_lines: sock=%p, data=%p.\n", sock, data);
    while ((p = cfa_client_socket_recv_line(sock)) != NULL) {
	CfaStringTokenizer *stk;
	gchar *cmd;
	xlog_debug(1, "has_lines: %s", p);
	stk = cfa_string_tokenizer_new(p);
	cmd = cfa_string_tokenizer_next(stk);
	if (cmd == NULL)
	    goto error;		// all space characters
	xlog_debug(1, "cmd=\"%s\"", cmd);
	if (strcmp(cmd, "state") == 0) {
	    const gchar *sub1 = cfa_string_tokenizer_peek(stk);
	    if (sub1 == NULL)
		goto error;	// no more tokens.
	    
	    if (strcmp(sub1, "begin") == 0) {
		if (conn->state != NULL)
		    g_object_unref(conn->state);
		conn->state = cfa_state_new();
	    } else if (strcmp(sub1, "end") == 0) {
		int i;
		
		if (conn->state0 != NULL)
		    g_object_unref(conn->state0);
		conn->state0 = conn->state;
		conn->state = NULL;
		
#if 0
		if (conn->initializing)
		    g_main_loop_quit(conn->loop);
#endif
		
		xlog_debug(1, "cpu_nr=%d", cfa_cpu_states_get_nr(conn->state0->cpu_states));
		
		for (i = 0; i < conn->state0->cpu_states->nr; i++) {
		    freq_t cur, min, max;
		    const freq_t *avail;
		    cfa_cpu_states_get_freqs(conn->state0->cpu_states, i, &cur, &min, &max, &avail);
		    xlog_debug(1, "cpu #%d: %lld %lld %lld", i, cur, min, max);
		}
	    } else {
		cfa_state_add(conn->state, stk);
	    }
	} else if (strcmp(cmd, "OK") == 0) {
	    conn->result = TRUE;
	    
	    if (conn->loop != NULL)
		g_main_loop_quit(conn->loop);
	} else if (strcmp(cmd, "FAILED") == 0) {
	    conn->result = FALSE;
	    
	    if (conn->failed_mesg != NULL)
		g_free(conn->failed_mesg);
	    conn->failed_mesg = g_strdup(p + 7);
	    
	    if (conn->loop != NULL)
		g_main_loop_quit(conn->loop);
	} else {
	    conn->other_strings = g_list_append(conn->other_strings, p);
	}
	
     error:
	g_object_unref(stk);
	g_free(p);
    }
}

static void cfa_conn_cb_closed(CfaClientSocket *sock, gpointer data)
{
    CfaConn *conn = data;
    xlog_debug(1, "closed: sock=%p, data=%p.\n", sock, data);
    cfa_client_socket_close(sock);
    conn->sock_closed = TRUE;
}

/**
 * cfa_conn_get_state:
 * @conn: a connection.
 *
 * Get the state, previously received from the daemon.
 * 
 * You own the state.
 * 
 * Returns: the state, or %NULL if none.
 */
CfaState *cfa_conn_get_state(CfaConn *conn)
{
    CfaState *s = conn->state0;
    conn->state0 = NULL;
    return s;
}

/**
 * cfa_conn_get_line:
 * @conn: a connection.
 *
 * Get a result string.
 * 
 * Returns: the result string, or %NULL if none.
 */
gchar *cfa_conn_get_line(CfaConn *conn)
{
    if (conn->other_strings == NULL)
	return NULL;
    else {
	gchar *s = conn->other_strings->data;
	conn->other_strings =
		g_list_delete_link(conn->other_strings, conn->other_strings);
	return s;
    }
}

/****************************************************************/

struct cfa_conn_source_t {
    GSource source;
    
    CfaConn *conn;
};

static gboolean	cfa_conn_source_prepare	(GSource *source,
					 gint *timeout_);
static gboolean	cfa_conn_source_check	(GSource *source);
static gboolean	cfa_conn_source_dispatch(GSource *source,
					 GSourceFunc callback,
					 gpointer user_data);
static void	cfa_conn_source_finalize(GSource *source);

static GSourceFuncs cfa_conn_source_funcs = {
    cfa_conn_source_prepare,
    cfa_conn_source_check,
    cfa_conn_source_dispatch,
    cfa_conn_source_finalize,
};

static GSource *cfa_conn_source_new(CfaConn *conn)
{
    GSource *source = g_source_new(
	    &cfa_conn_source_funcs,
	    sizeof(struct cfa_conn_source_t));
    struct cfa_conn_source_t *src = (struct cfa_conn_source_t *) source;
    xlog_debug(1, "cfa_conn_source_new:");
    
    src->conn = conn;
    g_object_ref(conn);
    
    return source;
}

static gboolean cfa_conn_source_prepare(GSource *source, gint *timeout_)
{
    struct cfa_conn_source_t *src = (struct cfa_conn_source_t *) source;
    CfaConn *conn = src->conn;
    
    xlog_debug(1, "cfa_conn_source_prepare:");
    xlog_debug(1, "cfa_conn_source_prepare: ref_cnt=%d.", source->ref_count);
    
    *timeout_ = -1;
    
    if (conn->state0 != NULL)
	return TRUE;
    
    if (conn->other_strings != NULL)
	return TRUE;
    
    if (conn->sock_closed)
	return TRUE;
    
    return FALSE;
}

static gboolean cfa_conn_source_check(GSource *source)
{
    struct cfa_conn_source_t *src = (struct cfa_conn_source_t *) source;
    CfaConn *conn = src->conn;
    
    xlog_debug(1, "cfa_conn_source_check:");
    
    if (conn->state0 != NULL)
	return TRUE;
    
    if (conn->other_strings != NULL)
	return TRUE;
    
    if (conn->sock_closed)
	return TRUE;
    
    return FALSE;
}

static gboolean cfa_conn_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
    struct cfa_conn_source_t *src = (struct cfa_conn_source_t *) source;
    CfaConn *conn = src->conn;
    
    xlog_debug(1, "cfa_conn_source_dispatch:%p,%p", callback, user_data);
    
    if (conn->state0 != NULL) {
	g_signal_emit(conn, signals[SIG_HAS_STATES], 0);
    } else if (conn->other_strings != NULL) {
	g_signal_emit(conn, signals[SIG_HAS_LINES], 0);
    } else if (conn->sock_closed) {
	g_signal_emit(conn, signals[SIG_CLOSED], 0);
    }
    
    return TRUE;
}

static void cfa_conn_source_finalize(GSource *source)
{
    struct cfa_conn_source_t *src = (struct cfa_conn_source_t *) source;
    xlog_debug(1, "cfa_conn_source_finalize:");
    
    if (src->conn != NULL) {
	g_object_unref(src->conn);
	src->conn = NULL;
    }
}

/*EOF*/
