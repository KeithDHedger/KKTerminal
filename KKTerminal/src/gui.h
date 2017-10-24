/*
 *
 * ©K. D. Hedger. Wed 13 Jul 20:09:57 BST 2016 kdhedger68713@gmail.com

 * This file (gui.h) is part of KKTerminal.

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


#ifndef _GUI_
#define _GUI_

void buildMainGui(void);
void addPage(const char *dir);
GtkWidget *makeMenu(pageStruct *page);
void doPrefs(GtkWidget* widget,gpointer data);
void setActivePrefs(void);

#endif
