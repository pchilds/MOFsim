/***************************************************************************
 *            disp.c
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

#include "disp.h"

GdkColormap *cmp;
GtkWidget *hwn, *cls, *jix;
guint alp=65535;

void dpa(GtkWidget *wgt, gpointer dta)
{
	DrawCirc *circ;
	GdkColor clr;
	gdouble *ptr;
	gdouble iv;
	gint dx;

	dx=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(wgt));
	circ=DRAW_CIRC(crc);
	dx=(((DrawCircGroup*) g_ptr_array_index((circ->data), dx))->col)+2;
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(cls), &clr);
	alp=gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(cls));
	ptr=&g_array_index((circ->rd), gdouble, dx);
	iv=((gdouble) (clr.red))/65535;
	*ptr=iv;
	ptr=&g_array_index((circ->gr), gdouble, dx);
	iv=((gdouble) (clr.green))/65535;
	*ptr=iv;
	ptr=&g_array_index((circ->bl), gdouble, dx);
	iv=((gdouble) (clr.blue))/65535;
	*ptr=iv;
	ptr=&g_array_index((circ->al), gdouble, dx);
	iv=((gdouble) alp)/65535;
	*ptr=iv;
}

void dpo(GtkWidget *wgt, gpointer dta)
{
	dpa(NULL, NULL);
	gtk_widget_destroy(hwn);
}

void upj(GtkWidget *wgt, gpointer dta)
{
	DrawCirc *circ;
	gchar *str;
	GdkColor clr;
	gint dx;

	dx=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(wgt));
	circ=DRAW_CIRC(crc);
	dx=(((DrawCircGroup*) g_ptr_array_index((circ->data), dx))->col)+2;
	(clr.red)=(guint16) (65535*g_array_index((circ->rd), gdouble, dx));
	(clr.green)=(guint16) (65535*g_array_index((circ->gr), gdouble, dx));
	(clr.blue)=(guint16) (65535*g_array_index((circ->bl), gdouble, dx));
	alp=(guint16) (65535*g_array_index((circ->al), gdouble, dx));
	str=g_strdup(((FdtdMat*) g_ptr_array_index(mtr, dx))->nme);/*change label*/
	gdk_colormap_alloc_color(cmp, &clr, FALSE, TRUE);
	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(cls), &clr);
	gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(cls), alp);
}

void dpr(GtkWidget *wgt, gpointer dta)
{
	DrawCirc *circ;
	GtkWidget *btt, *vpn, *hbx, *lbl, *vbx;
	GtkAdjustment *adj;
	AtkObject *atkwgt, *atklbl;
	GdkColor clr;
	gchar *str;
	gint dx;

	if (grp>=0)
	{
		circ=DRAW_CIRC(crc);
		dx=(((DrawCircGroup*) g_ptr_array_index((circ->data), grp))->col);
		if (dx>=0)
		{
			hwn=gtk_dialog_new_with_buttons(_("Display Properties"), GTK_WINDOW(dta), GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
			g_signal_connect_swapped(G_OBJECT(hwn), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(hwn));
			btt=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
			gtk_widget_show(btt);
			g_signal_connect_swapped(G_OBJECT(btt), "clicked", G_CALLBACK(gtk_widget_destroy), G_OBJECT(hwn));
			gtk_container_add(GTK_CONTAINER(GTK_DIALOG(hwn)->action_area), btt);
			btt=gtk_button_new_from_stock(GTK_STOCK_APPLY);
			gtk_widget_show(btt);
			g_signal_connect(G_OBJECT(btt), "clicked", G_CALLBACK(dpa), NULL);
			gtk_container_add(GTK_CONTAINER(GTK_DIALOG(hwn)->action_area), btt);
			btt=gtk_button_new_from_stock(GTK_STOCK_OK);
			gtk_widget_show(btt);
			g_signal_connect(G_OBJECT(btt), "clicked", G_CALLBACK(dpo), NULL);
			gtk_container_add(GTK_CONTAINER(GTK_DIALOG(hwn)->action_area), btt);
			vpn=gtk_vpaned_new();
			gtk_widget_show(vpn);
			gtk_container_add(GTK_CONTAINER(GTK_DIALOG(hwn)->vbox), vpn);
			cmp=gdk_colormap_get_system();
			cls=gtk_color_selection_new();
			gtk_widget_show(cls);
			gtk_paned_add1(GTK_PANED(vpn), cls);
			gtk_color_selection_set_has_opacity_control(GTK_COLOR_SELECTION(cls), TRUE);
			gtk_color_selection_set_has_palette(GTK_COLOR_SELECTION(cls), TRUE);
			(clr.red)=(guint16) (65535*g_array_index((circ->rd), gdouble, dx));
			(clr.green)=(guint16) (65535*g_array_index((circ->gr), gdouble, dx));
			(clr.blue)=(guint16) (65535*g_array_index((circ->bl), gdouble, dx));
			alp=(guint16) (65535*g_array_index((circ->al), gdouble, dx));
			gdk_colormap_alloc_color(cmp, &clr, FALSE, TRUE);
			gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(cls), &clr);
			gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(cls), alp);
			hbx=gtk_hbox_new(FALSE, 0);
			gtk_widget_show(hbx);
			gtk_paned_add2(GTK_PANED(vpn), hbx);
			adj=(GtkAdjustment*) gtk_adjustment_new(grp, 0, ((circ->data)->len), 1.0, 5.0, 0.0);
			jix=gtk_spin_button_new(adj, 0, 0);
			gtk_widget_show(jix);
			g_signal_connect(G_OBJECT(jix), "value-changed", G_CALLBACK(upj), NULL);
			gtk_box_pack_start(GTK_BOX(hbx), jix, FALSE, FALSE, 2);
			str=g_strdup(((FdtdMat*) g_ptr_array_index(mtr, dx))->nme);
			lbl=gtk_label_new(str);
			gtk_widget_show(lbl);
			gtk_box_pack_start(GTK_BOX(hbx), lbl, FALSE, FALSE, 2);
			gtk_widget_show(hwn);
		}
	}
}
