/*
 *
 * ©K. D. Hedger. Wed 13 Jul 20:10:11 BST 2016 kdhedger68713@gmail.com

 * This file (globals.h) is part of KKTerminal.

 * KKTerminal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKTerminal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKTerminal.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>

#include "config.h"
#include "internet.h"

#ifndef _GLOBALS_
#define _GLOBALS_

#ifdef _USEGTK3_
#define GTK_STOCK_CLOSE "window-close"
#define GTK_STOCK_NEW "window-new"
#define GTK_STOCK_OPEN "window-open"
#define GTK_STOCK_QUIT "gtk-quit"
#define GTK_STOCK_ABOUT "gtk-about"

#define GDK_C GDK_KEY_C
#define GDK_V GDK_KEY_V
#endif

#define	APPEXECNAME "kkterminal"

struct	args
{
	const char			*name;
	int					type;
	void				*data;
};

struct	pageStruct
{
	GtkWidget			*terminal;
	GtkWidget			*swindow;
	int					pid;
	GtkWidget			*tabVbox;
	GtkWidget			*menu;
};

enum {TYPEINT=1,TYPESTRING,TYPEBOOL,TYPELIST};
enum {NOERR=0,NOOPENFILE,NOSAVEFILE};

//aplication
extern char				*foreColour;
extern char				*backColour;
extern args				mydata[]; 
extern char				*prefsFile;
extern int				sinkReturn;
extern char				*sinkReturnStr;
extern bool				singleUse;
extern GApplication		*mainApp;
extern const char		*termCommand;
extern char				*windowAllocData;

//main mainWindow
extern int				windowWidth;
extern int				windowHeight;
extern int				windowX;
extern int				windowY;

extern GtkWidget		*mainWindow;
extern GtkWidget		*mainNotebook;
extern GtkWidget		*menuBar;

#ifdef _USEGTK3_
extern GtkStyleProvider	*tabBoxProvider;
#endif

//file menu
extern GtkWidget		*fileMenu;
//help
extern GtkWidget		*helpMenu;

//shells
extern char				*fontAndSize;

void doShutdown(GtkWidget* widget,gpointer data);
void freeAndNull(char **ptr);
int loadVarsFromFile(char *filepath,args *dataptr);

#endif
