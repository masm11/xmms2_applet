#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([XMMS2 Applet], [0.5], [masm@flowernet.gr.jp])
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AC_CONFIG_SRCDIR([XMMS2Applet.xml])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/socket.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memmove memset socket strchr strrchr strtol])

#
AC_PREFIX_PROGRAM(gnome-panel)

#
PKG_CHECK_MODULES(XMMS2, xmms2-client >= 0.2 xmms2-client-glib >= 0.2)

#
# G_DEFINE_TYPE is since glib-2.4.0.
PKG_CHECK_MODULES(GSTUFF, libpanelapplet-2.0 libgtop-2.0 libgnomeui-2.0 glib-2.0 >= 2.4.0 gobject-2.0 >= 2.4.0)
PKG_CHECK_MODULES(GSTUFF_APPLET, libpanelapplet-2.0 libgnomeui-2.0 glib-2.0 >= 2.4.0 gobject-2.0 >= 2.4.0)
PKG_CHECK_MODULES(GSTUFF_DAEMON, libgtop-2.0 glib-2.0 >= 2.4.0 gobject-2.0 >= 2.4.0)
PKG_CHECK_MODULES(GSTUFF_CMDTOOL, glib-2.0 >= 2.4.0 gobject-2.0 >= 2.4.0)
AC_PATH_PROG(GCONFTOOL, gconftool-2, no)
if test x"$GCONFTOOL" = xno; then
  AC_MSG_ERROR([gconftool-2 executable not found in your path - should be installed with GConf])
fi
AM_GCONF_SOURCE_2

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
