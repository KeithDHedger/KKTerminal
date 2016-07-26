/*
 *
 * ©K. D. Hedger. Fri 15 Jul 12:19:12 BST 2016 kdhedger68713@gmail.com

 * This file (callbacks.cpp) is part of KKTerminal.

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
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>

#include "globals.h"
#include "gui.h"
#include "config.h"

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

void exitShell(VteTerminal *vteterminal,gpointer pageptr)
{
	int			pagenum=-1;
	GtkWidget	*vbox;
	pageStruct	*page=(pageStruct*)pageptr;

	if(pageptr==NULL)
		{
			pagenum=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook);
			vbox=gtk_notebook_get_nth_page((GtkNotebook*)mainNotebook,pagenum);
			page=(pageStruct*)g_object_get_data((GObject*)vbox,"pageid");
			gtk_widget_destroy(page->menu);
			gtk_widget_destroy(page->tabVbox);
			gtk_notebook_remove_page((GtkNotebook*)mainNotebook,pagenum);
			g_free(page);
		}
	else
		{
			pagenum=gtk_notebook_page_num((GtkNotebook*)mainNotebook,page->swindow);
			gtk_notebook_remove_page((GtkNotebook*)mainNotebook,pagenum);
			gtk_widget_destroy(page->menu);
			gtk_widget_destroy(page->tabVbox);
			g_free(page);
		}
	if(gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)==0)
		doShutdown(NULL,NULL);
}

void copyFromTerm(GtkWidget* widget,pageStruct *page)
{
	vte_terminal_copy_clipboard((VteTerminal*)page->terminal);
}

void pasteToTerm(GtkWidget* widget,pageStruct *page)
{
	vte_terminal_paste_clipboard((VteTerminal*)page->terminal);
}

void selectAllInTerm(GtkWidget* widget,pageStruct *page)
{
	vte_terminal_select_all((VteTerminal*)page->terminal);
}

gboolean on_key_press(GtkWidget *terminal,GdkEventKey *event)
{
	if (event->state == (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
		{
			switch (event->keyval)
				{
				case GDK_C:
					vte_terminal_copy_clipboard(VTE_TERMINAL(terminal));
					return true;
				case GDK_V:
					vte_terminal_paste_clipboard(VTE_TERMINAL(terminal));
					return true;
				}
		}
	return false;
}

gboolean doButton(GtkWidget *widget, GdkEventButton *event,pageStruct* page)
{
	int			button,event_time;

	gtk_widget_set_can_focus(page->terminal,true);
	gtk_widget_grab_focus(page->terminal);

///* Ignore double-clicks and triple-clicks */
	if (event->button == 3 && event->type == GDK_BUTTON_PRESS)
		{
			gtk_widget_show_all(page->menu);
			if (event)
				{
					button=event->button;
					event_time=event->time;
				}
			else
				{
					button=0;
					event_time=gtk_get_current_event_time();
				}

			gtk_menu_popup(GTK_MENU(page->menu),NULL,NULL,NULL,NULL,button,event_time);
		}
	return(false);
}

void doAbout(GtkWidget* widget,gpointer data)
{
	const char	copyright[]=COPYRITE " \n" MYEMAIL;
	const char*	aboutboxstring="KKTerminal - Simple small terminal for gtk2/3";
	char*		licence;

	g_file_get_contents(DATADIR"/docs/gpl-3.0.txt",&licence,NULL,NULL);

	gtk_show_about_dialog(NULL,"authors",authors,"comments",aboutboxstring,"copyright",copyright,"version",VERSION,"website",KKTERMINALPAGE,"program-name","KKTerminal","website-label","KKTerminal Page","logo-icon-name","KKTerminal","license",licence,NULL);

	g_free(licence);
}

void doHelp(GtkWidget* widget,gpointer data)
{
	sinkReturn=system("xdg-open http://khapplications.darktech.org/pages/kkterminal/kkterminal.html");
}

void newPage(GtkWidget *widget,gpointer data)
{
	pageStruct	*page=NULL;
	int			pagenum=0;
	GtkWidget	*vbox;
	char		*wd=NULL;
	
	pagenum=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook);
	if(pagenum>-1)
		{
			vbox=gtk_notebook_get_nth_page((GtkNotebook*)mainNotebook,pagenum);
			page=(pageStruct*)g_object_get_data((GObject*)vbox,"pageid");
		}
	wd=getPwd(page);
	addPage(wd);
	g_free(wd);
}

void toggleMenuBar(GtkWidget *widget,gpointer data)
{
	showMenuBar=!showMenuBar;
	if(showMenuBar==true)
		gtk_widget_show_all(menuBar);
	else
		gtk_widget_hide(menuBar);
}

gboolean keyShortCut(GtkWidget *window,GdkEventKey *event,gpointer data)
{
	int				loop=0;
	bool			gotKey=false;
	activateCBPtr	acb;

	if(event->type==GDK_KEY_PRESS)
		{
			while(shortCuts[loop][0]!=-1)
				{
					if(event->state==shortCuts[loop][1])
						{
							if(event->keyval==shortCuts[loop][0])
								{
									acb=(activateCBPtr)shortCuts[loop][2];
									acb(NULL,NULL);
								return(true);
							}
						}		
					loop++;
				}
		}
	return(false);
}

void prevTab(GtkWidget* widget,gpointer data)
{
	gtk_notebook_prev_page((GtkNotebook*)mainNotebook);
}

void nextTab(GtkWidget* widget,gpointer data)
{
	gtk_notebook_next_page((GtkNotebook*)mainNotebook);
}



void setPrefs(GtkWidget* widget,gpointer data)
{
}







