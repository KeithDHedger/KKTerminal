/******************************************************
*
*     ©keithhedger Tue 26 Jul 13:08:22 BST 2016
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
enum {SHOWMENUBARCHK=0,SHOWICONSCHK};
enum {TEXTCOLOURTXT=0,BACKGROUNDCOLOURTXT,FONTTXT};

extern GtkWidget	*prefsCheck[];
extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

GtkWidget *createNewBox(int orient,bool homog,int spacing);
void doPrefs(GtkWidget* widget,gpointer data);

#endif

