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
#include "callbacks.h"

bool	singleOverRide=false;
bool	openTerm=false;

gchar	**copyargv;
gint	copyargc;

void printargs(void)
{
  if ( copyargc > 1)
    {
    for (int j=1;j< copyargc;j++)
    	printf("arg=%i val=%s\n",j, copyargv[j]);
    }

}

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
printf("start\n");
#ifdef _USEGTK3_
	char	*tabcss=NULL;

	asprintf(&tabcss,"* {\n padding: %ipx;\n}\n",1);
	tabBoxProvider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	gtk_css_provider_load_from_data((GtkCssProvider*)tabBoxProvider,tabcss,-1,NULL);
	g_free(tabcss);
#endif

	asprintf(&prefsFile,"mkdir -p %s/.KKTerminal ||true",getenv("HOME"));
	system(prefsFile);
	freeAndNull(&prefsFile);
	asprintf(&prefsFile,"%s/.KKTerminal/kkterminal.rc",getenv("HOME"));	
	asprintf(&fontAndSize,"Monospace 10");

	loadVarsFromFile(prefsFile,mydata);

	if(windowAllocData!=NULL)
		sscanf(windowAllocData,"%i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);

	if(overideWidth>-1)
		windowWidth=overideWidth;
	if(overideHeight>-1)
		windowHeight=overideHeight;
	if(overideXPos>-1)
		windowX=overideXPos;
	if(overideYPos>-1)
		windowY=overideYPos;

	buildMainGui();
	g_signal_connect(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(doShutdown),NULL);

	if(openTerm==true)
		addPage(getenv("PWD"));

	if(execInNewTab!=NULL)
		{
			int cnt=0;
			while(execInNewTab[cnt]!=NULL)
				{
					termCommand=execInNewTab[cnt];
					newPage(NULL,NULL);
					cnt++;
				}
			g_strfreev(execInNewTab);
			execInNewTab=NULL;
		}
	gtk_widget_show_all(mainWindow);
}


gint commandline (GApplication *application,GApplicationCommandLine *command_line,gpointer user_data)
{
	char**			newtab=NULL;
	GOptionContext	*context;
	GOptionEntry	entries[]=
		{
    		{"command",'c',0,G_OPTION_ARG_STRING_ARRAY,&execInNewTab,"Exec ARG in new tab","ARG"},
    		{"new-tab",'n',0,G_OPTION_ARG_STRING_ARRAY,&newtab,"Open a new tab in ARG","ARG"},
			{NULL}
		};

	copyargv=g_application_command_line_get_arguments(command_line,&copyargc);

	if(mainWindow!=NULL)
		{
			gtk_window_present((GtkWindow*)mainWindow);

			context=g_option_context_new(NULL);
			g_option_context_set_ignore_unknown_options(context,TRUE);
			g_option_context_add_main_entries(context,entries,NULL);
			g_option_context_set_help_enabled(context,false); 
			g_option_context_parse(context,&copyargc,&copyargv,NULL);

			if(newtab!=NULL)
				{
					int cnt=0;
					while(newtab[cnt]!=NULL)
						{
							termCommand=NULL;
							addPage(newtab[cnt]);
							cnt++;
						}
					g_strfreev(newtab);
					newtab=NULL;
				}
			

			if(execInNewTab!=NULL)
				{
					int cnt=0;
					while(execInNewTab[cnt]!=NULL)
						{
							termCommand=execInNewTab[cnt];
							newPage(NULL,NULL);
							cnt++;
						}
					g_strfreev(execInNewTab);
					execInNewTab=NULL;
				}

			if (copyargc>1)
				{
					for(int j=1;j<copyargc;j++)
						{
							if((strcmp(copyargv[j],"--tab")==0) || (strcmp(copyargv[j],"-t")==0))
								{
									termCommand=NULL;
									char *cwd=g_get_current_dir();
									addPage(cwd);
									g_free(cwd);
								}
						}
			    }
		}
	return(0);
}

void printHelp(void)
{
	const char *help={"\
Usage:\n\
kkterminal [OPTION] ... [OPTION]\n\
 -h, --help		Show help options\n\
 -v, --version		Version\n\
\n\
 -m, --multiple		Multiple instance mode\n\
 -w, --width		Use width from command line\n\
 -g, --height		Use height from command line\n\
 -x, --xpos		Use xpos from command line\n\
 -y, --ypos		Use ypos from command line\n\
\n\
 -c, --command=ARG	Execute ARG in new tab\n\
 -n, --new-tab=ARG	Open a new tab in ARG\n\
 -t, --tab		Open a new tab in PWD.\n\
"};

	printf("%s\n",help);
}

gint doLocalArgs(GApplication *application,GVariantDict *options,gpointer user_data)
{
	bool			showversion=false;
	bool			showhelp=false;
	GOptionContext	*context;
	GOptionEntry	entries[]=
		{
    		{"version",'v',0,G_OPTION_ARG_NONE,&showversion,"Version",NULL},
    		{"help",'h',0,G_OPTION_ARG_NONE,&showhelp,"Help",NULL},
			{NULL}
		};

	context=g_option_context_new(NULL);
	g_option_context_set_ignore_unknown_options(context,TRUE);
	g_option_context_add_main_entries(context,entries,NULL);
	g_option_context_set_help_enabled(context,false); 
	g_option_context_parse(context,&copyargc,&copyargv,NULL);

	if(showhelp==true)
		{
			printHelp();
			exit(0);
		}

	if(showversion==true)
		{
			printf("KKTerminal - Version %s\n",VERSION);
			exit(0);
		}
	return(0);
}

int main(int argc,char **argv)
{
	char			*dbusname;
	int				status;
	char			 *sink;

	singleOverRide=false;

	GOptionContext	*context;
	GOptionEntry	entries[]=
		{
    		{"multiple",'m',0,G_OPTION_ARG_NONE,&singleOverRide,"Multiple instance mode",NULL},
			{"width",'w',0,G_OPTION_ARG_INT,&overideWidth,"Use width from command line",NULL},
			{"height",'g',0,G_OPTION_ARG_INT,&overideHeight,"Use height from command line",NULL},
			{"xpos",'x',0,G_OPTION_ARG_INT,&overideXPos,"Use xpos from command line",NULL},
			{"ypos",'y',0,G_OPTION_ARG_INT,&overideYPos,"Use ypos from command line",NULL},
			{NULL}
		};

	context=g_option_context_new(NULL);
	g_option_context_set_ignore_unknown_options(context,TRUE);
	g_option_context_add_main_entries(context,entries,NULL);
	g_option_context_set_help_enabled(context,false); 
	g_option_context_parse(context,&argc,&argv,NULL);

	gtk_init(&argc,&argv);

#ifdef _DEBUGNAME_
	sinkReturn=asprintf(&dbusname,"org.keithhedger.%s",_DEBUGNAME_);
#else
	sinkReturn=asprintf(&dbusname,"org.keithhedger%i." APPEXECNAME,getWorkspaceNumber());
#endif
	if((singleOverRide==true) ||(singleUse==false))
		{
			mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_COMMAND_LINE));
		}
	else
		{
			mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_HANDLES_COMMAND_LINE));
		}

	g_signal_connect(mainApp,"activate",G_CALLBACK(activate),NULL);
	g_signal_connect(mainApp,"startup",G_CALLBACK(appStart),NULL);
	g_signal_connect(mainApp,"command-line",G_CALLBACK(commandline),NULL);
	g_signal_connect(mainApp,"handle-local-options",G_CALLBACK(doLocalArgs),NULL);

	copyargc=argc;
	copyargv=argv;

	if(argc==1)
		openTerm=true;

	status=g_application_run(mainApp,argc,argv);
	g_object_unref(mainApp);
	return(status);
}