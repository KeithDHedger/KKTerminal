
/******************************************************
*
*     ©keithhedger Tue 26 Jul 13:08:22 BST 2016
*     kdhedger68713@gmail.com
*
*     prefs.cpp
* 
******************************************************/

#include "globals.h"
#include "callbacks.h"
#include "prefs.h"

GtkWidget	*prefsCheck[2];
GtkWidget	*prefsText[3];
GtkWidget	*prefsWindow;

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

GtkWidget *createNewStockButton(const char *stock,const char *label)
{
	GtkWidget	*button;
#ifdef _USEGTK3_
	button=gtk_button_new_with_mnemonic(label);
#else
	button=gtk_button_new_from_stock(stock);
#endif

	return(button);
}

void makePrefsCheck(int widgnum,const char *label,bool onoff,GtkBox *box)
{
	prefsCheck[widgnum]=gtk_check_button_new_with_label(label);
	gtk_widget_set_name(prefsCheck[widgnum],label);
	gtk_toggle_button_set_active((GtkToggleButton*)prefsCheck[widgnum],onoff);
	gtk_box_pack_start(box,prefsCheck[widgnum],false,false,0);
}

void makePrefsText(int widgnum,const char *label,char *defaulttxt,GtkBox *box)
{
	GtkWidget	*hbox=createNewBox(NEWHBOX,false,0);
	GtkWidget	*pad=createNewBox(NEWHBOX,false,0);

	gtk_box_pack_start((GtkBox*)hbox,gtk_label_new(label),false,false,0);
	gtk_box_pack_start((GtkBox*)hbox,pad,true,false,0);
	prefsText[widgnum]=gtk_entry_new();
	gtk_entry_set_text((GtkEntry*)prefsText[widgnum],defaulttxt);
	gtk_box_pack_start((GtkBox*)hbox,prefsText[widgnum],false,false,0);
	gtk_box_pack_start(box,hbox,false,false,0);
}

void doPrefs(GtkWidget* widget,gpointer data)
{
	GtkBox		*vbox;
	GtkWidget	*hbox;
	GtkWidget	*item;

	prefsWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	vbox=(GtkBox*)createNewBox(NEWVBOX,false,0);

	makePrefsCheck(SHOWMENUBARCHK,"Show Menu Bar",showMenuBar,vbox);
	makePrefsCheck(SHOWICONSCHK,"Show Icons",iconsInMenu,vbox);
	makePrefsText(TEXTCOLOURTXT,"Text Colour",foreColour,vbox);
	makePrefsText(BACKGROUNDCOLOURTXT,"Background Colour",backColour,vbox);
	makePrefsText(FONTTXT,"Font",fontAndSize,vbox);
//buttons
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif

	hbox=createNewBox(NEWHBOX,true,4);
	item=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);

	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)1);	

	item=createNewStockButton(GTK_STOCK_CANCEL,GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),NULL);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);

	gtk_container_add(GTK_CONTAINER(prefsWindow),(GtkWidget*)vbox);
	gtk_widget_show_all(prefsWindow);
}
