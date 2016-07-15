
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

kkterminal_SOURCES = $(COMMONSRC)

kkterminal_CXXFLAGS = -O2 -g

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

docfilesdir = $(pkgdatadir)/docs
docfiles_DATA = ../resources/docs/*

bin_PROGRAMS = kkterminal

man1_MANS = ../resources/man/kkterminal.1

install-data-hook:
	mkdir -p $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps $(DESTDIR)${prefix}/share/applications $(DESTDIR)${prefix}/share/pixmaps||true
	ln -sf $(pkgdatadir)/pixmaps/KKTerminal.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKTerminal.png
	ln -sf $(pkgdatadir)/pixmaps/KKTerminal.png $(DESTDIR)${prefix}/share/pixmaps/KKTerminal.png
	cp ../resources/pixmaps/KKTerminal.desktop $(DESTDIR)${prefix}/share/applications/KKTerminal.desktop

uninstall-hook:
	rm $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKTerminal.png||true
	rm -f $(DESTDIR)${prefix}/share/applications/KKTerminal.desktop
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKTerminal.png
	rm -fr $(DESTDIR)$(pkgdatadir)

	