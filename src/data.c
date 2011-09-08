/***************************************************************************
 *            data.c
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

#include "data.h"

void sav(GtkWidget *wgt, gpointer dta)
{
}

void opn(GtkWidget *wgt, gpointer dta)
{
	GtkWidget *wfl;
	gint sal;
	gchar *cts=NULL, *fin=NULL, *str;
	gchar **sta=NULL;
	GError *Err=NULL;

	wfl=gtk_file_chooser_dialog_new(_("Select Data File"), GTK_WINDOW(wdw), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(wfl), FALSE);
	gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(wfl), FALSE);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
	g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
	if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
	{
		g_free(flr);
		flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
		fin=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
		if (g_file_get_contents(fin, &cts, NULL, &Err))
		{
			sta=g_strsplit_set(cts, DLMT, 0);
			sal=g_strv_length(sta);
			g_strfreev(sta);
			str=g_strdup_printf(_("File: %s successfully loaded."), fin);
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
			g_free(str);
		}
		else
		{
			str=g_strdup_printf(_("Loading failed for file: %s, Error: %s."), fin, (gchar*) Err);
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
			g_free(str);
			g_error_free(Err);
		}
		g_free(cts);
		g_free(fin);
	}
	gtk_widget_destroy(wfl);
}
