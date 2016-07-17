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
#include "callbacks.h"

enum {NEWVBOX=0,NEWHBOX};
unsigned 	labelNum=1;

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
	gtk_container_set_border_width (GTK_CONTAINER (hbox),0);

	pad=createNewBox(NEWHBOX,false,0);
	gtk_container_set_border_width (GTK_CONTAINER (pad),0);
	asprintf(&labeltext,"Shell %i",labelNum++);
	label=gtk_label_new(labeltext);
	g_free(labeltext);

	gtk_button_set_relief((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

	gtk_box_pack_start(GTK_BOX(hbox),pad,true,true,0);

	gtk_button_set_focus_on_click(GTK_BUTTON(button),FALSE);
	gtk_container_add(GTK_CONTAINER(button),close);
	gtk_container_set_border_width (GTK_CONTAINER (button),0);

	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	gtk_container_add(GTK_CONTAINER(evbox),hbox);
	gtk_container_set_border_width (GTK_CONTAINER (hbox),0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(exitShell),(void*)page);
	//g_signal_connect(G_OBJECT(evbox),"button-press-event",G_CALLBACK(tabPopUp),(void*)page);

#ifdef _USEGTK3_
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

GtkWidget *makeMenu(pageStruct *page)
{
	GtkWidget	*popmenuitem;
	GtkWidget	*retwidg=NULL;

	retwidg=gtk_menu_new ();

	popmenuitem=gtk_menu_item_new_with_label("Copy");
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(copyFromTerm),page);
	gtk_menu_shell_append(GTK_MENU_SHELL(retwidg),popmenuitem);

	popmenuitem=gtk_menu_item_new_with_label("Paste");
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(pasteToTerm),page);
	gtk_menu_shell_append(GTK_MENU_SHELL(retwidg),popmenuitem);

	popmenuitem=gtk_menu_item_new_with_label("Select All");
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(selectAllInTerm),page);
	gtk_menu_shell_append(GTK_MENU_SHELL(retwidg),popmenuitem);

	return(retwidg);
}

void addPage(const char *dir)
{
	GtkWidget	*label;
	int			newpagenum;
#ifdef _USEGTK3_
	GdkRGBA		colour;
#else
	GdkColor	colour;
#endif

	char		*startterm[2]={0,0};

//	loadVarsFromFile(prefsFile,mydata);

	pageStruct	*page=(pageStruct*)malloc(sizeof(pageStruct));
	page->terminal=vte_terminal_new();
	vte_terminal_set_default_colors((VteTerminal*)page->terminal);
	vte_terminal_set_scrollback_lines((VteTerminal*)page->terminal,-1);

	page->swindow=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)page->swindow,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(page->swindow),page->terminal);

	page->tabVbox=createNewBox(NEWVBOX,true,4);
	label=makeNewTab(NULL,page);

	newpagenum=gtk_notebook_append_page((GtkNotebook*)mainNotebook,page->swindow,label);
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
	vte_terminal_fork_command_full((VteTerminal *)page->terminal,VTE_PTY_DEFAULT,dir,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&page->pid,NULL);
#else
	vte_terminal_spawn_sync((VteTerminal *)page->terminal,VTE_PTY_DEFAULT,dir,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&page->pid,NULL,NULL);
#endif
#else
	vte_terminal_fork_command_full((VteTerminal *)page->terminal,VTE_PTY_DEFAULT,dir,startterm,NULL,(GSpawnFlags)(G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&page->pid,NULL);
#endif

#ifdef _USEGTK3_
	PangoFontDescription *pf;
	pf=pango_font_description_from_string(fontAndSize);
	vte_terminal_set_font ((VteTerminal *)page->terminal,pf);
#else
	vte_terminal_set_font_from_string ((VteTerminal *)page->terminal,fontAndSize);
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
	gtk_widget_show_all(mainWindow);
	g_object_set_data(G_OBJECT(page->swindow),"pageid",(gpointer)page);

	gtk_container_child_set((GtkContainer*)mainNotebook,page->swindow,"tab-expand",true,NULL);
	gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,newpagenum);
	gtk_widget_show_all(mainWindow);

	page->menu=makeMenu(page);
	if(termCommand!=NULL)
		{
			vte_terminal_feed_child((VteTerminal*)page->terminal,termCommand,-1);
			vte_terminal_feed_child((VteTerminal*)page->terminal,"\n",-1);
		}
}

//Thanks to xfce4-terminal for bits of this code
//http://archive.xfce.org/src/apps/xfce4-terminal/0.6
char *getPwd(pageStruct *page)
{
	char	buffer[4097];
	char	*file;
	char	*cwd;
	int		length;
	char	*retval=NULL;

	if(page!=NULL)
		{
//make sure that we use linprocfs on all systems
#if defined(__FreeBSD__)
			file=g_strdup_printf("/compat/linux/proc/%d/cwd",page->pid);
#elif defined(__NetBSD__) || defined(__OpenBSD__)
			file=g_strdup_printf("/emul/linux/proc/%d/cwd",page->pid);
#else
			file=g_strdup_printf ("/proc/%d/cwd",page->pid);
#endif
			length=readlink(file,buffer,sizeof(buffer));
			if(length>0 && *buffer == '/')
				{
					buffer[length]='\0';
					retval=g_strdup(buffer);
				}
			else if(length==0)
				{
					cwd=g_get_current_dir();
					if(G_LIKELY(cwd != NULL))
						{
							if (chdir(file)==0)
								retval=g_get_current_dir();
							g_free(cwd);
						}
				}
			g_free (file);
		}
	if(retval==NULL)
		retval=g_strdup(getenv("HOME"));
	return(retval);
}

void newPage(GtkWidget *widget,gpointer data)
{
	pageStruct	*page=NULL;
	int			pagenum=0;
	GtkWidget	*vbox;
	char		*wd=NULL;
	
	pagenum=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook);
	vbox=gtk_notebook_get_nth_page((GtkNotebook*)mainNotebook,pagenum);
	page=(pageStruct*)g_object_get_data((GObject*)vbox,"pageid");
	wd=getPwd(page);
	addPage(wd);
	g_free(wd);
}

GtkWidget* newMenuItem(const char* menuname,const char* stockid)
{
	GtkWidget	*menu;
#ifdef _USEGTK3_
	menu=gtk_menu_item_new_with_mnemonic(menuname);
#else
	menu=gtk_image_menu_item_new_from_stock(stockid,NULL);
#endif

	return(menu);
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

	mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_default_size((GtkWindow*)mainWindow,windowWidth,windowHeight);
	if(windowX!=-1 && windowY!=-1)
		gtk_window_move((GtkWindow *)mainWindow,windowX,windowY);

	gtk_window_set_default_icon_name(PACKAGE);
	gtk_window_set_icon_name((GtkWindow*)mainWindow,PACKAGE);

	mainNotebook=gtk_notebook_new();
	gtk_notebook_set_scrollable((GtkNotebook*)mainNotebook,true);

	gtk_notebook_set_show_tabs((GtkNotebook*)mainNotebook,true);
#ifndef _USEGTK3_
	g_object_set((GObject*)mainNotebook,"tab-vborder",0,NULL);
#endif

//menus
	menuBar=gtk_menu_bar_new();
//file menu
	fileMenu=gtk_menu_item_new_with_label("_File");
	gtk_menu_item_set_use_underline((GtkMenuItem*)fileMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenu),menu);
//new
	//menuitem=gtk_menu_item_new_with_mnemonic("_New Shell");
	menuitem=newMenuItem("_New Shell",GTK_STOCK_NEW);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(newPage),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//close
//	menuitem=gtk_menu_item_new_with_mnemonic("_Close Tab");
	menuitem=newMenuItem("_Close Tab",GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(exitShell),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//quit
	//menuitem=gtk_menu_item_new_with_mnemonic("_Quit");
	menuitem=newMenuItem("_Quit",GTK_STOCK_QUIT);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doShutdown),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//help
	helpMenu=gtk_menu_item_new_with_label("_Help");
	gtk_menu_item_set_use_underline((GtkMenuItem*)helpMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMenu),menu);
//about
	//menuitem=gtk_menu_item_new_with_mnemonic("_About");
	menuitem=newMenuItem("_About",GTK_STOCK_ABOUT);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doAbout),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//addmenubar
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),fileMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),helpMenu);
	gtk_box_pack_start((GtkBox*)vbox,menuBar,false,false,0);
	gtk_box_pack_start((GtkBox*)vbox,mainNotebook,true,true,0);

	gtk_container_add(GTK_CONTAINER(mainWindow),vbox);

	gtk_widget_show_all(mainWindow);
}





