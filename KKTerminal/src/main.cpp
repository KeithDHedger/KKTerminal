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
#include <getopt.h>

#include "globals.h"
#include "gui.h"
#include "callbacks.h"

bool		singleOverRide=false;
bool		openTerm=false;

gchar		**copyargv;
gint		copyargc;
struct		option longOptions[]=
		{
			{"multiple",0,0,'n'},
			{"width",1,0,'w'},
			{"height",1,0,'g'},
			{"xpos",1,0,'x'},
			{"ypos",1,0,'y'},
			{"command",1,0,'c'},
			{"new-tab",1,0,'n'},
			{"tab",0,0,'t'},
			{"hold",0,0,'l'},
			{"codeset",1,0,'s'},
			{"help",0,0,'h'},
			{0, 0, 0, 0}
		};

const char	*shortOpts="h?w:g:x:y:n:c:s:lmte";

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

void printHelp(void)
{
	const char *help={"\
Usage:\n\
kkterminal [OPTION] ... [OPTION]\n\
 -h, --help		Show help options\n\
 -v, --version		Version\n\
\n\
 -m, --multiple		Multiple instance mode\n\
 -l, --hold		Hold tab open afer last proc exits\n\
 -w, --width		Use width from command line\n\
 -g, --height		Use height from command line\n\
 -x, --xpos		Use xpos from command line\n\
 -y, --ypos		Use ypos from command line\n\
\n\
 -c, --command=ARG	Execute ARG in new tab\n\
 -n, --new-tab=ARG	Open a new tab in ARG\n\
 -t, --tab		Open a new tab in PWD.\n\
 -e ...			Consume the rest of the command line and execute it in a new tab.\n\
\n\
 -s, --codeset		Set the encoding ( default=UTF-8 ).\n\
 			The codeset effects all tabs opened after setting.\n\
			The last codeset used is remembered.\
"};

	printf("%s\n",help);
	exit(0);
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

	sinkReturn=asprintf(&tabcss,"* {\n padding: %ipx;\n}\n",1);
	tabBoxProvider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	gtk_css_provider_load_from_data((GtkCssProvider*)tabBoxProvider,tabcss,-1,NULL);
	g_free(tabcss);
#endif

	sinkReturn=asprintf(&prefsFile,"mkdir -p %s/.KKTerminal ||true",getenv("HOME"));
	sinkReturn=system(prefsFile);
	freeAndNull(&prefsFile);
	sinkReturn=asprintf(&prefsFile,"%s/.KKTerminal/kkterminal.rc",getenv("HOME"));	
	sinkReturn=asprintf(&fontAndSize,"Monospace 10");

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
	if(showMenuBar==true)
		gtk_widget_show_all(menuBar);
	else
		gtk_widget_hide(menuBar);

}

gint commandline (GApplication *application,GApplicationCommandLine *command_line,gpointer user_data)
{
	int		c=0;
	int		option_index=0;
	char	*cwd;
	GString *str;

	optind=0;
	copyargv=g_application_command_line_get_arguments(command_line,&copyargc);

	if(mainWindow!=NULL)
		{
			gtk_window_present((GtkWindow*)mainWindow);
			while (1)
				{
					option_index=0;
					c=getopt_long_only(copyargc,copyargv,shortOpts,longOptions,&option_index);
					if (c==-1)
						break;

					switch (c)
						{
							case 'h':
								printHelp();
								break;

							case 'l':
								holdOpen=true;
								break;;

							case 'c':
								termCommand=optarg;
								newPage(NULL,NULL);
								break;
							case 'e':
								str=g_string_new("");
								for(int k=optind;k<copyargc;k++)
									g_string_append_printf(str,"%s ",copyargv[k]);
								termCommand=str->str;
								cwd=g_get_current_dir();
								addPage(cwd);
								g_free(cwd);
								g_string_free(str,true);
								return(-1);
								break;
							case 'n':
								termCommand=NULL;
								addPage(optarg);
								break;
							case 't':
								{
									termCommand=NULL;
									char *cwd=g_get_current_dir();
									addPage(cwd);
									g_free(cwd);
								}
								break;
							case 's':
								free(codeset);
								codeset=strdup(optarg);
								break;
						}
				}
		}

	return(-1);
}

int main(int argc,char **argv)
{
	char			*dbusname;
	int				status;
	char			 *sink;
	int				c=0;
	int				option_index=0;
	char			*cwd;
	GString			*str;
	bool			loop=true;

	singleOverRide=false;
	optind=0;

	gtk_init(&argc,&argv);
	copyargc=argc;
	copyargv=argv;

	while (loop)
		{
			option_index=0;
			c=getopt_long_only(copyargc,copyargv,shortOpts,longOptions,&option_index);
			if (c==-1)
				break;

			switch (c)
				{
					case 'h':
					case '?':
						printHelp();
						exit(0);
						break;

					case 'e':
						loop=false;
						continue;
						break;
						
					case 'l':
						holdOpen=true;
						break;;
					case 'm':
						singleOverRide=true;
						break;
					case 'w':
						overideWidth=atoi(optarg);
						break;
					case 'g':
						overideHeight=atoi(optarg);
						break;
					case 'x':
						overideXPos=atoi(optarg);
						break;
					case 'y':
						overideYPos=atoi(optarg);
						break;


				}
		}

#ifdef _DEVMODE_
	sinkReturn=asprintf(&dbusname,"org.keithhedger.%s","devel");
#else
	sinkReturn=asprintf(&dbusname,"org.keithhedger%i." APPEXECNAME,getWorkspaceNumber());
#endif
	if((singleOverRide==true) ||(singleUse==false))
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_COMMAND_LINE));
	else
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_HANDLES_COMMAND_LINE));

	g_signal_connect(mainApp,"activate",G_CALLBACK(activate),NULL);
	g_signal_connect(mainApp,"startup",G_CALLBACK(appStart),NULL);
	g_signal_connect(mainApp,"command-line",G_CALLBACK(commandline),NULL);

	if(argc==1)
		openTerm=true;

	status=g_application_run(mainApp,argc,argv);
	g_object_unref(mainApp);
	return(status);
}