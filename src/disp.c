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
	FdtdMat *mts;
	GdkColor clr;
	gdouble *ptr;
	gdouble iv;
	gint dx;

	dx=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(wgt));
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(cls), &clr);
	alp=gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(cls));
	mts=(FdtdMat*) g_ptr_array_index(mtr, dx);
	ptr=&(mts->red);
	iv=((gdouble) (clr.red))/65535;
	*ptr=iv;
	ptr=&(mts->grn);
	iv=((gdouble) (clr.green))/65535;
	*ptr=iv;
	ptr=&(mts->blu);
	iv=((gdouble) (clr.blue))/65535;
	*ptr=iv;
	ptr=&(mts->alp);
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
	FdtdMat *mts;
	GdkColor clr;
	gint dx;

	dx=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(wgt));
	mts=(FdtdMat*) g_ptr_array_index(mtr, dx);
	(clr.red)=(guint16) (65535*(mts->red));
	(clr.green)=(guint16) (65535*(mts->grn));
	(clr.blue)=(guint16) (65535*(mts->blu));
	alp=(guint16) (65535*(mts->alp));
	gdk_colormap_alloc_color(cmp, &clr, FALSE, TRUE);
	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(cls), &clr);
	gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(cls), alp);
}

void dpr(GtkWidget *widget, gpointer data)
{
	FdtdMat *mts;
	GtkWidget *btt, *vpn, *hbx, *lbl, *vbx;
	GtkAdjustment *adj;
	AtkObject *atkwgt, *atklbl;
	GdkColor clr;
	gchar *str;
	gint dx;

	hwn=gtk_dialog_new_with_buttons(_("Display Properties"), GTK_WINDOW(wdw), GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
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
	mts=(FdtdMat*) g_ptr_array_index(mtr, dx);
	(clr.red)=(guint16) (65535*(mts->red));
	(clr.green)=(guint16) (65535*(mts->grn));
	(clr.blue)=(guint16) (65535*(mts->blu));
	alp=(guint16) (65535*(mts->alp));
	gdk_colormap_alloc_color(cmp, &clr, FALSE, TRUE);
	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(cls), &clr);
	gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(cls), alp);
	hbx=gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbx);
	gtk_paned_add2(GTK_PANED(vpn), hbx);
	adj=(GtkAdjustment*) gtk_adjustment_new(dx, 0, (mtr->len), 1.0, 5.0, 0.0);
	jix=gtk_spin_button_new(adj, 0, 0);
	gtk_widget_show(jix);
	g_signal_connect(G_OBJECT(jix), "value-changed", G_CALLBACK(upj), NULL);
	gtk_box_pack_start(GTK_BOX(hbx), jix, FALSE, FALSE, 2);
	str=g_strdup(mts->nme);
	lbl=gtk_label_new(str);
	gtk_widget_show(lbl);
	gtk_box_pack_start(GTK_BOX(hbx), lbl, FALSE, FALSE, 2);
	gtk_widget_show(hwn);
}
