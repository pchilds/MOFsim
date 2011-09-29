/***************************************************************************
 *            pprc.c
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

#include "pprc.h"

void crs(GtkWidget *wgt, gpointer dta)
{
	AtkObject *atklbl, *atkwgt;
	GtkAdjustment *adj;
	GtkWidget *ctt, *dlg, *lbl, *spn;

	dlg=gtk_dialog_new_with_buttons(_("Source Parameters"), GTK_WINDOW(dta), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_APPLY, NULL);
	g_signal_connect_swapped(G_OBJECT(dlg), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(dlg));
	gtk_widget_show(dlg);
	ctt=gtk_dialog_get_content_area(GTK_DIALOG(dlg));
	lbl=gtk_label_new(_("Number\nof Cores:"));
	gtk_widget_show(lbl);
	gtk_box_pack_start(GTK_BOX(ctt), lbl, TRUE, TRUE, 2);
	adj=(GtkAdjustment*) gtk_adjustment_new(crn, 1, G_MAXINT8, 1, 5, 0);
	spn=gtk_spin_button_new(adj, 0.5, 0);
	gtk_widget_show(spn);
	atkwgt=gtk_widget_get_accessible(spn);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	gtk_box_pack_start(GTK_BOX(ctt), spn, TRUE, TRUE, 2);
	if (gtk_dialog_run(GTK_DIALOG(dlg))==GTK_RESPONSE_APPLY) crn=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spn));
	gtk_widget_destroy(dlg);
}

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
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mfd)))
		{
			st1=g_strdup_printf("meep %s | tee log.out", fin);
			g_free(fin);
			system(st1);
			g_free(st1);
			system("grep harminv0: log.out > tmp/log.dat");
			/*g_remove("log.out");*/
		}
		else
		{
			st1=g_strdup_printf("meep fcn=%f fwd=%f %s", fcn, fwd, fin);
			g_free(fin);
			system(st1);
			g_free(st1);
			system("h5topng -RZc dkbluered -C tmp/*.h5");
			system("convert tmp/*.png output.gif");/*check for overwrite*/
		}
		/*g_rmdir("tmp");*/
	}
	gtk_widget_destroy(wfl);
}
