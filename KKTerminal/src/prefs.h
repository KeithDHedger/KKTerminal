/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:49:41 BST 2018 keithdhedger@gmail.com

 * This file (prefs.h) is part of KKTerminal.

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

#ifndef _PREFS_
#define _PREFS_

enum {NEWVBOX=0,NEWHBOX};
enum {SHOWMENUBARCHK=0,SHOWICONSCHK,ALLOWBOLDCHK,USESINGLECHK};

enum {SHOWMENUBARCHKID=3000,SHOWICONSCHKID,ALLOWBOLDCHKID,USESINGLECHKID};

enum {TEXTCOLOURTXT=0,BACKGROUNDCOLOURTXT,BOLDCOLOURTXT,FONTTXT,CODESET};

extern bool showMenuBar;
extern bool iconsInMenu;
extern bool allowBold;
extern bool useSingleQuotes;
extern char *foreColour;
extern char *backColour;
extern char *boldColour;
extern char *fontAndSize;
extern char *codeSet;

extern GtkWidget	*prefsCheck[];

extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

void doPrefs(GtkWidget* widget,gpointer data);
GtkWidget *createNewBox(int orient,bool homog,int spacing);

#endif

