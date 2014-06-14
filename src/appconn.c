/* XMMS2Applet
 *  Copyright (C) 2005-2006 Yuuki Harano
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
 */

#include <common.h>
#include <string.h>
#include <stdlib.h>
#include "appconn.h"
#include <xmmsclient/xmmsclient-glib.h>

G_DEFINE_TYPE(AppConn, app_conn, G_TYPE_OBJECT);

enum {
    SIG_PLAYTIME_CHANGED,
    SIG_PLAYSTAT_CHANGED,
    SIG_TITLE_CHANGED,
    SIG_NR
};

enum {
    PROP_0,
    PROP_CONF,
    PROP_NR
};

static void app_conn_dispose(GObject *object);
static void app_conn_finalize(GObject *object);
static void app_conn_set_property(GObject *object,
	guint prop_id, const GValue *value, GParamSpec *pspec);
static void app_conn_get_property(GObject *object,
	guint prop_id, GValue *value, GParamSpec *pspec);
static void app_conn_path_changed_cb(AppConf *conf, GParamSpec *arg1, gpointer user_data);
static void app_conn_set_playstat(AppConn *w, gint s);
static void app_conn_set_title(AppConn *w, const gchar *title);
static void app_conn_set_playtime(AppConn *w, guint sec);
static void app_conn_connect(AppConn *w);
static void app_conn_disconnect(AppConn *w);
static gboolean app_conn_timer_cb(gpointer data);

static guint signals[SIG_NR] = { 0 };

static void app_conn_init(AppConn *self)
{
    self->conf = NULL;
    self->conf_sig_id1 = self->conf_sig_id2 = 0;
    
    self->conn = NULL;
    self->conn_data = NULL;
    
    self->timer_id = g_timeout_add(100, app_conn_timer_cb, self);
    
    self->playstat = APP_CONN_PLAY_STATUS_BROKEN;
    self->play_time = 0;
    self->title = g_strdup("");
}

static void app_conn_class_init(AppConnClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    
    object_class->dispose = app_conn_dispose;
    object_class->finalize = app_conn_finalize;
    object_class->set_property = app_conn_set_property;
    object_class->get_property = app_conn_get_property;
    
    signals[SIG_PLAYTIME_CHANGED] = g_signal_new("playtime-changed",
	    G_OBJECT_CLASS_TYPE(klass),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__UINT,
	    G_TYPE_NONE, 1, G_TYPE_UINT);
    
    signals[SIG_PLAYSTAT_CHANGED] = g_signal_new("playstat-changed",
	    G_OBJECT_CLASS_TYPE(klass),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__INT,
	    G_TYPE_NONE, 1, G_TYPE_INT);
    
    signals[SIG_TITLE_CHANGED] = g_signal_new("title-changed",
	    G_OBJECT_CLASS_TYPE(klass),
	    G_SIGNAL_RUN_FIRST,
	    0,
	    NULL, NULL,
	    g_cclosure_marshal_VOID__STRING,
	    G_TYPE_NONE, 1, G_TYPE_STRING);
    
    g_object_class_install_property(object_class,
	    PROP_CONF,
	    g_param_spec_pointer("conf",
		    "Conf",
		    "config",
		    G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void app_conn_dispose(GObject *object)
{
    AppConn *w = APP_CONN(object);
    
    if (w->timer_id != 0) {
	g_source_remove(w->timer_id);
	w->timer_id = 0;
    }
    
    app_conn_disconnect(w);
    
    if (w->conf != NULL) {
	g_signal_handler_disconnect(w->conf, w->conf_sig_id1);
	g_signal_handler_disconnect(w->conf, w->conf_sig_id2);
	g_object_unref(w->conf);
	w->conf = NULL;
    }
    
    (*G_OBJECT_CLASS(app_conn_parent_class)->dispose)(object);
}

static void app_conn_finalize(GObject *object)
{
    AppConn *w = APP_CONN(object);
    
    if (w->title != NULL) {
	g_free(w->title);
	w->title = NULL;
    }
    
    (*G_OBJECT_CLASS(app_conn_parent_class)->finalize)(object);
}

static void app_conn_set_property(GObject *object,
	guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AppConn *w = APP_CONN(object);
    
    switch (prop_id) {
    case PROP_CONF:
	w->conf = g_value_get_pointer(value);
	g_object_ref(w->conf);
	w->conf_sig_id1 = g_signal_connect(w->conf, "notify::path-specified",
		G_CALLBACK(app_conn_path_changed_cb), w);
	w->conf_sig_id2 = g_signal_connect(w->conf, "notify::path",
		G_CALLBACK(app_conn_path_changed_cb), w);
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static void app_conn_get_property(GObject *object,
	guint prop_id, GValue *value, GParamSpec *pspec)
{
    AppConf *w = APP_CONF(object);
    
    switch (prop_id) {
    case PROP_CONF:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	break;
	
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	break;
    }
}

static void app_conn_path_changed_cb(AppConf *conf, GParamSpec *arg1, gpointer user_data)
{
    AppConn *w = user_data;
    
    app_conn_disconnect(w);
}

static void app_conn_lost_cb(void *data)
{
    AppConn *w = data;
    
    app_conn_disconnect(w);
}

static void app_conn_sig_playtime(xmmsc_result_t *res, void *user_data)
{
    AppConn *w = user_data;
    guint ptime;
    
    if (!xmmsc_result_iserror(res) && xmmsc_result_get_uint(res, &ptime)) {
	xmmsc_result_t *res2 = xmmsc_result_restart(res);
	xmmsc_result_unref(res2);
	
	app_conn_set_playtime(w, ptime / 1000);
    }
    
    xmmsc_result_unref(res);
}

static void app_conn_bc_playback_status(xmmsc_result_t *res, void *user_data)
{
    AppConn *w = user_data;
    guint st;
    
    if (!xmmsc_result_iserror(res) && xmmsc_result_get_uint(res, &st)) {
	switch (st) {
	case XMMS_PLAYBACK_STATUS_PAUSE:
	    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_PAUSE);
	    break;
	case XMMS_PLAYBACK_STATUS_PLAY:
	    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_PLAY);
	    break;
	case XMMS_PLAYBACK_STATUS_STOP:
	    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_STOP);
	    break;
	default:
	    break;
	}
    }
}

static void app_conn_cb_playback_status(xmmsc_result_t *res, void *user_data)
{
    AppConn *w = user_data;
    guint st;
    
    if (!xmmsc_result_iserror(res) && xmmsc_result_get_uint(res, &st)) {
	switch (st) {
	case XMMS_PLAYBACK_STATUS_PAUSE:
	    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_PAUSE);
	    break;
	    
	case XMMS_PLAYBACK_STATUS_PLAY:
	    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_PLAY);
	    break;
	    
	case XMMS_PLAYBACK_STATUS_STOP:
	    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_STOP);
	    break;
	    
	default:
	    break;
	}
    }
    
    xmmsc_result_unref(res);
}

static void app_conn_cb_get_info(xmmsc_result_t *res, void *user_data)
{
    AppConn *w = user_data;
    
    if (!xmmsc_result_iserror(res)) {
	gchar *str;
	
	gchar *title;
	if (!xmmsc_result_get_dict_entry_string(res, "title", &title))
	    title = "Unknown";
	
	gchar *artist;
	if (!xmmsc_result_get_dict_entry_string(res, "artist", &artist))
	    artist = NULL;
	
	if (artist != NULL) {
	    str = g_strdup_printf("%s - %s", artist, title);
	} else {
	    str = g_strdup(title);
	}
	
	app_conn_set_title(w, str);
	
	g_free(str);
    }
    
    xmmsc_result_unref(res);
}

static void app_conn_bc_current_id(xmmsc_result_t *res, void *user_data)
{
    AppConn *w = user_data;
    guint id;
    
    if (!xmmsc_result_iserror(res) && xmmsc_result_get_uint(res, &id)) {
	xmmsc_result_t *res2 = xmmsc_medialib_get_info(w->conn, id);
	xmmsc_result_notifier_set(res2, app_conn_cb_get_info, w);
	xmmsc_result_unref(res2);
    }
}

static void app_conn_cb_current_id(xmmsc_result_t *res, void *user_data)
{
    AppConn *w = user_data;
    guint id;
    
    if (!xmmsc_result_iserror(res) && xmmsc_result_get_uint(res, &id)) {
	xmmsc_result_t *res2 = xmmsc_medialib_get_info(w->conn, id);
	xmmsc_result_notifier_set(res2, app_conn_cb_get_info, w);
	xmmsc_result_unref(res2);
    }
    
    xmmsc_result_unref(res);
}

static void app_conn_set_playstat(AppConn *w, gint s)
{
    if (s != w->playstat) {
	w->playstat = s;
	g_signal_emit(w, signals[SIG_PLAYSTAT_CHANGED], 0, s);
    }
}

static void app_conn_set_title(AppConn *w, const gchar *title)
{
    if (strcmp(w->title, title) != 0) {
	g_free(w->title);
	w->title = g_strdup(title);
	g_signal_emit(w, signals[SIG_TITLE_CHANGED], 0, w->title);
    }
}

static void app_conn_set_playtime(AppConn *w, guint sec)
{
    if (sec != w->play_time) {
	w->play_time = sec;
	g_signal_emit(w, signals[SIG_PLAYTIME_CHANGED], 0, sec);
    }
}

static void app_conn_connect(AppConn *w)
{
    if (w->conn != NULL)
	return;
    
    w->conn = xmmsc_init("XMMS2Applet");
    
    char *path;
    
    if (app_conf_get_path_specified(w->conf)) {
	path = app_conf_get_path(w->conf);
	gboolean r = xmmsc_connect(w->conn, path);
	g_free(path);
	if (r)
	    goto conn_successful;
    } else {
	path = getenv("XMMS_PATH");
	if (path != NULL) {
	    if (xmmsc_connect(w->conn, path))
		goto conn_successful;
	}
	
	if (xmmsc_connect(w->conn, NULL))
	    goto conn_successful;
    }
    
    xmmsc_unref(w->conn);
    w->conn = NULL;
    return;
    
 conn_successful:
    
    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_STOP);
    
    XMMS_CALLBACK_SET(w->conn, xmmsc_broadcast_playback_status, app_conn_bc_playback_status, w);
    XMMS_CALLBACK_SET(w->conn, xmmsc_broadcast_playback_current_id, app_conn_bc_current_id, w);
    XMMS_CALLBACK_SET(w->conn, xmmsc_signal_playback_playtime, app_conn_sig_playtime, w);
    XMMS_CALLBACK_SET(w->conn, xmmsc_playback_current_id, app_conn_cb_current_id, w);
    XMMS_CALLBACK_SET(w->conn, xmmsc_playback_status, app_conn_cb_playback_status, w);
    xmmsc_disconnect_callback_set(w->conn, app_conn_lost_cb, w);
    
    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_STOP);
    app_conn_set_title(w, "");
    app_conn_set_playtime(w, 0);
    
    w->conn_data = xmmsc_mainloop_gmain_init(w->conn);
}

static void app_conn_disconnect(AppConn *w)
{
    app_conn_set_playstat(w, APP_CONN_PLAY_STATUS_BROKEN);
    
    if (w->conn == NULL)
	return;
    
    if (w->conn_data != NULL) {
	xmmsc_mainloop_gmain_shutdown(w->conn, w->conn_data);
	w->conn_data = NULL;
    }
    
    if (w->conn != NULL) {
	/* fixme: 消えてくれない場合あり?
	 *        play_or_pause() の中で path_specified の設定を変更したら、
	 *        切れてくれなかった。
	 */
	xmmsc_unref(w->conn);
	w->conn = NULL;
    }
}

static gboolean app_conn_timer_cb(gpointer data)
{
    AppConn *w = data;
    
    if (w->conn == NULL)
	app_conn_connect(w);
    
    return TRUE;
}

static void app_conn_cb_play_or_stop(xmmsc_result_t *res, void *user_data)
{
    AppConn *w = user_data;
    
    (void) w;
    
    xmmsc_result_unref(res);
}

void app_conn_play_or_stop(AppConn *w)
{
    if (w->conn != NULL) {
	xmmsc_result_t *res;
	switch (w->playstat) {
	case APP_CONN_PLAY_STATUS_PLAY:
	    res = xmmsc_playback_stop(w->conn);
	    break;
	    
	case APP_CONN_PLAY_STATUS_PAUSE:
	case APP_CONN_PLAY_STATUS_STOP:
	    res = xmmsc_playback_start(w->conn);
	    break;
	    
	default:
	    res = NULL;
	}
	
	if (res != NULL) {
	    xmmsc_result_notifier_set(res, app_conn_cb_play_or_stop, w);
	    xmmsc_result_unref(res);
	}
    }
}

AppConn *app_conn_new(AppConf *conf)
{
    AppConn *w;
    
    w = g_object_new(APP_TYPE_CONN,
	    "conf", conf,
	    NULL);
    
    return w;
}

/*EOF*/
