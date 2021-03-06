/*
 *
 * ©K. D. Hedger. Wed 13 Jul 20:10:06 BST 2016 keithdhedger@gmail.com

 * This file (globals.cpp) is part of KKTerminal.

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
 
#include <string.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>

#include "globals.h"
#include "callbacks.h"
#include "gui.h"

GtkWidget			*mainWindow;
GtkWidget			*mainNotebook;
GtkWidget			*menuBar;

#ifdef _USEGTK3_
GtkStyleProvider	*tabBoxProvider;
#endif

//file menu
GtkWidget			*fileMenu;
//view menu
GtkWidget			*viewMenu;
//help
GtkWidget			*helpMenu;

//aplication
char				*sinkReturnStr;
char				*prefsFile=NULL;
int					sinkReturn;
bool				singleUse=true;
GApplication		*mainApp;
const char			*termCommand=NULL;
char				*windowAllocData=NULL;
GtkAccelGroup		*accGroup=NULL;
bool				holdOpen=false;
GtkClipboard		*mainClipboard;

//main mainWindow
int					windowWidth=800;
int					windowHeight=400;
int					windowX=-1;
int					windowY=-1;
int					overideWidth=-1;
int					overideHeight=-1;
int					overideXPos=-1;
int					overideYPos=-1;
bool				showMenuBar=true;

//prefs
bool				iconsInMenu=true;
bool				allowBold=true;
bool				useSingleQuotes=false;
char				*foreColour=strdup("black");
char				*backColour=strdup("white");
char				*boldColour=strdup("black");
char				*codeSet=strdup("UTF-8");

long				shortCuts[][3]={
										{GDK_KEY_N,GDK_SHIFT_MASK+GDK_CONTROL_MASK,(long)&newPage},
										{GDK_KEY_W,GDK_SHIFT_MASK+GDK_CONTROL_MASK,(long)&exitShell},
										{GDK_KEY_Q,GDK_SHIFT_MASK+GDK_CONTROL_MASK,(long)&doShutdown},
										{0,0,0},
										{0,0,0},
										{GDK_KEY_Left,GDK_SHIFT_MASK+GDK_CONTROL_MASK,(long)&prevTab},
										{GDK_KEY_Right,GDK_SHIFT_MASK+GDK_CONTROL_MASK,(long)&nextTab},
										{GDK_KEY_H,GDK_SHIFT_MASK+GDK_CONTROL_MASK,(long)&toggleMenuBar},
										{GDK_KEY_P,GDK_SHIFT_MASK+GDK_CONTROL_MASK,(long)&doPrefs},
										{-1,0,0}
									};

//shells
char				*fontAndSize=NULL;

//options
char				**execInNewTab=NULL;

args		mydata[]=
				{
					{"forecol",TYPESTRING,&foreColour},
					{"backcol",TYPESTRING,&backColour},
					{"windowsize",TYPESTRING,&windowAllocData},
					{"font",TYPESTRING,&fontAndSize},
					{"showmbar",TYPEBOOL,&showMenuBar},
					{"iconsinmenu",TYPEBOOL,&iconsInMenu},
					{"allowbold",TYPEBOOL,&allowBold},
					{"usesingle",TYPEBOOL,&useSingleQuotes},
					{"boldcolour",TYPESTRING,&boldColour},
					{"codeSet",TYPESTRING,&codeSet},
					{NULL,0,NULL}
				};

void freeAndNull(char** ptr)
{
	if(*ptr!=NULL)
		free(*ptr);

	*ptr=NULL;
}

void saveVarsToFile(char *filepath,args *dataptr)
{
	FILE	*fd=NULL;
	int		cnt=0;
	GSList	*list=NULL;

	fd=fopen(filepath,"w");
	if(fd!=NULL)
		{
			while(dataptr[cnt].name!=NULL)
				{
					switch(dataptr[cnt].type)
						{
							case TYPEINT:
								fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
								break;
							case TYPESTRING:
								fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
								break;
							case TYPEBOOL:
								fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
								break;
						}
					cnt++;
				}
			fclose(fd);
		}
}

int loadVarsFromFile(char *filepath,args *dataptr)
{
	FILE		*fd=NULL;
	char		buffer[2048];
	int			cnt;
	char		*argname=NULL;
	char		*strarg=NULL;
	char		*liststr=NULL;
	int			retval=NOERR;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					sinkReturnStr=fgets(buffer,2048,fd);
					sscanf(buffer,"%ms %ms",&argname,&strarg);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((strarg!=NULL) &&(argname!=NULL) &&(strcmp(argname,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
											case TYPEINT:
												*(int*)dataptr[cnt].data=atoi(strarg);
												break;
											case TYPESTRING:
												if(*(char**)(dataptr[cnt].data)!=NULL)
													g_free(*(char**)(dataptr[cnt].data));
												sscanf(buffer,"%*s %m[^\n]s",(char**)dataptr[cnt].data);
												break;
											case TYPEBOOL:
												*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
												break;
											case TYPELIST:
												sscanf(buffer,"%*s\t%m[^\n]s",&liststr);
												*(GSList**)dataptr[cnt].data=g_slist_append(*(GSList**)dataptr[cnt].data,liststr);
												break;
										}
								}
							cnt++;
						}
					freeAndNull(&argname);
					freeAndNull(&strarg);
				}
			fclose(fd);
		}
	else
		{
			retval=NOOPENFILE;
		}

	return(retval);
}

void writeExitData(void)
{
	GtkAllocation	alloc;
	int				winx;
	int				winy;

	gtk_widget_get_allocation(mainWindow,&alloc);
	gtk_window_get_position((GtkWindow*)mainWindow,&winx,&winy);
	if( (alloc.width>10) && (alloc.height>10) )
		sinkReturn=asprintf(&windowAllocData,"%i %i %i %i",alloc.width,alloc.height,winx,winy);

	saveVarsToFile(prefsFile,mydata);
}

void doShutdown(GtkWidget* widget,gpointer data)
{
	writeExitData();
	g_free(prefsFile);
	g_free(windowAllocData);
	g_free(fontAndSize);
	g_application_quit(mainApp);
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
		{
			retval=g_get_current_dir();
			if(retval==NULL)
				retval=g_strdup(getenv("HOME"));
		}
	return(retval);
}
