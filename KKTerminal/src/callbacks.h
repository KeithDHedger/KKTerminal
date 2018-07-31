/*
 *
 * ©K. D. Hedger. Fri 15 Jul 12:19:20 BST 2016 keithdhedger@gmail.com

 * This file (callbacks.h) is part of KKTerminal.

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
#include <vte/vte.h>

#include "config.h"

#ifndef _CALLBACKS_
#define _CALLBACKS_

void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data);
void exitShell(VteTerminal *vteterminal,gpointer pageptr);
void copyFromTerm(GtkWidget* widget,pageStruct *page);
void pasteToTerm(GtkWidget* widget,pageStruct *page);
void selectAllInTerm(GtkWidget* widget,pageStruct *page);
gboolean on_key_press(GtkWidget *terminal,GdkEventKey *event);
gboolean doButton(GtkWidget *widget, GdkEventButton *event,pageStruct* page);
void doAbout(GtkWidget* widget,gpointer data);
void doHelp(GtkWidget* widget,gpointer data);
void newPage(GtkWidget *widget,gpointer data);
void toggleMenuBar(GtkWidget *widget,gpointer data);
gboolean keyShortCut(GtkWidget *window,GdkEventKey *event,gpointer data);
void prevTab(GtkWidget* widget,gpointer data);
void nextTab(GtkWidget* widget,gpointer data);
void setPrefs(GtkWidget* widget,gpointer data);
void pasteToTermWithQuotes(GtkWidget* widget,pageStruct *page);

#endif
