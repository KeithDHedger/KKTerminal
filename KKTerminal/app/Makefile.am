
include ../flagsandlibs
include ../sources

kkterminal_SOURCES = $(COMMONSRC)

kkterminal_CFLAGS = -O3 -export-dynamic
kkterminal_CXXFLAGS = -O3 -export-dynamic

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

scriptfilesdir = $(pkgdatadir)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/README

bin_PROGRAMS = kkterminal

man1_MANS = ../resources/man/kkterminal.1

install-exec-hook:
	cp ../resources/pixmaps/KKTerminal.desktop /usr/share/applications/KKTerminal.desktop
	echo Icon=${prefix}/share/KKTerminal/pixmaps/KKTerminal.png >> /usr/share/applications/KKTerminal.desktop
	echo Exec=${prefix}/bin/kkterminal >> /usr/share/applications/KKTerminal.desktop
#	update-menus

uninstall-hook:
	rm -f /usr/share/applications/KKTerminal.desktop
#	update-menus
	rm -fr $(pkgdatadir)

