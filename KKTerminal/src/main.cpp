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

#include "globals.h"
#include "gui.h"

void shutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
}

int main(int argc,char **argv)
{
	gtk_init(&argc,&argv);

	buildMainGui();
	g_signal_connect(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(shutdown),NULL);
	gtk_widget_show_all(mainWindow);
	gtk_main();
}