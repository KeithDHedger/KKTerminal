/******************************************************
*
*     ©keithhedger Thu 28 Jul 10:44:38 BST 2016
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#include <gtk/gtk.h>

#include "config.h"

#ifndef _PREFS_
#define _PREFS_

enum {NEWVBOX=0,NEWHBOX};
enum {SHOWMENUBARCHK=0,SHOWICONSCHK,ALLOWBOLDCHK};
enum {TEXTCOLOURTXT=0,BACKGROUNDCOLOURTXT,BOLDCOLOURTXT,FONTTXT};

extern GtkWidget	*prefsCheck[];
extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

GtkWidget *createNewBox(int orient,bool homog,int spacing);
void doPrefs(GtkWidget* widget,gpointer data);

#endif

