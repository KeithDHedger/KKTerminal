/*
 *
 * ©K. D. Hedger. Wed 13 Jul 20:10:06 BST 2016 kdhedger68713@gmail.com

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

#include "globals.h"

GtkWidget	*mainWindow;
GtkWidget	*mainNotebook;

char		*foreColour=strdup("#000000");
char		*backColour=strdup("#ffffff");
char		*sinkReturnStr;
char		*prefsFile=NULL;

args		mydata[]=
				{
					{"forecol",TYPESTRING,&foreColour},
					{"backcol",TYPESTRING,&backColour},
					{NULL,0,NULL}
				};

void freeAndNull(char** ptr)
{
	if(*ptr!=NULL)
		free(*ptr);

	*ptr=NULL;
}

void doShutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
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
