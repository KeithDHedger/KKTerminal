/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
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