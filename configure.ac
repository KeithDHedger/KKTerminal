#
#
AC_INIT([KKTerminal],[0.1.1],[PROJ],[KKTerminal])

AC_CONFIG_HEADER([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC

AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(CXXFLAGS)

X11_MODULES="x11 >= 1.0.0"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_LIBS)
AC_SUBST(X11_CFLAGS)

AC_MSG_CHECKING(whether to use gtk3)
AC_ARG_ENABLE(gtk3,
    [  --enable-gtk3           Use GTK3.],
    [enable_gtk3=$enableval],
    [enable_gtk3="no"])
AC_MSG_RESULT($enable_gtk3)
if test "$enable_gtk3" = "yes"; then
	AC_DEFINE([_USEGTK3_], [], [Use GTK3])
	GTK_MODULES="gtk+-3.0 >= 3.10.0"
else
	GTK_MODULES="gtk+-2.0 >= 2.24.0"
	AC_DEFINE([_EXECSUFFIX_], [""], [Use GTK3])
fi

PKG_CHECK_MODULES(GTK,$GTK_MODULES)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

if test "$enable_gtk3" = "yes"; then
	AC_MSG_CHECKING(Finding vte version)
	PKG_CHECK_EXISTS(vte-2.91,[usevte=91],[usevte=90])
	if test $usevte = 91; then
		VTE_MODULES="vte-2.91 >= 0.40.0"
		VTEVERS=2.91
		AC_MSG_RESULT(using $VTEVERS)
	else
		VTE_MODULES="vte-2.90 >= 0.29.1"
		VTEVERS=2.90
		AC_DEFINE([_VTEVERS290_], [""], [Use vte-2.90])
		AC_MSG_RESULT(using $VTEVERS)
	fi
else
	VTE_MODULES="vte >= 0.28.2"
fi

PKG_CHECK_MODULES(VTE,$VTE_MODULES,[],[AC_MSG_ERROR([No VTE dev files found ... For debian install either libvte-dev for gtk2 build or for gtk3 build libvte-2.91-dev or libvte-2.90-dev, see INSTALL file for details.])])
AC_SUBST(VTE_CFLAGS)
AC_SUBST(VTE_LIBS)

if test "$DEVMODE" = "yes"; then
	AC_DEFINE([_DEVMODE_], [""], [Use dev mode])
fi

AC_OUTPUT(Makefile KKTerminal/app/Makefile)

