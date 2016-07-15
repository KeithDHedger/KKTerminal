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
#include <gdk/gdkkeysyms.h>

#include "config.h"
#include "globals.h"

enum {NEWVBOX=0,NEWHBOX};
unsigned 	labelNum=1;
GtkWidget	*contextMenu;

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
			g_free(page);
			gtk_notebook_remove_page((GtkNotebook*)mainNotebook,pagenum);
		}
	else
		{
			pagenum=gtk_notebook_page_num((GtkNotebook*)mainNotebook,page->swindow);
			gtk_notebook_remove_page((GtkNotebook*)mainNotebook,pagenum);
			g_free(page);
		}
	if(gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)==0)
		doShutdown(NULL,NULL);
}

#ifdef _USEGTK3_
void applyCSS (GtkWidget *widget, GtkStyleProvider *widgprovider)
{
	gtk_style_context_add_provider(gtk_widget_get_style_context (widget),widgprovider,GTK_STYLE_PROVIDER_PRIORITY_USER);
	if (GTK_IS_CONTAINER (widget))
		gtk_container_forall (GTK_CONTAINER(widget),(GtkCallback)applyCSS,widgprovider);
}
#endif

GtkWidget *makeNewTab(char *name,pageStruct *page)
{
	char		*labeltext;
	GtkWidget	*hbox;
	GtkWidget	*pad;
	GtkWidget	*label;
	GtkWidget	*evbox=NULL;
	
	evbox=gtk_event_box_new();
	GtkWidget	*close=gtk_image_new_from_icon_name(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
	GtkWidget	*button=gtk_button_new();

	hbox=createNewBox(NEWHBOX,false,0);
	pad=createNewBox(NEWHBOX,false,0);
	asprintf(&labeltext,"Shell %i",labelNum++);
	label=gtk_label_new(labeltext);
	g_free(labeltext);

	gtk_button_set_relief((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

	gtk_box_pack_start(GTK_BOX(hbox),pad,true,true,0);


	gtk_button_set_focus_on_click(GTK_BUTTON(button),FALSE);
	gtk_container_add(GTK_CONTAINER(button),close);

	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	gtk_container_add(GTK_CONTAINER(evbox),hbox);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(exitShell),(void*)page);
	//g_signal_connect(G_OBJECT(evbox),"button-press-event",G_CALLBACK(tabPopUp),(void*)page);

//TODO//
#ifdef _USEGTK3_
#ifdef _USEGTK3_
	char	*tabcss=NULL;
	GtkStyleProvider	*provider;
	GtkStyleProvider	*tabBoxProvider;

	provider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	asprintf(&tabcss,"* {\n \
  padding: %ipx; \n \
}\n",0);

	tabBoxProvider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	gtk_css_provider_load_from_data((GtkCssProvider*)tabBoxProvider,tabcss,-1,NULL);
	g_free(tabcss);
#endif
	applyCSS(evbox,tabBoxProvider);
	gtk_style_context_reset_widgets(gdk_screen_get_default());
#else
	GtkRcStyle	*style=gtk_rc_style_new();
	style->xthickness=style->ythickness=0;
	gtk_widget_modify_style(button,style);
	g_object_unref(G_OBJECT(style));
#endif
	gtk_widget_show_all(evbox);
	return(evbox);
}

gboolean doButton(GtkWidget *widget, GdkEventButton *event,pageStruct* page)
{
	int button, event_time;

	gtk_widget_set_can_focus(page->terminal,true);
	gtk_widget_grab_focus(page->terminal);

///* Ignore double-clicks and triple-clicks */
  if (event->button == 3 && event->type == GDK_BUTTON_PRESS)
    {
		gtk_widget_show_all(contextMenu);
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

		gtk_menu_popup(GTK_MENU(contextMenu),NULL,NULL,NULL,NULL,button,event_time);
	}
	return(false);
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


void makeMenu(pageStruct *page)
{
	GtkWidget *popmenuitem;

	contextMenu=gtk_menu_new ();

	popmenuitem=gtk_menu_item_new_with_label("Copy");
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(copyFromTerm),page);
	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);

	popmenuitem=gtk_menu_item_new_with_label("Paste");
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(pasteToTerm),page);
	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);

	popmenuitem=gtk_menu_item_new_with_label("Select All");
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(selectAllInTerm),page);
	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);
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

void addPage(void)
{
	GtkWidget	*label;
#ifdef _USEGTK3_
	GdkRGBA		colour;
#else
	GdkColor	colour;
#endif

	char		*startterm[2]={0,0};

	loadVarsFromFile(prefsFile,mydata);

	pageStruct	*page=(pageStruct*)malloc(sizeof(pageStruct));
	page->terminal=vte_terminal_new();
	vte_terminal_set_default_colors((VteTerminal*)page->terminal);
	vte_terminal_set_scrollback_lines((VteTerminal*)page->terminal,-1);

	page->swindow=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)page->swindow,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(page->swindow),page->terminal);

	page->tabVbox=createNewBox(NEWVBOX,true,4);
	label=makeNewTab(NULL,page);

//	g_object_set_data(G_OBJECT(page->swindow),"tab-expand",(gpointer)0);
//	g_object_set_data(G_OBJECT(page->tabVbox),"tab-expand",(gpointer)0);
//	g_object_set_data(G_OBJECT(page->swindow),"tab-fill",(gpointer)0);
//	g_object_set_data(G_OBJECT(page->tabVbox),"tab-fill",(gpointer)0);
//	g_object_set_data(G_OBJECT(mainNotebook),"homogeneous",(gpointer)0);


	gtk_notebook_append_page((GtkNotebook*)mainNotebook,page->swindow,label);
	gtk_notebook_set_tab_reorderable((GtkNotebook*)mainNotebook,page->swindow,true);

	startterm[0]=vte_get_user_shell();

//gdk_rgba_parse
#ifdef _USEGTK3_
	gdk_rgba_parse(&colour,(const gchar*)foreColour);
#ifdef _VTEVERS290_
	vte_terminal_set_color_foreground_rgba((VteTerminal*)page->terminal,(const GdkRGBA*)&colour);
#else
	vte_terminal_set_color_foreground((VteTerminal*)page->terminal,(const GdkRGBA*)&colour);
#endif
	gdk_rgba_parse(&colour,(const gchar*)backColour);
#ifdef _VTEVERS290_
	vte_terminal_set_color_background_rgba((VteTerminal*)page->terminal,(const GdkRGBA*)&colour);
#else
	vte_terminal_set_color_background((VteTerminal*)page->terminal,(const GdkRGBA*)&colour);
#endif
#else
	gdk_color_parse((const gchar*)foreColour,&colour);
	vte_terminal_set_color_foreground((VteTerminal*)page->terminal,(const GdkColor*)&colour);
	gdk_color_parse((const gchar*)backColour,&colour);
	vte_terminal_set_color_background((VteTerminal*)page->terminal,(const GdkColor*)&colour);
#endif

#ifdef _USEGTK3_
#ifdef _VTEVERS290_
	vte_terminal_fork_command_full((VteTerminal *)page->terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&page->pid,NULL);
#else
	vte_terminal_spawn_sync((VteTerminal *)page->terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&page->pid,NULL,NULL);
#endif
#else
	vte_terminal_fork_command_full((VteTerminal *)page->terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&page->pid,NULL);
#endif

//dnd
	gtk_drag_dest_set(page->terminal,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets(page->terminal);
	gtk_drag_dest_add_text_targets(page->terminal);
	g_signal_connect(G_OBJECT(page->terminal),"drag_data_received",G_CALLBACK(dropUri),NULL);
	g_signal_connect(G_OBJECT(page->terminal),"child-exited",G_CALLBACK(exitShell),(void*)page);

//context
	g_signal_connect(page->terminal,"button-press-event",G_CALLBACK(doButton),page);
	g_signal_connect(page->terminal,"key-press-event",G_CALLBACK(on_key_press),NULL);
	makeMenu(page);
	gtk_widget_show_all(mainWindow);
	g_object_set_data(G_OBJECT(page->tabVbox),"pageid",(gpointer)page);

//gtk_notebook_set_tab_label_packing ((GtkNotebook*)mainNotebook,page->swindow, TRUE, TRUE, GTK_PACK_START);
//
//
//	gtk_widget_show_all(mainWindow);
//
//GtkWidget *child=gtk_notebook_get_nth_page ((GtkNotebook*)mainNotebook,0);
////printf(">>>>%i<<<<\n",g_object_get_data(G_OBJECT(page->swindow),"tab-expand"));
////printf(">>>>%i<<<<\n",g_object_get_data(G_OBJECT(page->swindow),"tab-fill"));
////printf(">>>>%i<<<<\n",g_object_get_data(G_OBJECT(page->tabVbox),"tab-expand"));
////printf(">>>>%i<<<<\n",g_object_get_data(G_OBJECT(page->tabVbox),"tab-fill"));
////printf(">>>>%i<<<<\n",g_object_get_data(G_OBJECT(label),"tab-expand"));
////printf(">>>>%i<<<<\n",g_object_get_data(G_OBJECT(label),"tab-fill"));
//printf(">>>>%i<<<<\n",g_object_get_data(G_OBJECT(child),"tab-expand"));
//printf(">>>>%i<<<<\n",g_object_get_data(G_OBJECT(child),"tab-fill"));
//
//gboolean ex;
//gboolean fil;
//GtkPackType pack;
//gboolean *ptr;
//
//gpointer p1=NULL,p2=NULL,p3=NULL;
//
//gtk_notebook_query_tab_label_packing ((GtkNotebook*)mainNotebook,child,&ex,&fil,&pack);
//printf(">>%i %i\n",ex,fil);
//
//GtkWidget *cc=NULL;
//cc=gtk_notebook_get_tab_label ((GtkNotebook*)mainNotebook,page->swindow);
//
//p1=g_object_get_data(G_OBJECT(child),"tab-expand");
//p2=g_object_get_data(G_OBJECT(child),"tab-fill");
//printf("--%p %p\n",p1,p2);
//
//p1=g_object_get_data(G_OBJECT(page->swindow),"tab-expand");
//p2=g_object_get_data(G_OBJECT(page->swindow),"tab-fill");
//p3=g_object_get_data(G_OBJECT(page->swindow),"reorderable");
//printf("--%p %p %p %p\n",p1,p2,p3,cc);
//
//p1=g_object_get_data((GObject*)evbox,"tab-expand");
//p2=g_object_get_data((GObject*)evbox,"tab-fill");
//p3=g_object_get_data((GObject*)cc,"reorderable");
//printf("--%p %p %p %p\n",p1,p2,p3,cc);

	gtk_container_child_set((GtkContainer*)mainNotebook,page->swindow,"tab-expand",true,NULL);
//gtk_container_child_set((GtkContainer*)mainNotebook,page->swindow,"tab-fill",true,NULL);
gtk_widget_show_all(mainWindow);
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
	GtkWidget	*menu;
	GtkWidget	*menuitem;

	char		*startterm[2]={0,0};
	GtkWidget	*vbox=createNewBox(NEWVBOX,false,0);

	asprintf(&prefsFile,"mkdir -p %s/.KKEdit%s/plugins-gtk",getenv("HOME"),_EXECSUFFIX_);
	system(prefsFile);
	freeAndNull(&prefsFile);

	asprintf(&prefsFile,"%s/.KKEdit%s/plugins-gtk/terminalpane.rc",getenv("HOME"),_EXECSUFFIX_);	

	mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)mainWindow,800,320);
	mainNotebook=gtk_notebook_new();
	gtk_notebook_set_scrollable((GtkNotebook*)mainNotebook,true);

	gtk_notebook_set_show_tabs((GtkNotebook*)mainNotebook,true);
	g_object_set_data(G_OBJECT(mainNotebook),"tab-expand",(gpointer)true);
	g_object_set_data(G_OBJECT(mainNotebook),"tab-fill",(gpointer)true);

//menus
	menuBar=gtk_menu_bar_new();
//file menu
	fileMenu=gtk_menu_item_new_with_label("_File");
	gtk_menu_item_set_use_underline((GtkMenuItem*)fileMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenu),menu);
//new
	menuitem=gtk_menu_item_new_with_mnemonic("_New Shell");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(addPage),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//close
	menuitem=gtk_menu_item_new_with_mnemonic("_Close Tab");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(exitShell),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//quit
	menuitem=gtk_menu_item_new_with_mnemonic("_Quit");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doShutdown),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//addmenubar
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),fileMenu);
	gtk_box_pack_start((GtkBox*)vbox,menuBar,false,false,0);
	gtk_box_pack_start((GtkBox*)vbox,mainNotebook,true,true,0);

	gtk_container_add(GTK_CONTAINER(mainWindow),vbox);

	addPage();
	gtk_widget_show_all(mainWindow);
}





