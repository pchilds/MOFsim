/***************************************************************************
 *            proc.c
 *
 *  Sat Dec  4 17:18:14 2010
 *  Copyright  2010  Paul Childs
 *  <pchilds@physics.org>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "proc.h"

void go(GtkWidget *wgt, gpointer dta)
{
	gchar *fin, *st1;
	GtkWidget *wfl;

	wfl=gtk_file_chooser_dialog_new(_("Select Data File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(wfl), FALSE);
	gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(wfl), FALSE);
	if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
	{
		g_free(flr);
		flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
		fin=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
		if (!g_str_has_suffix(fin, ".ctl"))
		{
			st1=g_strdup(fin);
			g_free(fin);
			fin=g_strconcat(st1, ".ctl", NULL);
			g_free(st1);
		}
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mfd))) st1=g_strdup_printf("meep %s", fin);
		else st1=g_strdup_printf("meep fcn=%f fwd=%f %s", fcn, fwd, fin);
		g_free(fin);
		system(st1);
		g_free(st1);
	}
	gtk_widget_destroy(wfl);
}
