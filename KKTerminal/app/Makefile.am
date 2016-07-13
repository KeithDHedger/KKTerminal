
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

kkterminal_SOURCES = $(COMMONSRC)

kkterminal_CXXFLAGS = -O2 -g

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

scriptfilesdir = $(pkgdatadir)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/README

bin_PROGRAMS = kkterminal

man1_MANS = ../resources/man/kkterminal.1

#install-exec-hook:
#	cp ../resources/pixmaps/KKTerminal.desktop /usr/share/applications/KKTerminal.desktop
#	echo Icon=${prefix}/share/KKTerminal/pixmaps/KKTerminal.png >> /usr/share/applications/KKTerminal.desktop
#	echo Exec=${prefix}/bin/kkterminal >> /usr/share/applications/KKTerminal.desktop
#
#uninstall-hook:
#	rm -f /usr/share/applications/KKTerminal.desktop
#	rm -fr $(pkgdatadir)

