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

	popmenuitem=gtk_menu_item_new_with_label("Toggle Menu Bar");
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(toggleMenuBar),page);
	gtk_menu_shell_append(GTK_MENU_SHELL(retwidg),popmenuitem);

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
	if(showMenuBar==true)
		gtk_widget_show_all(menuBar);
	else
		gtk_widget_hide(menuBar);

	page->menu=makeMenu(page);
	if(termCommand!=NULL)
		{
			vte_terminal_feed_child((VteTerminal*)page->terminal,termCommand,-1);
			vte_terminal_feed_child((VteTerminal*)page->terminal,"\n",-1);
		}
}

#ifdef _USEGTK3_
void showidg (GtkWidget *widget)
{
if(widget!=NULL)
	printf("widget\n");
	//gtk_style_context_add_provider(gtk_widget_get_style_context (widget),widgprovider,GTK_STYLE_PROVIDER_PRIORITY_USER);
	if (GTK_IS_CONTAINER (widget))
		gtk_container_forall (GTK_CONTAINER(widget),(GtkCallback)showidg,NULL);
}
#endif

//GtkWidget* newMenuItem(const char* menuname,const char* stockid,char hotkey,GdkModifierType modkeys)
GtkWidget* newMenuItem(const char* menuname,const char* stockid,int shortnum,const char* hotkey)
{
	GtkWidget	*menu;
#ifdef _USEGTK3_
	char		*menulabel;
	GtkWidget	*menuhbox;
	GtkWidget	*pad;
	GtkWidget	*image;
//	GtkWidget	*leftlabel;
	GtkWidget	*ritelabel;

	menu=gtk_menu_item_new_with_mnemonic(menuname);
	if(iconsInMenu==true)
		{
			gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(menu)));
			menuhbox=createNewBox(NEWHBOX,false,0);
			pad=createNewBox(NEWHBOX,false,0);

			image=gtk_image_new_from_icon_name(stockid,GTK_ICON_SIZE_MENU);
			gtk_box_pack_start((GtkBox*)menuhbox,image,false,false,0);

			gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new(" "),false,false,0);
			gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new_with_mnemonic(menuname),false,false,0);
			gtk_box_pack_start(GTK_BOX(menuhbox),pad,true,true,0);

			ritelabel=gtk_label_new(hotkey);
			gtk_widget_set_sensitive(ritelabel,false);
			gtk_box_pack_start(GTK_BOX(menuhbox),ritelabel,false,false,8);

			gtk_container_add(GTK_CONTAINER(menu),menuhbox);
		}
	else
		{
			if(shortCuts[shortnum][0]>0)
				gtk_widget_add_accelerator((GtkWidget *)menu,"activate",accGroup,shortCuts[shortnum][0],(GdkModifierType)shortCuts[shortnum][1],GTK_ACCEL_VISIBLE);
		}

#else
	menu=gtk_image_menu_item_new_from_stock(stockid,NULL);
	if(shortCuts[shortnum][0]>0)
		gtk_widget_add_accelerator((GtkWidget *)menu,"activate",accGroup,shortCuts[shortnum][0],(GdkModifierType)shortCuts[shortnum][1],GTK_ACCEL_VISIBLE);
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

	accGroup=gtk_accel_group_new();
	gtk_window_add_accel_group((GtkWindow*)mainWindow,accGroup);

	g_signal_connect(G_OBJECT(mainWindow),"key-press-event",G_CALLBACK(keyShortCut),NULL);

	mainNotebook=gtk_notebook_new();
	gtk_notebook_set_scrollable((GtkNotebook*)mainNotebook,true);

	gtk_notebook_set_show_tabs((GtkNotebook*)mainNotebook,true);
#ifndef _USEGTK3_
	g_object_set((GObject*)mainNotebook,"tab-vborder",0,NULL);
#endif

#ifdef _USEGTK3_
	applyCSS((GtkWidget*)mainNotebook,tabBoxProvider);
	gtk_style_context_reset_widgets(gdk_screen_get_default());
#endif

//menus
	menuBar=gtk_menu_bar_new();
//file menu
	fileMenu=gtk_menu_item_new_with_label("_File");
	gtk_menu_item_set_use_underline((GtkMenuItem*)fileMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenu),menu);
//new
	menuitem=newMenuItem("_New Shell",GTK_STOCK_NEW,NEWPAGEMENU,"Shift+Ctrl+N");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(newPage),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//gtk_widget_destroy(gtk_bin_get_child (GTK_BIN (menuitem)));
//GtkWidget	*mbox=createNewBox(NEWHBOX,false,0);
//GtkWidget	*image=gtk_image_new_from_icon_name(GTK_STOCK_NEW,GTK_ICON_SIZE_MENU);
//
//gtk_box_pack_start((GtkBox*)mbox,image,false,false,0);
//gtk_box_pack_start((GtkBox*)mbox,gtk_label_new(" New Tab			Shift+Ctrl+N"),false,false,0);
//gtk_container_add(GTK_CONTAINER(menuitem),mbox);

#if 1
//close
	menuitem=newMenuItem("_Close Tab",GTK_STOCK_CLOSE,CLOSEPAGEMENU,"Shift+Ctrl+W");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(exitShell),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//quit
	menuitem=newMenuItem("_Quit",GTK_STOCK_QUIT,QUITMENU,"Shift+Ctrl+Q");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doShutdown),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//menuitem=gtk_check_menu_item_new_with_label ("checkbox");
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);


#endif
//view
	viewMenu=gtk_menu_item_new_with_label("_View");
	gtk_menu_item_set_use_underline((GtkMenuItem*)viewMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(viewMenu),menu);
//hide mbar
	menuitem=gtk_menu_item_new_with_mnemonic("_Hide Menu Bar");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(toggleMenuBar),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
//prevtab
	menuitem=newMenuItem("_Previous Tab",GTK_STOCK_GO_BACK,PREVTABMENU,"Shift+Ctrl+Left");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(prevTab),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//next tab
	menuitem=newMenuItem("_Next Tab",GTK_STOCK_GO_FORWARD,NEXTTABMENU,"Shift+Ctrl+Right");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(nextTab),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//help
	helpMenu=gtk_menu_item_new_with_label("_Help");
	gtk_menu_item_set_use_underline((GtkMenuItem*)helpMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMenu),menu);
//about
	menuitem=newMenuItem("_About",GTK_STOCK_ABOUT,ABOUTMENU,NULL);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doAbout),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//online help
	menuitem=newMenuItem("_Help",GTK_STOCK_HELP,ONLINEHELPMENU,NULL);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doHelp),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//addmenubar
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),fileMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),viewMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),helpMenu);
	gtk_box_pack_start((GtkBox*)vbox,menuBar,false,false,0);
	gtk_box_pack_start((GtkBox*)vbox,mainNotebook,true,true,0);

	gtk_container_add(GTK_CONTAINER(mainWindow),vbox);

	gtk_widget_show_all(mainWindow);
	if(showMenuBar==true)
		gtk_widget_show_all(menuBar);
	else
		gtk_widget_hide(menuBar);
}





