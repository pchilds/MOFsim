/***************************************************************************
 *            main.c
 *
 *  A GTK+ front-end for running FDTD simulations (meep back-end) for
 *  microstructured fibre
 *  version 0.1.0
 *  Features:
 *            
 *
 *  Tue Jul  26 15:14:14 2011
 *  Copyright  2011  Paul Childs
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

#include "config.h"
#include <gdk/gdkkeysyms.h>
#include "main.h"
#include "data.h"
#include "disp.h"
#include "util.h"
#if MEEP_II
#	include "pprc.h"
#	define vrs "parallel version enabled";
gint crn;
#else
#	include "proc.h"
#	define vrs "parallel version disabled";
#endif

gchar img[]=vrs;
gchar *flr=NULL;
gdouble fcn, fsz, fwd;
gint gem, grp;
GPtrArray *fbl, *mtr;
GtkWidget *az, *cb1, *cb2, *crc, *di1, *di2, *di3, *di4, *di5, *di6, *ex, *ey, *ez, *hx, *hy, *hz, *lb1, *lb2, *lb3, *mfd, *mg1, *mg2, *mg3, *mg4, *mg5, *mg6, *pt, *rd, *rg, *sbr, *wdw, *sc, *xc, *yc;
gulong cb1_id, cb2_id;

int main(int argc, char *argv[])
{
	AtkObject *atklbl, *atkwgt;
	DrawCirc *circ;
	DrawCircData *dcd;
	DrawCircGroup *dcg;
	GArray *ary;
	GPtrArray *pta;
	GSList *grp=NULL;
	GtkAccelGroup *a_g=NULL;
	GtkAdjustment *adj;
	GtkCellRenderer *crn;
	GtkListStore *sto;
	GtkTreeIter it1, it2;
	GtkWidget *btt, *hpn, *lbl, *mnb, *mni, *mnu, *smu, *tbl, *vbx;

	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
	gtk_init(&argc, &argv);
	wdw=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wdw), _("FDTD solver for microstructured fibre"));
	g_signal_connect_swapped(G_OBJECT(wdw), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	a_g=gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(wdw), a_g);
	vbx=gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(wdw), vbx);
	gtk_widget_show(vbx);
	mnb=gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbx), mnb, FALSE, FALSE, 2);
	gtk_widget_show(mnb);
	mnu=gtk_menu_new();
	mni=gtk_menu_item_new_with_mnemonic(_("Run"));
	gtk_widget_add_accelerator(mni, "activate", a_g, GDK_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(go), (gpointer)wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
	gtk_widget_add_accelerator(mni, "activate", a_g, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(opn), (gpointer)wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
	gtk_widget_add_accelerator(mni, "activate", a_g, GDK_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(sav), (gpointer)wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, a_g);
	gtk_widget_add_accelerator(mni, "activate", a_g, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(gtk_main_quit), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_File"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	mnu=gtk_menu_new();
	di1=gtk_check_menu_item_new_with_label(_("Permittivity"));
	di2=gtk_check_menu_item_new_with_label(_("Dielectric\nAnisotropy"));
	di3=gtk_check_menu_item_new_with_label(_("Dielectric\nDispersion/Loss/Gain"));
	di4=gtk_check_menu_item_new_with_label(_("Electric\nConductiviy"));
	di5=gtk_check_menu_item_new_with_label(_("Dielectric\nχ2 Nonlinearity"));
	di6=gtk_check_menu_item_new_with_label(_("Dielectric\nχ3 Nonlinearity"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(di1), TRUE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(di2), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(di3), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(di4), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(di5), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(di6), FALSE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), di1);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), di2);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), di3);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), di4);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), di5);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), di6);
	gtk_widget_show(di1);
	gtk_widget_show(di2);
	gtk_widget_show(di3);
	gtk_widget_show(di4);
	gtk_widget_show(di5);
	gtk_widget_show(di6);
	g_signal_connect(G_OBJECT(di1), "toggled", G_CALLBACK(dit), (gpointer)mnu);
	mg1=gtk_check_menu_item_new_with_label(_("Permeability"));
	mg2=gtk_check_menu_item_new_with_label(_("Magnetic\nAnisotropy"));
	mg3=gtk_check_menu_item_new_with_label(_("Magnetic\nDispersion/Loss/Gain"));
	mg4=gtk_check_menu_item_new_with_label(_("Magnetic\nConductiviy"));
	mg5=gtk_check_menu_item_new_with_label(_("Magnetic\nχ2 Nonlinearity"));
	mg6=gtk_check_menu_item_new_with_label(_("Magnetic\nχ3 Nonlinearity"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mg1), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mg2), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mg3), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mg4), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mg5), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mg6), FALSE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mg1);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mg2);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mg3);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mg4);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mg5);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mg6);
	gtk_widget_show(mg1);
	g_signal_connect(G_OBJECT(mg1), "toggled", G_CALLBACK(mgt), (gpointer)mnu);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_label(_("Display Properties:"));
	gtk_widget_add_accelerator(mni, "activate", a_g, GDK_F2, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(dpr), (gpointer)wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_Properties"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	mnu=gtk_menu_new();
	mfd=gtk_check_menu_item_new_with_label(_("Mode finder"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mfd), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mfd);
	gtk_widget_show(mfd);
	mni=gtk_menu_item_new_with_label(_("Source:"));
	gtk_widget_add_accelerator(mni, "activate", a_g, GDK_F3, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(src), (gpointer)wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	smu=gtk_menu_new();
	ez=gtk_radio_menu_item_new_with_label(grp, "Ez");
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(ez));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ez), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(smu), ez);
	gtk_widget_show(ez);
	ex=gtk_radio_menu_item_new_with_label(grp, "Ex");
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(ex));
	gtk_menu_shell_append(GTK_MENU_SHELL(smu), ex);
	gtk_widget_show(ex);
	ey=gtk_radio_menu_item_new_with_label(grp, "Ey");
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(ey));
	gtk_menu_shell_append(GTK_MENU_SHELL(smu), ey);
	gtk_widget_show(ey);
	hz=gtk_radio_menu_item_new_with_label(grp, "Hz");
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(hz));
	gtk_menu_shell_append(GTK_MENU_SHELL(smu), hz);
	gtk_widget_show(hz);
	hx=gtk_radio_menu_item_new_with_label(grp, "Hx");
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(hx));
	gtk_menu_shell_append(GTK_MENU_SHELL(smu), hx);
	gtk_widget_show(hx);
	hy=gtk_radio_menu_item_new_with_label(grp, "Hy");
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(hy));
	gtk_menu_shell_append(GTK_MENU_SHELL(smu), hy);
	gtk_widget_show(hy);
	mni=gtk_menu_item_new_with_label(_("Polarisation:"));
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), smu);
#if MEEP_II
	mni=gtk_menu_item_new_with_label(_("Parallelisation:"));
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(crs), (gpointer)wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
#endif
	mni=gtk_menu_item_new_with_mnemonic(_("_Analysis"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	mnu=gtk_menu_new();
	mni=gtk_menu_item_new_with_label(_("Instructions"));
	gtk_widget_add_accelerator(mni, "activate", a_g, GDK_F1, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(hlp), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(abt), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_Help"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	gtk_menu_item_right_justify(GTK_MENU_ITEM(mni));
	hpn=gtk_hpaned_new();
	gtk_widget_show(hpn);
	gtk_container_add(GTK_CONTAINER(vbx), hpn);
	crc=draw_circ_new();
	g_signal_connect(crc, "moved", G_CALLBACK(cmv), NULL);
	g_signal_connect(crc, "highlight", G_CALLBACK(chl), NULL);
	draw_circ_update_scale(crc, PML-(CLL/2), (CLL/2)-PML, PML-(CLL/2), (CLL/2)-PML);
	gtk_widget_show(crc);
	gtk_paned_add2(GTK_PANED(hpn), crc);
	sbr=gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbx), sbr, FALSE, FALSE, 2);
	gtk_widget_show(sbr);
	gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), _(img)), _(img));
	vbx=gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbx);
	gtk_paned_add1(GTK_PANED(hpn), vbx);
	sto=gtk_list_store_new(1, G_TYPE_STRING);
	gtk_list_store_append(sto, &it1);
	gtk_list_store_set(sto, &it1, 0, _("Single Cylinder"), -1);
	gtk_list_store_append(sto, &it1);
	gtk_list_store_set(sto, &it1, 0, _("Hexagonal Array\nminus centre"), -1);
	gtk_list_store_append(sto, &it1);
	gtk_list_store_set(sto, &it1, 0, _("Hexagonal Ring"), -1);
	lbl=gtk_label_new_with_mnemonic(_("_Geometry Type:"));
	gtk_box_pack_start(GTK_BOX(vbx), lbl, FALSE, FALSE, 2);
	gtk_widget_show(lbl);
	cb1=gtk_combo_box_new_with_model(GTK_TREE_MODEL(sto));
	g_object_unref(G_OBJECT(sto));
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), 0);
	cb1_id=g_signal_connect(G_OBJECT(cb1), "changed", G_CALLBACK(gmc), NULL);
	gtk_box_pack_start(GTK_BOX(vbx), cb1, FALSE, FALSE, 2);
	gtk_widget_show(cb1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), cb1);
	crn=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(cb1), crn, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(cb1), crn, "text", 0, NULL);
	lbl=gtk_label_new_with_mnemonic(_("_GOrientation\n/Scale"));
	gtk_box_pack_start(GTK_BOX(vbx), lbl, FALSE, FALSE, 2);
	gtk_widget_show(lbl);
	tbl=gtk_table_new(6, 2, FALSE);
	gtk_box_pack_start(GTK_BOX(vbx), tbl, FALSE, FALSE, 2);
	gtk_widget_show(tbl);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), tbl);
	lbl=gtk_label_new(_("x centre:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	xc=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(xc), "value-changed", G_CALLBACK(xcc), NULL);
	gtk_table_attach(GTK_TABLE(tbl), xc, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(xc);
	atkwgt=gtk_widget_get_accessible(xc);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lbl=gtk_label_new(_("y centre:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	yc=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(yc), "value-changed", G_CALLBACK(ycc), NULL);
	gtk_table_attach(GTK_TABLE(tbl), yc, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(yc);
	atkwgt=gtk_widget_get_accessible(yc);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lbl=gtk_label_new(_("radius:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new((CLL/2)-PML, 0, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	rd=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(rd), "value-changed", G_CALLBACK(rdc), NULL);
	gtk_table_attach(GTK_TABLE(tbl), rd, 0, 1, 5, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(rd);
	atkwgt=gtk_widget_get_accessible(rd);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lb1=gtk_label_new(_("pitch:"));
	gtk_table_attach(GTK_TABLE(tbl), lb1, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	adj=(GtkAdjustment*) gtk_adjustment_new((CLL/4)-PML, 0, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	pt=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(pt), "value-changed", G_CALLBACK(ptc), NULL);
	gtk_table_attach(GTK_TABLE(tbl), pt, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	atkwgt=gtk_widget_get_accessible(pt);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lb1));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lb2=gtk_label_new(_("ring:"));
	gtk_table_attach(GTK_TABLE(tbl), lb2, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	adj=(GtkAdjustment*) gtk_adjustment_new(1, 1, G_MAXINT8, 1, 5, 0);
	rg=gtk_spin_button_new(adj, 0.5, 0);
	g_signal_connect(G_OBJECT(rg), "value-changed", G_CALLBACK(rgc), NULL);
	gtk_table_attach(GTK_TABLE(tbl), rg, 1, 2, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	atkwgt=gtk_widget_get_accessible(rg);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lb2));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lb3=gtk_label_new(_("azimuth:\n(degrees)"));
	gtk_table_attach(GTK_TABLE(tbl), lb3, 1, 2, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	adj=(GtkAdjustment*) gtk_adjustment_new(0.0, -180, 180, 10.0, 30.0, 0.0);
	az=gtk_spin_button_new(adj, 0.5, 0);
	g_signal_connect(G_OBJECT(az), "value-changed", G_CALLBACK(azc), NULL);
	gtk_table_attach(GTK_TABLE(tbl), az, 1, 2, 5, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	atkwgt=gtk_widget_get_accessible(az);
	atklbl=gtk_widget_get_accessible(GTK_WIDGET(lb3));
	atk_object_add_relationship(atklbl, ATK_RELATION_LABEL_FOR, atkwgt);
	atk_object_add_relationship(atkwgt, ATK_RELATION_LABELLED_BY, atklbl);
	lbl=gtk_label_new_with_mnemonic(_("_Material:"));
	gtk_box_pack_start(GTK_BOX(vbx), lbl, FALSE, FALSE, 2);
	gtk_widget_show(lbl);
	sto=gtk_list_store_new(1, G_TYPE_STRING);
	gtk_list_store_append(sto, &it2);
	gtk_list_store_set(sto, &it2, 0, _("New"), -1);
	gtk_list_store_append(sto, &it2);
	gtk_list_store_set(sto, &it2, 0, _("Air"), -1);
	cb2=gtk_combo_box_new_with_model(GTK_TREE_MODEL(sto));
	g_object_unref(G_OBJECT(sto));
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), 1);
	cb2_id=g_signal_connect(G_OBJECT(cb2), "changed", G_CALLBACK(mtc), (gpointer)wdw);
	gtk_box_pack_start(GTK_BOX(vbx), cb2, FALSE, FALSE, 2);
	gtk_widget_show(cb2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), cb2);
	crn=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(cb2), crn, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(cb2), crn, "text", 0, NULL);
	btt=gtk_button_new_with_mnemonic(_("_Delete"));
	gtk_box_pack_start(GTK_BOX(vbx), btt, FALSE, FALSE, 2);
	gtk_widget_show(btt);
	g_signal_connect(G_OBJECT(btt), "clicked", G_CALLBACK(del), NULL);
	btt=gtk_button_new_with_mnemonic(_("Add/_Update"));
	gtk_box_pack_start(GTK_BOX(vbx), btt, FALSE, FALSE, 2);
	gtk_widget_show(btt);
	g_signal_connect(G_OBJECT(btt), "clicked", G_CALLBACK(add), NULL);
	flr=g_strdup("/home");
	{grp=0; gem=0; fcn=0.15; fwd=0.10; fsz=1;}
	mtr=g_ptr_array_new();
	fbl=g_ptr_array_new();
	dcd=g_new(DrawCircData, 1);
	{(dcd->x)=0; (dcd->y)=0; (dcd->r)=(CLL/2)-PML;}
	ary=g_array_new(FALSE, FALSE, sizeof(DrawCircData*));
	g_array_append_val(ary, dcd);
	dcg=g_new(DrawCircGroup, 1);
	{(dcg->col)=0; (dcg->xyr)=ary;}
	pta=g_ptr_array_new();
	g_ptr_array_add(pta, (gpointer) dcg);
	circ=DRAW_CIRC(crc);
	{(circ->data)=pta; (circ->hlc)=0; (circ->hlp)=0;}
	gtk_widget_show(wdw);
	gtk_main();
	return 0;
}
