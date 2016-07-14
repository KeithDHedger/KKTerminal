/*
 *


 * This file (gui.cpp) is part of KKTerminal.

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
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "globals.h"

enum {NEWVBOX=0,NEWHBOX};

GtkWidget *createNewBox(int orient,bool homog,int spacing)
{
	GtkWidget	*retwidg=NULL;

#ifdef _USEGTK3_
	if(orient==NEWVBOX)
		retwidg=gtk_box_new(GTK_ORIENTATION_VERTICAL,spacing);
	else
		retwidg=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,spacing);
	gtk_box_set_homogeneous((GtkBox*)retwidg,homog);
#else
	if(orient==NEWVBOX)
		retwidg=gtk_vbox_new(homog,spacing);
	else
		retwidg=gtk_hbox_new(homog,spacing);
#endif

	return(retwidg);
}

int	childPid=-999;

void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	GString	*pastestr=g_string_new(NULL);
	gchar	**array=NULL;
	int		cnt=0;
	char	*filename=NULL;
	char	*pastedata=NULL;

	filename=(char*)gtk_selection_data_get_text(selection_data);
	if(filename!=NULL)
		{
			vte_terminal_feed_child((VteTerminal*)widget,filename,strlen(filename));
			g_free(filename);
		}

	array=gtk_selection_data_get_uris(selection_data);
	if(array!=NULL)
		{
			cnt=g_strv_length(array);
			for(int j=0; j<cnt; j++)
				{
					filename=g_filename_from_uri(array[j],NULL,NULL);
					if(filename!=NULL)
						{
							g_string_append_printf(pastestr,"'%s' ",filename);
							g_free(filename);
						}
				}
			vte_terminal_feed_child((VteTerminal*)widget,pastestr->str,pastestr->len);
			g_string_free(pastestr,true);
			g_strfreev(array);
		}

	gtk_drag_finish(context,true,false,time);		
}

void buildMainGui(void)
{
#ifdef _USEGTK3_
	GdkRGBA		colour;
#else
	GdkColor	colour;
#endif

	GtkWidget	*terminal=NULL;
	GtkWidget	*swindow;
	char		*prefsfile=NULL;
	char		*startterm[2]={0,0};
	GtkWidget	*vbox=createNewBox(NEWVBOX,true,true);

	asprintf(&prefsfile,"mkdir -p %s/.KKEdit%s/plugins-gtk",getenv("HOME"),_EXECSUFFIX_);
	system(prefsfile);
	freeAndNull(&prefsfile);

	asprintf(&prefsfile,"%s/.KKEdit%s/plugins-gtk/terminalpane.rc",getenv("HOME"),_EXECSUFFIX_);	

	loadVarsFromFile(prefsfile,mydata);
	freeAndNull(&prefsfile);

	mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)mainWindow,800,320);
	mainNotebook=gtk_notebook_new();
	gtk_notebook_set_scrollable((GtkNotebook*)mainNotebook,true);
	gtk_notebook_set_show_tabs((GtkNotebook*)mainNotebook,true);
	gtk_container_add((GtkContainer*)vbox,mainNotebook);

	terminal=vte_terminal_new();
	vte_terminal_set_default_colors((VteTerminal*)terminal);
	vte_terminal_set_scrollback_lines((VteTerminal*)terminal,-1);

	swindow=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)swindow,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(swindow),terminal);

	gtk_notebook_append_page((GtkNotebook*)mainNotebook,swindow,NULL);
	gtk_container_add(GTK_CONTAINER(mainWindow),vbox);

	startterm[0]=vte_get_user_shell();

//gdk_rgba_parse
#ifdef _USEGTK3_
	gdk_rgba_parse(&colour,(const gchar*)foreColour);

#ifdef _VTEVERS290_
	vte_terminal_set_color_foreground_rgba((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#else
	vte_terminal_set_color_foreground((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#endif

	gdk_rgba_parse(&colour,(const gchar*)backColour);
#ifdef _VTEVERS290_
	vte_terminal_set_color_background_rgba((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#else
	vte_terminal_set_color_background((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#endif

#else
	gdk_color_parse((const gchar*)foreColour,&colour);
	vte_terminal_set_color_foreground((VteTerminal*)terminal,(const GdkColor*)&colour);
	gdk_color_parse((const gchar*)backColour,&colour);
	vte_terminal_set_color_background((VteTerminal*)terminal,(const GdkColor*)&colour);
#endif


#ifdef _USEGTK3_

#ifdef _VTEVERS290_
	vte_terminal_fork_command_full((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL);
#else
	vte_terminal_spawn_sync((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL,NULL);
#endif

#else
	vte_terminal_fork_command_full((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL);
#endif

//dnd
	gtk_drag_dest_set(terminal,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets(terminal);
	gtk_drag_dest_add_text_targets(terminal);
	g_signal_connect(G_OBJECT(terminal),"drag_data_received",G_CALLBACK(dropUri),NULL);

	gtk_widget_show_all(mainWindow);
}





