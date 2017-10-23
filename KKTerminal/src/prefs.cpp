
/******************************************************
*
*     ©keithhedger Mon 23 Oct 14:08:12 BST 2017
*     kdhedger68713@gmail.com
*
*     prefs.cpp
* 
******************************************************/
#include <string.h>

#include "prefs.h"
#include "globals.h"
#include "callbacks.h"

GtkWidget	*prefsCheck[4];
GtkWidget	*prefsText[4];

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

void makePrefsCheck(int widgnum,const char *label,bool onoff,GtkBox *box)
{
	prefsCheck[widgnum]=gtk_check_button_new_with_label(label);
	gtk_widget_set_name(prefsCheck[widgnum],label);
	gtk_toggle_button_set_active((GtkToggleButton*)prefsCheck[widgnum],onoff);
	gtk_box_pack_start(box,prefsCheck[widgnum],false,false,0);
	g_signal_connect(G_OBJECT(prefsCheck[widgnum]),"clicked",G_CALLBACK(setPrefs),(void*)(long)(widgnum+1000));
}
 
void makePrefsText(int widgnum,const char *label,const char *defaulttxt,GtkBox *box,bool showlabel)
{
	GtkWidget	*hbox=createNewBox(NEWHBOX,false,0);
	GtkWidget	*pad=createNewBox(NEWHBOX,false,0);

	prefsText[widgnum]=gtk_entry_new();
	if(defaulttxt!=NULL)
		gtk_entry_set_text((GtkEntry*)prefsText[widgnum],defaulttxt);
	if((strlen(label)>0) && (showlabel==true))
		{
			gtk_box_pack_start((GtkBox*)hbox,gtk_label_new(label),false,false,0);
			gtk_box_pack_start((GtkBox*)hbox,pad,true,false,0);
			gtk_box_pack_start((GtkBox*)hbox,prefsText[widgnum],false,false,0);
			gtk_box_pack_start(box,hbox,false,false,0);
		}
	else
		gtk_box_pack_start(box,prefsText[widgnum],false,false,0);
}

void doPrefs(GtkWidget* widget,gpointer data)
{
	GtkBox		*vbox;
	GtkWidget	*hbox;
	GtkBox		*tbox;
	GtkWidget	*item;

	prefsWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)prefsWindow,"Preferences");
	vbox=(GtkBox*)createNewBox(NEWVBOX,false,0);

	makePrefsCheck(SHOWMENUBARCHK,"Show Menu Bar",showMenuBar,vbox);
	makePrefsCheck(SHOWICONSCHK,"Show Icons",iconsInMenu,vbox);
	makePrefsCheck(ALLOWBOLDCHK,"Allow Bold",allowBold,vbox);
	makePrefsCheck(USESINGLECHK,"Use Single Quotes",useSingleQuotes,vbox);
	gtk_box_pack_start(GTK_BOX(vbox),createNewBox(NEWHBOX,false,0),false,false,10);
	makePrefsText(TEXTCOLOURTXT,"Text Colour",foreColour,vbox,true);
	makePrefsText(BACKGROUNDCOLOURTXT,"Background Colour",backColour,vbox,true);
	makePrefsText(BOLDCOLOURTXT,"Bold Colour",boldColour,vbox,true);
	makePrefsText(FONTTXT,"Font",fontAndSize,vbox,true);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif

	hbox=createNewBox(NEWHBOX,true,4);

#ifdef _USEGTK3_
	item=gtk_button_new_with_mnemonic("_Apply");
#else
	item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
#endif

	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)-1);	

#ifdef _USEGTK3_
	item=gtk_button_new_with_mnemonic("_Cancel");
#else
	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
#endif
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)-2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);
	gtk_container_add(GTK_CONTAINER(prefsWindow),(GtkWidget*)vbox);
	gtk_widget_show_all(prefsWindow);
}
