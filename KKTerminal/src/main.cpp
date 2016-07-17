/*
 *
 * ©K. D. Hedger. Thu 14 Jul 13:07:39 BST 2016 kdhedger68713@gmail.com

 * This file (main.cpp) is part of KKTerminal.

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

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <X11/Xatom.h>
#include <gdk/gdkx.h>

#include "globals.h"
#include "gui.h"

bool	singleOverRide=false;
bool	openHome=false;

int getWorkspaceNumber(void)
{
	GdkDisplay		*display;
	GdkWindow		*root_win;
	Atom			_net_current_desktop,type;
	int				format;
	unsigned long	n_items, bytes_after;
	unsigned char	*data_return=0;
	int				retnum=0;

	display=gdk_screen_get_display(gdk_screen_get_default());
	root_win=gdk_screen_get_root_window(gdk_screen_get_default());

	_net_current_desktop=gdk_x11_get_xatom_by_name_for_display(display,"_NET_CURRENT_DESKTOP");

	XGetWindowProperty(GDK_DISPLAY_XDISPLAY(display),GDK_WINDOW_XID(root_win),_net_current_desktop,0,G_MAXLONG,False,XA_CARDINAL,&type,&format,&n_items,&bytes_after,&data_return);

	if(type==XA_CARDINAL && format==32 && n_items>0)
		{
			retnum=(int)data_return[0];
			XFree(data_return);
		}
	return retnum;
}

void activate(GApplication *application)
{
	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);

}

void appStart(GApplication  *application,gpointer data)
{
	g_application_hold(application);
#ifdef _USEGTK3_
	char	*tabcss=NULL;

	asprintf(&tabcss,"* {\n padding: %ipx;\n}\n",0);
	tabBoxProvider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	gtk_css_provider_load_from_data((GtkCssProvider*)tabBoxProvider,tabcss,-1,NULL);
	g_free(tabcss);
#endif

	buildMainGui();
	g_signal_connect(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(doShutdown),NULL);
	if(openHome==true)
		addPage(getenv("HOME"));
	gtk_widget_show_all(mainWindow);
}

void open(GApplication *application,GFile** files,gint n_files,const gchar *hint)
{
	char	*filepath=NULL;
	g_application_hold(application);

	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);

	for(int i=0; i<n_files; i++)
		{
			filepath=g_file_get_path(files[i]);
			addPage(filepath);
		}
	g_application_release(application);
	termCommand=NULL;
}

int main(int argc,char **argv)
{
	GOptionContext	*context;
	char			*dbusname;
	int				status;

	singleOverRide=false;

	GOptionEntry	entries[]=
{
    {"multiple",'m',0,G_OPTION_ARG_NONE,&singleOverRide,"Multiple instance mode",NULL},
    {"command",'e',0,G_OPTION_ARG_STRING,&termCommand,"Command to inject into shell",NULL},
    { NULL }
};

	context=g_option_context_new(NULL);
	g_option_context_add_main_entries(context,entries,NULL);
	g_option_context_set_help_enabled(context,true); 
	g_option_context_parse(context,&argc,&argv,NULL);

	gtk_init(&argc,&argv);

	sinkReturn=asprintf(&dbusname,"org.keithhedger%i." PACKAGE,getWorkspaceNumber());
	if((singleOverRide==true) ||(singleUse==false))
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_OPEN));
	else
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_HANDLES_OPEN));

	g_signal_connect(mainApp,"activate",G_CALLBACK(activate),NULL);
	g_signal_connect(mainApp,"startup",G_CALLBACK(appStart),NULL);
	g_signal_connect(mainApp,"open",G_CALLBACK(open),NULL);

	if(argc==1)
		openHome=true;

	status=g_application_run(mainApp,argc,argv);
	g_object_unref(mainApp);
	return(status);
}