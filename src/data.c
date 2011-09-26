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

void opn(GtkWidget *wgt, gpointer dta)
{
	gchar *cts=NULL, *fin=NULL, *str;
	gchar **sta=NULL;
	gint sal;
	GError *Err=NULL;
	GtkWidget *wfl;

	wfl=gtk_file_chooser_dialog_new(_("Select Data File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
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

void sav(GtkWidget *wgt, gpointer dta)
{
	DrawCirc *circ;
	DrawCircData *dcd;
	DrawCircGroup *dcg;
	FdtdMat *mat;
	gchar *cts=NULL, *fot=NULL, *st1=NULL, *st2=NULL, *st3=NULL;
	gdouble xn, xx, yn, yx;
	GError *Err=NULL;
	gint j, k;
	GtkWidget *wfl;

	wfl=gtk_file_chooser_dialog_new(_("Select Data File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
	gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(wfl), FALSE);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfl), TRUE);
	if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
	{
		g_free(flr);
		flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
		fot=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
		if (!g_str_has_suffix(fot, ".ctl"))
		{
			st2=g_strdup(fot);
			g_free(fot);
			fot=g_strconcat(st2, ".ctl", NULL);
			g_free(st2);
		}
		circ=DRAW_CIRC(crc);
		g_object_get(G_OBJECT(crc), "xmin", &xn, "xmax", &xx, "ymin", &yn, "ymax", &yx, NULL);
		cts=g_strdup("(reset-meep)");
		st1=g_strdup_printf("(set! geometry-lattice (make lattice (size %f %f no-size)))", xx-xn+(2*PML), yx-yn+(2*PML));
		st2=g_strjoin(DLMT, cts, st1, NULL);
		{g_free(cts); g_free(st1);}
		for (j=0;j<(mtr->len);j++)
		{
			mat=(FdtdMat*) g_ptr_array_index(mtr, j);
			st3=g_strdup(" ");
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di1)))
			{
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di2)))
				{
					cts=g_strdup_printf("(epsilon-diag %f %f %f) (epsilon-offdiag %f %f %f)", (mat->d11), (mat->d22), (mat->d33), (mat->d12), (mat->d13), (mat->d23));
					st1=g_strjoin(" ", st3, cts, NULL);
					{g_free(st3); g_free(cts);}
					st3=g_strdup(st1);
					g_free(st1);
				}
				else
				{
					cts=g_strdup_printf("(epsilon %f)", (mat->d11));
					st1=g_strjoin(" ", st3, cts, NULL);
					{g_free(st3); g_free(cts);}
					st3=g_strdup(st1);
					g_free(st1);
				}
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di3)))
				{
				}
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di4)))
				{
					cts=g_strdup_printf("(D-conductivity %f)", (mat->dsg));
					st1=g_strjoin(" ", st3, cts, NULL);
					{g_free(st3); g_free(cts);}
					st3=g_strdup(st1);
					g_free(st1);
				}
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di5)))
				{
					cts=g_strdup_printf("(chi2 %f)", (mat->dx2));
					st1=g_strjoin(" ", st3, cts, NULL);
					{g_free(st3); g_free(cts);}
					st3=g_strdup(st1);
					g_free(st1);
				}
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di6)))
				{
					cts=g_strdup_printf("(chi3 %f)", (mat->dx3));
					st1=g_strjoin(" ", st3, cts, NULL);
					{g_free(st3); g_free(cts);}
					st3=g_strdup(st1);
					g_free(st1);
				}
			}
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg1)))
			{
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg2)))
				{
					cts=g_strdup_printf("(mu-diag %f %f %f) (mu-offdiag %f %f %f)", (mat->m11), (mat->m22), (mat->m33), (mat->m12), (mat->m13), (mat->m23));
					st1=g_strjoin(" ", st3, cts, NULL);
					{g_free(st3); g_free(cts);}
					st3=g_strdup(st1);
					g_free(st1);
				}
				else
				{
					cts=g_strdup_printf("(mu %f)", (mat->m11));
					st1=g_strjoin(" ", st3, cts, NULL);
					{g_free(st3); g_free(cts);}
					st3=g_strdup(st1);
					g_free(st1);
				}
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg3)))
				{
				}
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg4)))
				{
					cts=g_strdup_printf("(B-conductivity %f)", (mat->msg));
					st1=g_strjoin(" ", st3, cts, NULL);
					{g_free(st3); g_free(cts);}
					st3=g_strdup(st1);
					g_free(st1);
				}
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg5)))
				{
				}
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg6)))
				{
				}
			}
			st1=g_strdup_printf("(define %s (make material-type%s))", (mat->nme), st3);
			g_free(st3);
			cts=g_strjoin(DLMT, st2, st1, NULL);
			{g_free(st1); g_free(st2);}
			st2=g_strdup(cts);
			g_free(cts);
		}
		for (j=0;j<((circ->data)->len);j++)
		{
			dcg=(DrawCircGroup*) g_ptr_array_index((circ->data), j);
			if ((dcg->col)<0) st3=g_strdup("air");
			else st3=g_strdup(((FdtdMat*) g_ptr_array_index(mtr, (dcg->col)))->nme);
			for (k=0;k<((dcg->xyr)->len);k++)
			{
				dcd=(DrawCircData*) g_array_index((dcg->xyr), DrawCircData*, k);
				st1=g_strdup_printf("(set! geometry (list (make cylinder (center %f %f) (height infinity) (radius %f) (material %s))))", (dcd->x), (dcd->y), (dcd->r), st3);
				cts=g_strjoin(DLMT, st2, st1, NULL);
				{g_free(st1); g_free(st2);}
				st2=g_strdup(cts);
				g_free(cts);
			}
			g_free(st3);
		}
		st1=g_strdup_printf("(set! pml-layers (list (make pml (thickness %f))))", PML);
		cts=g_strjoin(DLMT, st2, st1, NULL);
		{g_free(st1); g_free(st2);}
		st1=g_strdup("(set! resolution 10)");
		st2=g_strjoin(DLMT, cts, st1, NULL);
		{g_free(st1); g_free(cts);}
		st1=g_strdup_printf("(define-param fcn %f)", fcn);
		cts=g_strjoin(DLMT, st2, st1, NULL);
		{g_free(st1); g_free(st2);}
		st1=g_strdup_printf("(define-param fwd %f)", fwd);
		st2=g_strjoin(DLMT, cts, st1, NULL);
		{g_free(st1); g_free(cts);}
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(ex))) st3=g_strdup("Ex");
		else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(ey))) st3=g_strdup("Ey");
		else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(hz))) st3=g_strdup("Hz");
		else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(hx))) st3=g_strdup("Hx");
		else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(hy))) st3=g_strdup("Hy");
		else st3=g_strdup("Ez");
		st1=g_strdup_printf("(set! sources (list (make source (src (make gaussian-src (frequency fcn) (fwidth fwd))) (component %s) (center 0 0) (size %f %f))))", st3, fsz, fsz);
		cts=g_strjoin(DLMT, st2, st1, NULL);
		{g_free(st1); g_free(st2);}
		st1=g_strdup("(use-output-directory tmp)");
		st2=g_strjoin(DLMT, cts, st1, NULL);
		{g_free(st1); g_free(cts);}
		st1=g_strdup_printf("(run-sources+ 300 (at-beginning output-epsilon) (after-sources (harminv %s (vector3 0) fcn fwd)))", st3);
		cts=g_strjoin(DLMT, st2, st1, NULL);
		{g_free(st1); g_free(st2);}
		if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mfd)))
		{
			st1=g_strdup_printf("(run-until (/ 1 fcn) (at-every (/ 1 fcn 20) (output-png %s \"-Zc bluered\")))", st3);
			st2=g_strjoin(DLMT, cts, st1, NULL);
			{g_free(st1); g_free(cts);}
			g_file_set_contents(fot, st2, -1, &Err);
			g_free(st2);
		}
		else
		{
			g_file_set_contents(fot, cts, -1, &Err);
			g_free(cts);
		}
		g_free(st3);
		if (Err)
		{
			st1=g_strdup_printf(_("Error Saving file: %s."), (Err->message));
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), st1), st1);
			g_free(st1);
			g_error_free(Err);
		}
		g_free(fot);
	}
	gtk_widget_destroy(wfl);
}
