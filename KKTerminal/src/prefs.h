/******************************************************
*
*     ©keithhedger Tue 24 Oct 12:47:40 BST 2017
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

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

