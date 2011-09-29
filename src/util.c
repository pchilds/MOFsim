/***************************************************************************
 *            util.c
 *
 *  Sat Dec  4 17:18:14 2010
 *  Copyright  2010  Paul Childs
 *  <pchilds@physics.org>
 ****************************************************************************/

/*
 * This program is free sof-tware; you can redistribute it and/or modify
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
#define MY_PI_180 0.0174532925199432957692369076848861271344287188854 /* pi/180 */
#define MY_180_PI 57.295779513082320876798154814105170332405472466564 /* inverse of this */

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
	FdtdLayout *lyt;

	if (grp>=0)
	{
		if ((fbl->len)>grp) lyt=(FdtdLayout*)g_ptr_array_index(fbl, grp);
		else
		{
			lyt=g_new(FdtdLayout, 1);
			g_ptr_array_add(fbl, (gpointer) lyt);
		}
		(lyt->geo)=gtk_combo_box_get_active(GTK_COMBO_BOX(cb1));
		if ((lyt->geo)>0) {gtk_widget_hide(lb1); gtk_widget_hide(lb2); gtk_widget_hide(lb3); gtk_widget_hide(pt); gtk_widget_hide(rg); gtk_widget_hide(az);}
		(lyt->mat)=gtk_combo_box_get_active(GTK_COMBO_BOX(cb2))-2;
		(lyt->ring)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
		(lyt->rr)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
		(lyt->xc)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
		(lyt->yc)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
		(lyt->pitch)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
		(lyt->azim)=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
		g_signal_handler_block(G_OBJECT(cb1), cb1_id);
		g_signal_handler_block(G_OBJECT(cb2), cb2_id);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), -1);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), -1);
		g_signal_handler_unblock(G_OBJECT(cb1), cb1_id);
		g_signal_handler_unblock(G_OBJECT(cb2), cb2_id);
		circ=DRAW_CIRC(crc);
		{grp=-1; (circ->hlp)=-1; (circ->hlc)=-1; gem=-1;}
		draw_circ_redraw(crc);
	}
}

void apr(GtkWidget *wgt, gpointer dta)
{
	AtkObject *atklbl, *atkwgt;
	GtkAdjustment *adj;
	GtkWidget *ctt, *dlg, *fc, *fw, *fz, *lbl, *tbl;

	dlg=gtk_dialog_new_with_buttons(_("Analysis Properties"), GTK_WINDOW(dta), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_APPLY, NULL);
	g_signal_connect_swapped(G_OBJECT(dlg), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(dlg));
	gtk_widget_show(dlg);
	ctt=gtk_dialog_get_content_area(GTK_DIALOG(dlg));
	tbl=gtk_table_new(4, 2, FALSE);
	gtk_widget_show(tbl);
	lbl=gtk_label_new(_("Resolution:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(rsn, 0, G_MAXINT8, 1.0, 5.0, 0.0);
	fc=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), fc, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(fc);
	atkwgt=gtk_widget_get_accessible(fc);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lbl=gtk_label_new(_("Frequency\nWidth:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(fwd, 0, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	fw=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), fw, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(fw);
	atkwgt=gtk_widget_get_accessible(fw);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lbl=gtk_label_new(_("Source\nWidth:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(fsz, 0, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	fz=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), fz, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(fz);
	atkwgt=gtk_widget_get_accessible(fz);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	gtk_box_pack_start(GTK_BOX(ctt), tbl, TRUE, TRUE, 2);
	if (gtk_dialog_run(GTK_DIALOG(dlg))==GTK_RESPONSE_APPLY)
	{
		rsn=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(fc));
		fwd=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fw));
		fsz=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fz));
	}
	gtk_widget_destroy(dlg);
}

void azc(GtkWidget *wgt, gpointer dta)
{
	DrawCircData *dcd;
	GArray *ary;
	gdouble azi, az2, vcx, vcy, xce, yce;
	gint j;

	if ((grp>=0)&&(gem>0))
	{
		xce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
		yce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		dcd=(DrawCircData*) g_array_index(ary, DrawCircData*, 0);
		azi=-atan2((dcd->y)-yce, (dcd->x)-xce);
		azi+=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
		az2=sin(azi);
		azi=cos(azi);
		for (j=0;j<(ary->len);j++)
		{
			dcd=(DrawCircData*) g_array_index(ary, DrawCircData*, j);
			{vcx=(dcd->x)-xce; vcy=(dcd->y)-yce;}
			(dcd->x)=(vcx*azi)-(vcy*az2)+xce;
			(dcd->y)=(vcx*az2)+(vcy*azi)+yce;
		}
		draw_circ_redraw(crc);
	}
}

void chl(DrawCirc *circ, gpointer data)
{
	DrawCircData *dcd;
	DrawCircGroup *dcg;
	FdtdLayout *lyt;
	GArray *ary;
	gdouble c1, c2, c3, s1, s2, s3, x0, y0;
	gint j, k;

	if (grp>=0)
	{
		if ((fbl->len)<=grp) g_ptr_array_remove_index((circ->data), grp);/*free data*/
		else
		{
			lyt=(FdtdLayout*) g_ptr_array_index(fbl, grp);
			dcg=(DrawCircGroup*)g_ptr_array_index((circ->data), grp);
			(dcg->col)=(lyt->mat);
			if ((lyt->geo)==2)
			{
				{c1=(lyt->pitch)*cos(lyt->azim); s1=(lyt->pitch)*sin(lyt->azim);}
				{c2=s1*NRT3_2; s2=c1*RT3_2;}
				{c3=(c1/2)-c2; s3=(s1/2)-s2;}
				{c2+=(c1/2); s2+=(s1/2);}
				ary=g_array_sized_new(FALSE, FALSE, sizeof(DrawCircData*), 6*(lyt->ring));
				x0=(c1*(lyt->ring))+(lyt->xc);
				y0=(s1*(lyt->ring))+(lyt->yc);
				for (j=0; j<(lyt->ring); j++)
				{
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
					{x0-=c3; y0-=s3;}
				}
				for (j=0; j<(lyt->ring); j++)
				{
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
					{x0-=c1; y0-=s1;}
				}
				for (j=0; j<(lyt->ring); j++)
				{
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
					{x0-=c2; y0-=s2;}
				}
				for (j=0; j<(lyt->ring); j++)
				{
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
					{x0+=c3; y0+=s3;}
				}
				for (j=0; j<(lyt->ring); j++)
				{
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
					{x0+=c1; y0+=s1;}
				}
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0; j=1;}
				g_array_append_val(ary, dcd);
				while (j<(lyt->ring))
				{
					{x0+=c2; y0+=s2; j++;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
			}
			else if ((lyt->geo)==1)
			{
				{c1=(lyt->pitch)*cos(lyt->azim); s1=(lyt->pitch)*sin(lyt->azim);}
				{c2=s1*NRT3_2; s2=c1*RT3_2;}
				{c3=(c1/2)-c2; s3=(s1/2)-s2;}
				{c2+=(c1/2); s2+=(s1/2);}
				dcd=g_new(DrawCircData, 1);
				{x0=(lyt->xc); y0=(lyt->yc);}
				ary=g_array_sized_new(FALSE, FALSE, sizeof(DrawCircData*), 3*(lyt->ring)*((lyt->ring)+1));
				for (k=0; k<(lyt->ring); k++)
				{
					{x0+=c1; y0+=s1;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
					for (j=0; j<=k; j++)
					{
						{x0-=c3; y0-=s3;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{x0-=c1; y0-=s1;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{x0-=c2; y0-=s2;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{x0+=c3; y0+=s3;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{x0+=c1; y0+=s1;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					j=1;
					while (j<=k)
					{
						{x0+=c2; y0+=s2; j++;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=(lyt->rr); (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					{x0+=c2; y0+=s2;}
				}
			}
			else if ((lyt->geo)==0)
			{
				ary=g_array_new(FALSE, FALSE, sizeof(DrawCircData*));
				dcd=g_new(DrawCircData, 1);
				{(dcd->x)=(lyt->xc); (dcd->y)=(lyt->yc); (dcd->r)=(lyt->rr);}
				g_array_append_val(ary, dcd);
			}
			else ary=g_array_new(FALSE, FALSE, sizeof(DrawCircData*));
			(dcg->xyr)=ary;/*free old array*/
		}
	}
	draw_circ_redraw(crc);
	grp=(circ->hlc);
	g_signal_handler_block(G_OBJECT(cb1), cb1_id);
	g_signal_handler_block(G_OBJECT(cb2), cb2_id);
	if (grp>=0)
	{
		lyt=(FdtdLayout*) g_ptr_array_index(fbl, grp);
		gem=(lyt->geo);/*segfaults somehow likely bug elsewhere (zoom related)*/
		if (gem>0) if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb1))<=0) {gtk_widget_show(lb1); gtk_widget_show(lb2); gtk_widget_show(lb3); gtk_widget_show(pt); gtk_widget_show(rg); gtk_widget_show(az);}
		else if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb1))>0) {gtk_widget_hide(lb1); gtk_widget_hide(lb2); gtk_widget_hide(lb3); gtk_widget_hide(pt); gtk_widget_hide(rg); gtk_widget_hide(az);}
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), gem);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), (lyt->mat)+2);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(xc), (lyt->xc));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(yc), (lyt->yc));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(rd), (lyt->rr));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(pt), (lyt->pitch));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(az), MY_180_PI*(lyt->azim));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(rg), (lyt->ring));
	}
	else
	{
		if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb1))>0) {gtk_widget_hide(lb1); gtk_widget_hide(lb2); gtk_widget_hide(lb3); gtk_widget_hide(pt); gtk_widget_hide(rg); gtk_widget_hide(az);}
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), -1);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), -1);
		gem=-1;
	}
	g_signal_handler_unblock(G_OBJECT(cb1), cb1_id);
	g_signal_handler_unblock(G_OBJECT(cb2), cb2_id);
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
		if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb1))>0) {gtk_widget_hide(lb1); gtk_widget_hide(lb2); gtk_widget_hide(lb3); gtk_widget_hide(pt); gtk_widget_hide(rg); gtk_widget_hide(az);}
		g_signal_handler_block(G_OBJECT(cb1), cb1_id);
		g_signal_handler_block(G_OBJECT(cb2), cb2_id);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), -1);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), -1);
		g_signal_handler_unblock(G_OBJECT(cb1), cb1_id);
		g_signal_handler_unblock(G_OBJECT(cb2), cb2_id);
		if ((fbl->len)>grp) g_ptr_array_remove_index(fbl, grp);
		circ=DRAW_CIRC(crc);
		g_ptr_array_remove_index((circ->data), grp);
		{grp=-1; (circ->hlp)=-1; (circ->hlc)=-1; gem=-1;}
		draw_circ_redraw(crc);
	}
}

void dit(GtkWidget *wgt, gpointer dta)
{
	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(wgt))) {gtk_widget_show(di2); gtk_widget_show(di3); gtk_widget_show(di4); gtk_widget_show(di5); gtk_widget_show(di6);}
	else {gtk_widget_hide(di2); gtk_widget_hide(di3); gtk_widget_hide(di4); gtk_widget_hide(di5); gtk_widget_hide(di6);}
}

void gmc(GtkWidget *wgt, gpointer dta)
{
	DrawCircData *dcd;
	DrawCircData **ptr;
	DrawCircGroup *dcg;
	GArray *ary;
	gdouble c1, c2, c3, r0, s1, s2, s3, x0, y0;
	gint j, k, rin, x;

	x=gtk_combo_box_get_active(GTK_COMBO_BOX(cb1));
	if (x==2)
	{
		if (gem==1)
		{
			rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			if (rin>1)
			{
				ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
				g_array_remove_range(ary, 0, 3*(rin-1)*rin);/*TODO: free data*/
				draw_circ_redraw(crc);
			}
			gem=2;
		}
		else if (gem==0)
		{
			c2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
			s2=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
			{c1=c2*cos(s2); s1=c2*sin(s2);}
			{c2=s1*NRT3_2; s2=c1*RT3_2;}
			{c3=(c1/2)-c2; s3=(s1/2)-s2;}
			{c2+=(c1/2); s2+=(s1/2);}
			rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
			(ary->len)=0;
			r0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			x0=(c1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			y0=(s1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c3; y0-=s3;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c1; y0-=s1;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c2; y0-=s2;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0+=c3; y0+=s3;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0+=c1; y0+=s1;}
			}
			dcd=g_new(DrawCircData, 1);
			{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
			{g_array_append_val(ary, dcd); j=1;}
			while (j<rin)
			{
				{x0+=c2; y0+=s2; j++;}
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
			}
			{gtk_widget_show(lb1); gtk_widget_show(lb2); gtk_widget_show(lb3); gtk_widget_show(pt); gtk_widget_show(rg); gtk_widget_show(az);}
			gem=2;
			draw_circ_redraw(crc);
		}
		else if (gem<0)
		{
			if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb2))<0) gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), 1);
			c2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
			s2=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
			{c1=c2*cos(s2); s1=c2*sin(s2);}
			{c2=s1*NRT3_2; s2=c1*RT3_2;}
			{c3=(c1/2)-c2; s3=(s1/2)-s2;}
			{c2+=(c1/2); s2+=(s1/2);}
			rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			ary=g_array_sized_new(FALSE, FALSE, sizeof(DrawCircData*), 6*rin);
			x0=(c1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			y0=(s1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			r0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c3; y0-=s3;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c1; y0-=s1;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c2; y0-=s2;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0+=c3; y0+=s3;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0+=c1; y0+=s1;}
			}
			dcd=g_new(DrawCircData, 1);
			{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0; j=1;}
			g_array_append_val(ary, dcd);
			while (j<rin)
			{
				{x0+=c2; y0+=s2; j++;}
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
			}
			dcg=g_new(DrawCircGroup, 1);
			{(dcg->col)=gtk_combo_box_get_active(GTK_COMBO_BOX(cb2))-2; (dcg->xyr)=ary;}
			g_ptr_array_add(((DRAW_CIRC(crc))->data), (gpointer) dcg);
			{gtk_widget_show(lb1); gtk_widget_show(lb2); gtk_widget_show(lb3); gtk_widget_show(pt); gtk_widget_show(rg); gtk_widget_show(az);}
			{gem=2; grp=(((DRAW_CIRC(crc))->data)->len)-1;}
			draw_circ_redraw(crc);
		}
	}
	else if (x>0)
	{
		if (gem==2)
		{
			c2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
			s2=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
			{c1=c2*cos(s2); s1=c2*sin(s2);}
			{c2=s1*NRT3_2; s2=c1*RT3_2;}
			{c3=(c1/2)-c2; s3=(s1/2)-s2;}
			{c2+=(c1/2); s2+=(s1/2);}
			rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			ary=g_array_sized_new(FALSE, FALSE, sizeof(DrawCircData*), 3*rin*(rin+1));
			r0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			x0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			y0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			for (k=0; k<(rin-1); k++)
			{
				{x0+=c1; y0+=s1;}
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				for (j=0; j<=k; j++)
				{
					{x0-=c3; y0-=s3;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0-=c1; y0-=s1;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0-=c2; y0-=s2;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0+=c3; y0+=s3;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0+=c1; y0+=s1;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				j=1;
				while (j<=k)
				{
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					{x0+=c2; y0+=s2; j++;}
					g_array_append_val(ary, dcd);
				}
				{x0+=c2; y0+=s2;}
			}
			dcg=(DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp);
			for (j=0;j<((dcg->xyr)->len);j++)
			{
				dcd=g_array_index((dcg->xyr), DrawCircData*, j);
				g_array_append_val(ary, dcd);
			}
			g_array_free((dcg->xyr), FALSE);
			(dcg->xyr)=ary;
			gem=1;
			draw_circ_redraw(crc);
		}
		else if (gem==0)
		{
			c2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
			s2=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
			{c1=c2*cos(s2); s1=c2*sin(s2);}
			{c2=s1*NRT3_2; s2=c1*RT3_2;}
			{c3=(c1/2)-c2; s3=(s1/2)-s2;}
			{c2+=(c1/2); s2+=(s1/2);}
			ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);/*segfault when grp=-1*/
			(ary->len)=0;
			r0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			x0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			y0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			for (k=0; k<rin; k++)
			{
				{x0+=c1; y0+=s1;}
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				for (j=0; j<=k; j++)
				{
					{x0-=c3; y0-=s3;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0-=c1; y0-=s1;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0-=c2; y0-=s2;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0+=c3; y0+=s3;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0+=c1; y0+=s1;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				j=1;
				while (j<=k)
				{
					{x0+=c2; y0+=s2; j++;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				{x0+=c2; y0+=s2;}
			}
			{gtk_widget_show(lb1); gtk_widget_show(lb2); gtk_widget_show(lb3); gtk_widget_show(pt); gtk_widget_show(rg); gtk_widget_show(az);}
			gem=1;
			draw_circ_redraw(crc);
		}
		else if (gem<0)
		{
			if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb2))<0) gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), 1);
			c2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
			s2=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
			{c1=c2*cos(s2); s1=c2*sin(s2);}
			{c2=s1*NRT3_2; s2=c1*RT3_2;}
			{c3=(c1/2)-c2; s3=(s1/2)-s2;}
			{c2+=(c1/2); s2+=(s1/2);}
			r0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			x0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			y0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			ary=g_array_sized_new(FALSE, FALSE, sizeof(DrawCircData*), 3*rin*(rin+1));
			for (k=0; k<rin; k++)
			{
				{x0+=c1; y0+=s1;}
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				for (j=0; j<=k; j++)
				{
					{x0-=c3; y0-=s3;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0-=c1; y0-=s1;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0-=c2; y0-=s2;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0+=c3; y0+=s3;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{x0+=c1; y0+=s1;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				j=1;
				while (j<=k)
				{
					{x0+=c2; y0+=s2; j++;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
				}
				{x0+=c2; y0+=s2;}
			}
			dcg=g_new(DrawCircGroup, 1);
			{(dcg->col)=gtk_combo_box_get_active(GTK_COMBO_BOX(cb2))-2; (dcg->xyr)=ary;}
			g_ptr_array_add(((DRAW_CIRC(crc))->data), (gpointer) dcg);
			{gtk_widget_show(lb1); gtk_widget_show(lb2); gtk_widget_show(lb3); gtk_widget_show(pt); gtk_widget_show(rg); gtk_widget_show(az);}
			{gem=1; grp=(((DRAW_CIRC(crc))->data)->len)-1;}
			draw_circ_redraw(crc);
		}
	}
	else if (x==0)
	{
		if (gem>0)
		{
			ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);/*segfault for grp<0*/
			(ary->len)=1;/*TODO: free array contents*/
			ptr=&g_array_index(ary, DrawCircData*, 0);
			((*ptr)->x)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			((*ptr)->y)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			{gtk_widget_hide(lb1); gtk_widget_hide(lb2); gtk_widget_hide(lb3); gtk_widget_hide(pt); gtk_widget_hide(rg); gtk_widget_hide(az);}
			 gem=0;
			draw_circ_redraw(crc);
		}
		else if (gem<0)
		{
			if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb2))<0) gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), 1);
			dcd=g_new(DrawCircData, 1);
			(dcd->x)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			(dcd->y)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			(dcd->r)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			ary=g_array_new(FALSE, FALSE, sizeof(DrawCircData*));
			g_array_append_val(ary, dcd);
			dcg=g_new(DrawCircGroup, 1);
			{(dcg->col)=gtk_combo_box_get_active(GTK_COMBO_BOX(cb2))-2; (dcg->xyr)=ary;}
			g_ptr_array_add(((DRAW_CIRC(crc))->data), (gpointer) dcg);
			{gem=0; grp=(((DRAW_CIRC(crc))->data)->len)-1;}
			draw_circ_redraw(crc);
		}
	}
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
	AtkObject *atklbl, *atkwgt;
	DrawCircGroup *dcg;
	FdtdMat *mat;
	gint x;
	GtkAdjustment *adj;
	GtkTreeIter itr;
	GtkTreeModel *sto;
	GtkWidget *ctt, *d11, *d12, *d13, *d22, *d23, *d33, *dlg, *dpi, *dpr, *dsg, *dx2, *dx3, *ent, *lbl, *m11, *m12, *m13, *m21, *m22, *m23, *m31, *m32, *m33, *mpi, *mpr, *msg, *mx2, *mx3, *tbl;

	x=gtk_combo_box_get_active(GTK_COMBO_BOX(wgt));
	if (!x)
	{
		dlg=gtk_dialog_new_with_buttons(_("Material Parameters"), GTK_WINDOW(dta), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_APPLY, NULL);
		g_signal_connect_swapped(G_OBJECT(dlg), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(dlg));
		gtk_widget_show(dlg);
		ctt=gtk_dialog_get_content_area(GTK_DIALOG(dlg));
		ent=gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(ctt), ent, TRUE, TRUE, 2);
		gtk_widget_show(ent);
		gtk_widget_grab_focus(ent);
		tbl=gtk_table_new(12, 6, FALSE);
		gtk_widget_show(tbl);
		lbl=gtk_label_new(_("Permittivity:"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 3, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(1.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		d11=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), d11, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(1.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		d22=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), d22, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(1.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		d33=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), d33, 2, 3, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		d12=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), d12, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		d23=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), d23, 2, 3, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		d13=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), d13, 2, 3, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di1))) {gtk_widget_show(lbl); gtk_widget_show(d11);}
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di2))) {gtk_widget_show(d22); gtk_widget_show(d33); gtk_widget_show(d12); gtk_widget_show(d23); gtk_widget_show(d13);}
		lbl=gtk_label_new(_("Dielectric Propagation\n(Dispersion, Loss, Gain):"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 3, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		dpr=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), dpr, 0, 1, 5, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		dpi=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), dpi, 1, 3, 5, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di3))) {gtk_widget_show(lbl); gtk_widget_show(dpr); gtk_widget_show(dpi);}
		lbl=gtk_label_new(_("Electric\nConductivity:"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 3, 6, 7, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		dsg=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), dsg, 0, 3, 7, 8, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di4))) {gtk_widget_show(lbl); gtk_widget_show(dsg);}
		atkwgt=gtk_widget_get_accessible(dsg);
		atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
		atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
		atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
		lbl=gtk_label_new(_("Dielectric χ2:"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 3, 8, 9, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		dx2=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), dx2, 0, 3, 9, 10, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di5))) {gtk_widget_show(lbl); gtk_widget_show(dx2);}
		atkwgt=gtk_widget_get_accessible(dx2);
		atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
		atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
		atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
		lbl=gtk_label_new(_("Dielectric χ3:"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 3, 10, 11, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		dx3=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), dx3, 0, 3, 11, 12, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(di6))) {gtk_widget_show(lbl); gtk_widget_show(dx3);}
		atkwgt=gtk_widget_get_accessible(dx3);
		atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
		atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
		atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
		lbl=gtk_label_new(_("Magnetic\nPermeability:"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 3, 6, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(1.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		m11=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), m11, 3, 4, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(1.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		m22=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), m22, 4, 5, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(1.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		m33=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), m33, 5, 6, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		m12=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), m12, 4, 5, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		m23=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), m23, 5, 6, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		m13=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), m13, 5, 6, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg1))) {gtk_widget_show(lbl); gtk_widget_show(m11);}
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg2))) {gtk_widget_show(m22); gtk_widget_show(m33); gtk_widget_show(m12); gtk_widget_show(m23); gtk_widget_show(m13);}
		lbl=gtk_label_new(_("Magnetic Propagation\n(Dispersion, Loss, Gain):"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 3, 6, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		mpr=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), mpr, 3, 4, 5, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		mpi=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), mpi, 4, 6, 5, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg3))) {gtk_widget_show(lbl); gtk_widget_show(mpr); gtk_widget_show(mpi);}
		lbl=gtk_label_new(_("Magnetic\nConductivity:"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 3, 6, 6, 7, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		msg=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), msg, 3, 6, 7, 8, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg4))) {gtk_widget_show(lbl); gtk_widget_show(msg);}
		atkwgt=gtk_widget_get_accessible(msg);
		atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
		atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
		atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
		lbl=gtk_label_new(_("Magnetic χ2:"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 3, 6, 8, 9, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		mx2=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), mx2, 3, 6, 9, 10, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg5))) {gtk_widget_show(lbl); gtk_widget_show(mx2);}
		atkwgt=gtk_widget_get_accessible(mx2);
		atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
		atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
		atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
		lbl=gtk_label_new(_("Magnetic χ3:"));
		gtk_table_attach(GTK_TABLE(tbl), lbl, 3, 6, 10, 11, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
		mx3=gtk_spin_button_new(adj, 0.5, 3);
		gtk_table_attach(GTK_TABLE(tbl), mx3, 3, 6, 11, 12, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(mg6))) {gtk_widget_show(lbl); gtk_widget_show(mx3);}
		atkwgt=gtk_widget_get_accessible(mx3);
		atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
		atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
		atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
		gtk_box_pack_start(GTK_BOX(ctt), tbl, TRUE, TRUE, 2);
		if (gtk_dialog_run(GTK_DIALOG(dlg))==GTK_RESPONSE_APPLY)
		{
			mat=g_new(FdtdMat, 1);
			g_ptr_array_add(mtr, (gpointer) mat);
			(mat->nme)=g_strdup(gtk_entry_get_text(GTK_ENTRY(ent)));
			(mat->d11)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(d11));
			(mat->d22)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(d22));
			(mat->d33)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(d33));
			(mat->d12)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(d12));
			(mat->d23)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(d23));
			(mat->d13)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(d13));
			(mat->dpr)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(dpr));
			(mat->dpi)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(dpi));
			(mat->dsg)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(dsg));
			(mat->dx2)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(dx2));
			(mat->dx3)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(dx3));
			(mat->m11)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(m11));
			(mat->m22)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(m22));
			(mat->m33)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(m33));
			(mat->m12)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(m12));
			(mat->m23)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(m23));
			(mat->m13)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(m13));
			(mat->mpr)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(mpr));
			(mat->mpi)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(mpi));
			(mat->msg)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(msg));
			(mat->mx2)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(mx2));
			(mat->mx3)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(mx3));
			sto=gtk_combo_box_get_model(GTK_COMBO_BOX(wgt));
			gtk_combo_box_get_active_iter(GTK_COMBO_BOX(wgt), &itr);
			gtk_list_store_append(GTK_LIST_STORE(sto), &itr);
			gtk_list_store_set(GTK_LIST_STORE(sto), &itr, 0, (mat->nme), -1);
			gtk_combo_box_set_model(GTK_COMBO_BOX(wgt), sto);
			if (grp>=0)
			{
				dcg=(DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp);
				(dcg->col)=gtk_tree_model_iter_n_children(sto, NULL)-3;
				gtk_combo_box_set_active(GTK_COMBO_BOX(wgt), (dcg->col)+2);
				draw_circ_redraw(crc);
			}
			g_object_unref(G_OBJECT(sto));
		}
		gtk_widget_destroy(dlg);
	}
	else if (grp>=0)
	{
		dcg=(DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp);
		(dcg->col)=x-2;
		draw_circ_redraw(crc);
	}
}

void ptc(GtkWidget *wgt, gpointer dta)
{
	DrawCircData *dcd;
	GArray *ary;
	gdouble pit, pol, xce, yce;
	gint j;

	if ((grp>=0)&&(gem>0))
	{
		xce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
		yce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		dcd=(DrawCircData*) g_array_index(ary, DrawCircData*, 0);
		{pol=(dcd->x)-xce; pit=(dcd->y)-yce;}
		{pol*=pol; pit*=pit;}
		pol=sqrt(pol+pit);
		if (gem==2) pol/=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
		pit=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt))/pol;
		for (j=0;j<(ary->len);j++)
		{
			dcd=(DrawCircData*) g_array_index(ary, DrawCircData*, j);
			pol=pit*((dcd->x)-xce);
			(dcd->x)=pol+xce;
			pol=pit*((dcd->y)-yce);
			(dcd->y)=pol+yce;
		}
		draw_circ_redraw(crc);
	}
}

void rdc(GtkWidget *wgt, gpointer dta)
{
	GArray *ary;
	gdouble rad;
	gdouble *ptr;
	gint j;

	if (grp>=0)
	{
		rad=gtk_spin_button_get_value(GTK_SPIN_BUTTON(wgt));
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		for (j=0;j<(ary->len);j++)
		{
			ptr=&(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->r);
			*ptr=rad;
		}
		draw_circ_redraw(crc);
	}
}

void rgc(GtkWidget *wgt, gpointer dta)
{
	DrawCircData *dcd;
	GArray *ary;
	gdouble c1, c2, c3, r0, rad, s1, s2, s3, x0, y0;
	gint j, k, rin;

	if ((grp>=0)&&(gem>0))
	{
		rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		if (gem==1)
		{
			j=3*rin*(rin+1);
			if ((ary->len)>=j) (ary->len)=j;/*TODO: free data*/
			else
			{
				c2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
				s2=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
				{c1=c2*cos(s2); s1=c2*sin(s2);}
				{c2=s1*NRT3_2; s2=c1*RT3_2;}
				{c3=(c1/2)-c2; s3=(s1/2)-s2;}
				{c2+=(c1/2); s2+=(s1/2);}
				r0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
				j=(gint) sqrt((ary->len)/3);
				x0=(c1*j)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
				y0=(s1*j)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
				for (k=j; k<rin; k++)
				{
					{x0+=c1; y0+=s1;}
					dcd=g_new(DrawCircData, 1);
					{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
					g_array_append_val(ary, dcd);
					for (j=0; j<=k; j++)
					{
						{x0-=c3; y0-=s3;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{x0-=c1; y0-=s1;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{x0-=c2; y0-=s2;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{x0+=c3; y0+=s3;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{x0+=c1; y0+=s1;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					j=1;
					while (j<=k)
					{
						{x0+=c2; y0+=s2; j++;}
						dcd=g_new(DrawCircData, 1);
						{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
						g_array_append_val(ary, dcd);
					}
					{x0+=c2; y0+=s2;}
				}
			}
		}
		else if ((ary->len)!=6*rin)
		{
			(ary->len)=0;
			c2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
			s2=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
			{c1=c2*cos(s2); s1=c2*sin(s2);}
			{c2=s1*NRT3_2; s2=c1*RT3_2;}
			{c3=(c1/2)-c2; s3=(s1/2)-s2;}
			{c2+=(c1/2); s2+=(s1/2);}
			r0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			x0=(c1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			y0=(s1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c3; y0-=s3;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c1; y0-=s1;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0-=c2; y0-=s2;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0+=c3; y0+=s3;}
			}
			for (j=0; j<rin; j++)
			{
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
				{x0+=c1; y0+=s1;}
			}
			dcd=g_new(DrawCircData, 1);
			{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0; j=1;}
			g_array_append_val(ary, dcd);
			while (j<rin)
			{
				{x0+=c2; y0+=s2; j++;}
				dcd=g_new(DrawCircData, 1);
				{(dcd->r)=r0; (dcd->x)=x0; (dcd->y)=y0;}
				g_array_append_val(ary, dcd);
			}
		}
		draw_circ_redraw(crc);
	}
}

void src(GtkWidget *wgt, gpointer dta)
{
	AtkObject *atklbl, *atkwgt;
	GtkAdjustment *adj;
	GtkWidget *ctt, *dlg, *fc, *fw, *fz, *lbl, *tbl;

	dlg=gtk_dialog_new_with_buttons(_("Source Parameters"), GTK_WINDOW(dta), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_APPLY, NULL);
	g_signal_connect_swapped(G_OBJECT(dlg), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(dlg));
	gtk_widget_show(dlg);
	ctt=gtk_dialog_get_content_area(GTK_DIALOG(dlg));
	tbl=gtk_table_new(4, 2, FALSE);
	gtk_widget_show(tbl);
	lbl=gtk_label_new(_("Centre\nFrequency:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(fcn, 0, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	fc=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), fc, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(fc);
	atkwgt=gtk_widget_get_accessible(fc);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lbl=gtk_label_new(_("Frequency\nWidth:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(fwd, 0, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	fw=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), fw, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(fw);
	atkwgt=gtk_widget_get_accessible(fw);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lbl=gtk_label_new(_("Source\nWidth:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(fsz, 0, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	fz=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), fz, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(fz);
	atkwgt=gtk_widget_get_accessible(fz);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	gtk_box_pack_start(GTK_BOX(ctt), tbl, TRUE, TRUE, 2);
	if (gtk_dialog_run(GTK_DIALOG(dlg))==GTK_RESPONSE_APPLY)
	{
		fcn=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fc));
		fwd=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fw));
		fsz=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fz));
	}
	gtk_widget_destroy(dlg);
}

void xcc(GtkWidget *wgt, gpointer dta)
{
	GArray *ary;
	gdouble xce;
	gdouble *ptr;
	gint j;

	if (grp>=0)
	{
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		xce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc))*(ary->len);
		for (j=0;j<(ary->len);j++) xce-=(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->x);
		xce/=j;
		for (j=0;j<(ary->len);j++)
		{
			ptr=&(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->x);
			(*ptr)+=xce;
		}
		draw_circ_redraw(crc);
	}
}

void ycc(GtkWidget *wgt, gpointer dta)
{
	GArray *ary;
	gdouble yce;
	gdouble *ptr;
	gint j;

	if (grp>=0)
	{
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		yce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc))*(ary->len);
		for (j=0;j<(ary->len);j++) yce-=(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->y);
		yce/=j;
		for (j=0;j<(ary->len);j++)
		{
			ptr=&(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->y);
			(*ptr)+=yce;
		}
		draw_circ_redraw(crc);
	}
}

