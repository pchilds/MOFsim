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
		{grp=-1; (circ->hlp)=-1; (circ->hlc)=-1;}
		draw_circ_redraw(crc);
	}
}

void azc(GtkWidget *wgt, gpointer dta)
{
	DrawCircData *dcd;
	GArray *ary;
	gdouble azi, az2, vcx, vcy, xce, yce;
	gint j;

	if ((grp>=0)&&(gem>0))
	{
		xce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
		yce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		dcd=(DrawCircData*) g_array_index(ary, DrawCircData*, 0);
		azi=-atan2((dcd->x)-xce, (dcd->y)-yce);
		azi+=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
		az2=sin(azi);
		azi=cos(azi);
		for (j=0;j<(ary->len);j++)
		{
			dcd=(DrawCircData*) g_array_index(ary, DrawCircData*, j);
			vcx=(dcd->x)-xce; vcy=(dcd->y)-yce;
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
	gdouble c1, c2, c3, s1, s2, s3;
	gint j, k;

	if (grp>=0)
	{
		if ((fbl->len)<=grp) g_ptr_array_remove_index((circ->data), grp);
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
				dcd=g_new(DrawCircData, 1);
				(dcd->r)=(lyt->rr);
				(dcd->x)=(c1*(lyt->ring))+(lyt->xc);
				(dcd->y)=(s1*(lyt->ring))+(lyt->yc);
				ary=g_array_sized_new(FALSE, FALSE, sizeof(DrawCircData*), 6*(lyt->ring));
				for (j=0; j<(lyt->ring); j++)
				{
					g_array_append_val(ary, dcd);
					{(dcd->x)-=c3; (dcd->y)-=s3;}
				}
				for (j=0; j<(lyt->ring); j++)
				{
					g_array_append_val(ary, dcd);
					{(dcd->x)-=c1; (dcd->y)-=s1;}
				}
				for (j=0; j<(lyt->ring); j++)
				{
					g_array_append_val(ary, dcd);
					{(dcd->x)-=c2; (dcd->y)-=s2;}
				}
				for (j=0; j<(lyt->ring); j++)
				{
					g_array_append_val(ary, dcd);
					{(dcd->x)+=c3; (dcd->y)+=s3;}
				}
				for (j=0; j<(lyt->ring); j++)
				{
					g_array_append_val(ary, dcd);
					{(dcd->x)+=c1; (dcd->y)+=s1;}
				}
				{g_array_append_val(ary, dcd); j=1;}
				while (j<(lyt->ring))
				{
					{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
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
				{(dcd->r)=(lyt->rr); (dcd->x)=(lyt->xc); (dcd->y)=(lyt->yc);}
				ary=g_array_sized_new(FALSE, FALSE, sizeof(DrawCircData*), 3*(lyt->ring)*((lyt->ring)+1));
				for (k=0; k<(lyt->ring); k++)
				{
					{(dcd->x)+=c1; (dcd->y)+=s1;}
					g_array_append_val(ary, dcd);
					for (j=0; j<=k; j++)
					{
						{(dcd->x)-=c3; (dcd->y)-=s3;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{(dcd->x)-=c1; (dcd->y)-=s1;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{(dcd->x)-=c2; (dcd->y)-=s2;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{(dcd->x)+=c3; (dcd->y)+=s3;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{(dcd->x)+=c1; (dcd->y)+=s1;}
						g_array_append_val(ary, dcd);
					}
					j=1;
					while (j<=k)
					{
						{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
						g_array_append_val(ary, dcd);
					}
					{(dcd->x)+=c2; (dcd->y)+=s2;}
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
			(dcg->xyr)=ary;
		}
	}
	draw_circ_redraw(crc);
	grp=(circ->hlc);
	g_signal_handler_block(G_OBJECT(cb1), cb1_id);
	g_signal_handler_block(G_OBJECT(cb2), cb2_id);
	if (grp>=0)
	{
		lyt=(FdtdLayout*) g_ptr_array_index(fbl, grp);
		gem=(lyt->geo);
		if (gem>0) if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb1))<=0) {gtk_widget_show(lb1); gtk_widget_show(lb2); gtk_widget_show(lb3); gtk_widget_show(pt); gtk_widget_show(rg); gtk_widget_show(az);}
		else if (gtk_combo_box_get_active(GTK_COMBO_BOX(cb1))>0) {gtk_widget_hide(lb1); gtk_widget_hide(lb2); gtk_widget_hide(lb3); gtk_widget_hide(pt); gtk_widget_hide(rg); gtk_widget_hide(az);}
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), gem);
		gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), (lyt->mat));
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
	gdouble c1, c2, c3, s1, s2, s3;
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
				g_array_remove_range(ary, 0, 3*(rin-1)*rin);
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
			dcd=g_new(DrawCircData, 1);
			(dcd->r)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			(dcd->x)=(c1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			(dcd->y)=(s1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c3; (dcd->y)-=s3;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c1; (dcd->y)-=s1;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c2; (dcd->y)-=s2;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)+=c3; (dcd->y)+=s3;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)+=c1; (dcd->y)+=s1;}
			}
			{g_array_append_val(ary, dcd); j=1;}
			while (j<rin)
			{
				{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
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
			dcd=g_new(DrawCircData, 1);
			(dcd->x)=(c1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			(dcd->y)=(s1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			(dcd->r)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c3; (dcd->y)-=s3;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c1; (dcd->y)-=s1;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c2; (dcd->y)-=s2;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)+=c3; (dcd->y)+=s3;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)+=c1; (dcd->y)+=s1;}
			}
			{g_array_append_val(ary, dcd); j=1;}
			while (j<rin)
			{
				{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
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
			dcd=g_new(DrawCircData, 1);
			(dcd->r)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			(dcd->x)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			(dcd->y)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			for (k=0; k<(rin-1); k++)
			{
				{(dcd->x)+=c1; (dcd->y)+=s1;}
				g_array_append_val(ary, dcd);
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c3; (dcd->y)-=s3;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c1; (dcd->y)-=s1;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c2; (dcd->y)-=s2;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)+=c3; (dcd->y)+=s3;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)+=c1; (dcd->y)+=s1;}
					g_array_append_val(ary, dcd);
				}
				j=1;
				while (j<=k)
				{
					{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
					g_array_append_val(ary, dcd);
				}
				{(dcd->x)+=c2; (dcd->y)+=s2;}
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
			dcd=g_new(DrawCircData, 1);
			(dcd->r)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			(dcd->x)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			(dcd->y)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			for (k=0; k<rin; k++)
			{
				{(dcd->x)+=c1; (dcd->y)+=s1;}
				g_array_append_val(ary, dcd);
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c3; (dcd->y)-=s3;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c1; (dcd->y)-=s1;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c2; (dcd->y)-=s2;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)+=c3; (dcd->y)+=s3;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)+=c1; (dcd->y)+=s1;}
					g_array_append_val(ary, dcd);
				}
				j=1;
				while (j<=k)
				{
					{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
					g_array_append_val(ary, dcd);
				}
				{(dcd->x)+=c2; (dcd->y)+=s2;}
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
			dcd=g_new(DrawCircData, 1);
			(dcd->r)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			(dcd->x)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			(dcd->y)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			ary=g_array_sized_new(FALSE, FALSE, sizeof(DrawCircData*), 3*rin*(rin+1));
			for (k=0; k<rin; k++)
			{
				{(dcd->x)+=c1; (dcd->y)+=s1;}
				g_array_append_val(ary, dcd);
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c3; (dcd->y)-=s3;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c1; (dcd->y)-=s1;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)-=c2; (dcd->y)-=s2;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)+=c3; (dcd->y)+=s3;}
					g_array_append_val(ary, dcd);
				}
				for (j=0; j<=k; j++)
				{
					{(dcd->x)+=c1; (dcd->y)+=s1;}
					g_array_append_val(ary, dcd);
				}
				j=1;
				while (j<=k)
				{
					{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
					g_array_append_val(ary, dcd);
				}
				{(dcd->x)+=c2; (dcd->y)+=s2;}
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
			ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
			(ary->len)=1;
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
	DrawCircGroup *dcg;
	FdtdMat *mat;
	gint x;
	GtkTreeIter itr;
	GtkTreeModel *sto;
	GtkWidget *ctt, *dlg, *ent, *tbl;

	x=gtk_combo_box_get_active(GTK_COMBO_BOX(wgt));
	if (!x)
	{
		dlg=gtk_dialog_new_with_buttons(_("Material Parameters"), GTK_WINDOW(wdw), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_APPLY, NULL);
		g_signal_connect_swapped(G_OBJECT(dlg), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(dlg));
		gtk_widget_show(dlg);
		ctt=gtk_dialog_get_content_area(GTK_DIALOG(dlg));
		ent=gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(ctt), ent, TRUE, TRUE, 2);
		gtk_widget_show(ent);
		tbl=gtk_table_new(2, 3, FALSE);
		gtk_widget_show(tbl);
		/*populate dialog with name and properties for material*/
		gtk_box_pack_start(GTK_BOX(ctt), tbl, TRUE, TRUE, 2);
		if (gtk_dialog_run(GTK_DIALOG(dlg))==GTK_RESPONSE_APPLY)
		{
			mat=g_new(FdtdMat, 1);
			g_ptr_array_add(mtr, (gpointer) mat);
			(mat->nme)=g_strdup(gtk_entry_get_text(GTK_ENTRY(ent)));
			/*populate mat*/
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
		xce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
		yce=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		dcd=(DrawCircData*) g_array_index(ary, DrawCircData*, 0);
		{pol=(dcd->x)-xce; pit=(dcd->y)-yce;}
		{pol*=pol; pit*=pit;}
		pol=sqrt(pol+pit);
		if (gem==2)
		{
			j=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
			pol/=j;
		}
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
	gdouble c1, c2, c3, rad, s1, s2, s3;
	gint j, k, rin;

	if ((grp>=0)&&(gem>0))
	{
		rin=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(rg));
		ary=(((DrawCircGroup*) g_ptr_array_index(((DRAW_CIRC(crc))->data), grp))->xyr);
		if (gem==1)
		{
			j=3*rin*(rin+1);
			if ((ary->len)>=j) (ary->len)=j;
			else
			{
				c2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pt));
				s2=MY_PI_180*gtk_spin_button_get_value(GTK_SPIN_BUTTON(az));
				{c1=c2*cos(s2); s1=c2*sin(s2);}
				{c2=s1*NRT3_2; s2=c1*RT3_2;}
				{c3=(c1/2)-c2; s3=(s1/2)-s2;}
				{c2+=(c1/2); s2+=(s1/2);}
				dcd=g_new(DrawCircData, 1);
				(dcd->r)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
				(dcd->x)=(c1*(ary->len))+gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
				(dcd->y)=(s1*(ary->len))+gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
				for (k=(gint) sqrt((ary->len)/3); k<rin; k++)
				{
					{(dcd->x)+=c1; (dcd->y)+=s1;}
					g_array_append_val(ary, dcd);
					for (j=0; j<=k; j++)
					{
						{(dcd->x)-=c3; (dcd->y)-=s3;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{(dcd->x)-=c1; (dcd->y)-=s1;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{(dcd->x)-=c2; (dcd->y)-=s2;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{(dcd->x)+=c3; (dcd->y)+=s3;}
						g_array_append_val(ary, dcd);
					}
					for (j=0; j<=k; j++)
					{
						{(dcd->x)+=c1; (dcd->y)+=s1;}
						g_array_append_val(ary, dcd);
					}
					j=1;
					while (j<=k)
					{
						{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
						g_array_append_val(ary, dcd);
					}
					{(dcd->x)+=c2; (dcd->y)+=s2;}
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
			dcd=g_new(DrawCircData, 1);
			(dcd->r)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(rd));
			(dcd->x)=(c1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(xc));
			(dcd->y)=(s1*rin)+gtk_spin_button_get_value(GTK_SPIN_BUTTON(yc));
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c3; (dcd->y)-=s3;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c1; (dcd->y)-=s1;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)-=c2; (dcd->y)-=s2;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)+=c3; (dcd->y)+=s3;}
			}
			for (j=0; j<rin; j++)
			{
				g_array_append_val(ary, dcd);
				{(dcd->x)+=c1; (dcd->y)+=s1;}
			}
			{g_array_append_val(ary, dcd); j=1;}
			while (j<rin)
			{
				{(dcd->x)+=c2; (dcd->y)+=s2; j++;}
				g_array_append_val(ary, dcd);
			}
		}
		draw_circ_redraw(crc);
	}
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
		for (j=0;j<(ary->len);j++) xce+=(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->x);
		xce/=j;
		for (j=0;j<(ary->len);j++)
		{
			ptr=&(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->x);
			(*ptr)+=xce;
		}
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
		for (j=0;j<(ary->len);j++) yce+=(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->y);
		yce/=j;
		for (j=0;j<(ary->len);j++)
		{
			ptr=&(((DrawCircData*) g_array_index(ary, DrawCircData*, j))->y);
			(*ptr)+=yce;
		}
		draw_circ_redraw(crc);
	}
}

