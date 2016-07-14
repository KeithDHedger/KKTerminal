/*
 *
 * ©K. D. Hedger. Wed 13 Jul 20:10:11 BST 2016 kdhedger68713@gmail.com

 * This file (globals.h) is part of KKTerminal.

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

#ifndef _GLOBALS_
#define _GLOBALS_

#include "globals.h"

struct	args
{
	const char		*name;
	int				type;
	void			*data;
};

enum {TYPEINT=1,TYPESTRING,TYPEBOOL,TYPELIST};
enum {NOERR=0,NOOPENFILE,NOSAVEFILE};

extern char*		foreColour;
extern char*		backColour;

extern args			mydata[]; 
extern GtkWidget	*mainWindow;
extern GtkWidget	*mainNotebook;

void freeAndNull(char **ptr);
int loadVarsFromFile(char *filepath,args *dataptr);

#endif
