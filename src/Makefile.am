#

libexec_PROGRAMS = xmms2_applet

xmms2_applet_SOURCES =	\
  common.h			\
  xmms2_applet.c \
  app.h \
  appconn.c \
  appconn.h \
  appscrolledtext.c \
  appscrolledtext.h \
  about.c \
  about.h \
  appconf.c \
  appconf.h \
  apppref.c \
  apppref.h

XMMS2_CFLAGS = @XMMS2_CFLAGS@
XMMS2_LIBS = @XMMS2_LIBS@

AM_CPPFLAGS = $(GSTUFF_APPLET_CFLAGS) $(XMMS2_CFLAGS) -I$(srcdir)
AM_CFLAGS = -Wall

LDADD = $(GSTUFF_APPLET_LIBS) $(XMMS2_LIBS)
