#!/bin/sh

distdir="$1"
top_srcdir="$2"

if ( cd $top_srcdir; svn status ) 2>&1 | fgrep -v "$distdir" | grep .; then
    echo bad svn status >&2
    exit 1
else
    :
fi

find $distdir			\
  -name autom4te.cache -prune -o	\
  -name .svn -prune -o	\
  -type f		\
  \! -name '*~'		\
  \! -name Makefile.in	\
  \! -name configure	\
  \! -name config.h.in	\
  \! -name aclocal.m4	\
  \! -name INSTALL	\
  \! -name COPYING	\
  \! -name depcomp	\
  \! -name compile	\
  \! -name install-sh	\
  \! -name missing	\
  \! -name Makefile.in.in	\
  \! -name ja.po	\
  \! -name ja.gmo	\
  \! -name config.guess	\
  \! -name config.sub	\
  \! -name mkinstalldirs	\
  \! -name intltool-extract.in	\
  \! -name intltool-merge.in	\
  \! -name intltool-update.in	\
  \! -name ltmain.sh	\
  \! -name xmms2_applet.pot	\
  -print |
while read path; do
  file=`echo $path | sed 's,.*/,,'`

  case "$file" in
  *.[ch])
    if ! grep '^[^"]*Copyright (C) 2005-2006 Yuuki Harano[^"]*$' $path > /dev/null; then
      echo $path: No copyright. >&2
      echo false
    fi
    ;;
  esac

  case "$path" in
  */common.h)
    if grep '#include <common.h>' $path > /dev/null; then
      echo $path: common.h included. >&2
      echo false
    fi
    ;;
  *.[ch])
    if ! grep '#include <common.h>' $path > /dev/null; then
      echo $path: common.h not included. >&2
      echo false
    fi
    ;;
  esac

  case "$path" in
  */common.h)
    if ! grep '#include ["<]config\.h[>"]' $path > /dev/null; then
      echo $path: config.h not included. >&2
      echo false
    fi
    ;;
  *.[ch])
    if grep '#include ["<]config\.h[>"]' $path > /dev/null; then
      echo $path: config.h included. >&2
      echo false
    fi
    ;;
  esac

#  case "$path" in
#  *.[ch])
#    if ! grep '#include ["<]config\.h[>"]' $path > /dev/null; then
#      echo $path: config.h not included. >&2
#      echo false
#    fi
#    ;;
#  esac
done | grep false > /dev/null

if [ $? -eq 0 ]; then
  exit 1
else
  exit 0
fi
