SUBDIRS = src

EXTRA_DIST    = XMMS2Applet.server.in.in XMMS2Applet.xml XMMS2Applet.schemas.in
CLEANFILES    = $(server_DATA) $(server_in_files) $(schema_DATA)

all-local: $(server_DATA)

serverdir       = $(libdir)/bonobo/servers
server_in_files = XMMS2Applet.server.in
server_DATA     = $(server_in_files:.server.in=.server)

schemadir 	= $(GCONF_SCHEMA_FILE_DIR)
schema_DATA	= XMMS2Applet.schemas

uidir           = $(datadir)/gnome-2.0/ui
ui_DATA         = XMMS2Applet.xml

$(server_in_files): $(server_in_files:.server.in=.server.in.in) Makefile
	-rm -f $@
	sed -e "s|\@LIBEXECDIR\@|$(libexecdir)|" $< > $@.new
	mv $@.new $@

install-data-local: $(schema_DATA)
if GCONF_SCHEMAS_INSTALL
	for p in $(schema_DATA) ; do \
		GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $$p; \
	done
endif

$(server_DATA): $(server_in_files)
	cp $(server_in_files) $@.new
	mv -f $@.new $@

$(schema_DATA): XMMS2Applet.schemas.in
	cp $< $@.new
	mv -f $@.new $@

distcheck-hook:
	$(srcdir)/check.sh $(distdir) $(top_srcdir)

DISTCHECK_CONFIGURE_FLAGS = --disable-schemas-install

#
