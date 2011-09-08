/***************************************************************************
 *            util.c
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

#include "util.h"
#define RT3_2  0.8660254037844386467637231707529361834714026269052 /* pi/3 */
#define NRT3_2  0.8660254037844386467637231707529361834714026269052 /* negative of this. */

void abt(GtkWidget *wgt, gpointer dta)
{
	GtkWidget *hwn;

	hwn=gtk_about_dialog_new();
	g_signal_connect_swapped(G_OBJECT(hwn), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(hwn));
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(hwn), "0.1.0");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(hwn), "(c) Paul Childs, 2011");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(hwn), _("MOFsim is a Graphical User Interface for performing Finite Difference Time Domain simulations on microstructured optical fibre using meep by AbInitio."));
	gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(hwn), _("translator-credits"));
	gtk_widget_show(hwn);
	g_signal_connect_swapped(G_OBJECT(hwn), "response", G_CALLBACK(gtk_widget_destroy), G_OBJECT(hwn));
}

void add(GtkWidget *wgt, gpointer dta)
{
	DrawCirc *circ;
	DrawCircData *dtm;
	FdtdLayout *lyt;
	gdouble c, c1, c2, c3, s, s1, s2, s3;
	gint j, k, l;
	GSList *dat=NULL;

	circ=DRAW_CIRC(crc);
	if (grp>=0)
	{
		lyt=(FdtdLayout*)g_ptr_array_index(fbl, grp);
		(lyt->geo)=gtk_combo_box_get_active(GTK_COMBO_BOX(cb1));
		/*send data to drawcirc*/
	}
	else
	{
		lyt=g_new(FdtdLayout, 1);
		dtm=g_new(DrawCircData, 1);
		(dtm->r)=(lyt->rr);
		g_ptr_array_add(fbl, (gpointer) lyt);
		(lyt->geo)=gtk_combo_box_get_active(GTK_COMBO_BOX(cb1));
		j=g_slist_length(circ->data);
		g_array_append_val((circ->ind), j);
		switch (lyt->geo)
		{
			case 2:
			{c1=(lyt->pitch)*cos(lyt->azim); s1=(lyt->pitch)*sin(lyt->azim);}
			{c2=s1*NRT3_2; s2=c1*RT3_2;}
			{c3=(c1/2)-c2; s3=(s1/2)-s2;}
			{c2+=(c1/2); s2+=(s1/2);}
			{(dtm->x)=(lyt->xc)+(c1*(lyt->ring)); (dtm->y)=(lyt->yc)+(s1*(lyt->ring));}
			for (j=0; j<(lyt->ring); j++)
			{
				dat=g_slist_prepend(dat, (gpointer) dtm);
				{(dtm->x)+=c2; (dtm->y)+=s2;}
			}
			for (j=0; j<(lyt->ring); j++)
			{
				dat=g_slist_prepend(dat, (gpointer) dtm);
				{(dtm->x)-=c1; (dtm->y)-=s1;}
			}
			for (j=0; j<(lyt->ring); j++)
			{
				dat=g_slist_prepend(dat, (gpointer) dtm);
				{(dtm->x)+=c3; (dtm->y)+=s3;}
			}
			for (j=0; j<(lyt->ring); j++)
			{
				dat=g_slist_prepend(dat, (gpointer) dtm);
				{(dtm->x)-=c2; (dtm->y)-=s2;}
			}
			for (j=0; j<(lyt->ring); j++)
			{
				dat=g_slist_prepend(dat, (gpointer) dtm);
				{(dtm->x)+=c1; (dtm->y)+=s1;}
			}
			{dat=g_slist_prepend(dat, dtm); j=1;}
			while (j<(lyt->ring))
			{
				{(dtm->x)-=c3; (dtm->y)-=s3; j++;}
				dat=g_slist_prepend(dat, (gpointer) dtm);
			}
			(circ->data)=g_slist_append((circ->data), (gpointer) dat);
			j*=6;
			g_array_append_val((circ->sizes), j);
			break;
			case 1:
			{c1=(lyt->pitch)*cos(lyt->azim); s1=(lyt->pitch)*sin(lyt->azim);}
			{c2=s1*NRT3_2; s2=c1*RT3_2;}
			{c3=(c1/2)-c2; s3=(s1/2)-s2;}
			{c2+=(c1/2); s2+=(s1/2);}
			{(dtm->x)=(lyt->xc); (dtm->y)=(lyt->yc);}
			for (k=1; k<=(lyt->ring); k++)
			{
				{(dtm->x)+=c1; (dtm->y)+=s1;}
				dat=g_slist_prepend(dat, (gpointer) dtm);
				for (j=0; j<k; j++)
				{
					{(dtm->x)+=c2; (dtm->y)+=s2;}
					dat=g_slist_prepend(dat, (gpointer) dtm);
				}
				for (j=0; j<k; j++)
				{
					{(dtm->x)-=c1; (dtm->y)-=s1;}
					dat=g_slist_prepend(dat, (gpointer) dtm);
				}
				for (j=0; j<k; j++)
				{
					{(dtm->x)+=c3; (dtm->y)+=s3;}
					dat=g_slist_prepend(dat, (gpointer) dtm);
				}
				for (j=0; j<k; j++)
				{
					{(dtm->x)-=c2; (dtm->y)-=s2;}
					dat=g_slist_prepend(dat, (gpointer) dtm);
				}
				for (j=0; j<k; j++)
				{
					{(dtm->x)+=c1; (dtm->y)+=s1;}
					dat=g_slist_prepend(dat, (gpointer) dtm);
				}
				j=1;
				while (j<k)
				{
					{(dtm->x)-=c3; (dtm->y)-=s3; j++;}
					dat=g_slist_prepend(dat, (gpointer) dtm);
				}
				{(dtm->x)-=c3; (dtm->y)-=s3;}
			}
			(circ->data)=g_slist_append((circ->data), (gpointer) dat);
			j=(lyt->ring);
			j*=3*(j+1);
			g_array_append_val((circ->sizes), j);
			break;
			default:
			(dtm->x)=(lyt->xc);
			(dtm->y)=(lyt->yc);
			(circ->data)=g_slist_append((circ->data), (gpointer) dtm);
			j=1;
			g_array_append_val((circ->sizes), j);
			break;
		}
	}
	(lyt->mat)=gtk_combo_box_get_active(GTK_COMBO_BOX(cb2));
	/*reset display when done*/
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), -1);
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), -1);
	{grp=-1; (circ->hgh)=-1;}
}

void chl(DrawCirc *circ, gpointer data)
{
	FdtdLayout *lyt;

	grp=(circ->hgh);
	if (grp>=0)
	{
		lyt=(FdtdLayout*) g_ptr_array_index(fbl, grp);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), (lyt->geo));
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), (lyt->mat));
	}
	else
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), -1);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), -1);
	}
}

void cmv(DrawCirc *circ, gpointer data)
{
	gchar *str;

	str=g_strdup_printf(_("x: %f, y: %f"), (circ->xps), (circ->yps));
	gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
	g_free(str);
}

void del(GtkWidget *wgt, gpointer dta)
{
	DrawCirc *circ;

	if (grp>=0)
	{
		circ=DRAW_CIRC(crc);
		g_ptr_array_remove_index(fbl, grp);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), -1);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), -1);
		{grp=-1; (circ->hgh)=-1;}
		/*also change plot*/
	}
}

void dit(GtkWidget *wgt, gpointer dta)
{
	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(wgt))) {gtk_widget_show(di2); gtk_widget_show(di3); gtk_widget_show(di4); gtk_widget_show(di5); gtk_widget_show(di6);}
	else {gtk_widget_hide(di2); gtk_widget_hide(di3); gtk_widget_hide(di4); gtk_widget_hide(di5); gtk_widget_hide(di6);}
}

void gmc(GtkWidget *wgt, gpointer dta)
{
	gint x;

	x=gtk_combo_box_get_active(GTK_COMBO_BOX(wgt));
}

void hlp(GtkWidget *wgt, gpointer dta)
{
	gchar *uri, *str;
	GError *Err=NULL;

	if (dta) uri=g_strdup_printf("ghelp:MOFsim-help?%s", (gchar *) dta);
	else uri=g_strdup("ghelp:MOFsim-help");
	gtk_show_uri(NULL, uri, gtk_get_current_event_time(), &Err);
	g_free(uri);
	if (Err)
	{
		str=g_strdup(_("Could not load help files."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
		g_error_free(Err);
	}
}

void mgt(GtkWidget *wgt, gpointer dta)
{
	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(wgt))) {gtk_widget_show(mg2); gtk_widget_show(mg3); gtk_widget_show(mg4); gtk_widget_show(mg5); gtk_widget_show(mg6);}
	else {gtk_widget_hide(mg2); gtk_widget_hide(mg3); gtk_widget_hide(mg4); gtk_widget_hide(mg5); gtk_widget_hide(mg6);}
}

void mtc(GtkWidget *wgt, gpointer dta)
{
	gint x;
	GtkTreeIter itr;
	GtkTreeModel* sto;

	x=gtk_combo_box_get_active(GTK_COMBO_BOX(wgt));
	if (!x)
	{
		sto=gtk_combo_box_get_model(GTK_COMBO_BOX(wgt));
		gtk_combo_box_get_active_iter(GTK_COMBO_BOX(wgt), &itr);
		gtk_list_store_append(GTK_LIST_STORE(sto), &itr);
		gtk_list_store_set(GTK_LIST_STORE(sto), &itr, 0, _("New Item"), -1);
		gtk_combo_box_set_model(GTK_COMBO_BOX(wgt), sto);
		g_object_unref(G_OBJECT(sto));
	}
}

