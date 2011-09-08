/***************************************************************************
 *            util.h
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

#ifndef __UTIL_H__
#	define __UTIL_H__
#	include "main.h"
	extern gint grp;
	extern GPtrArray *fbl, *mtr;
	extern GtkWidget *cb1, *cb2, *crc, *sbr;
	extern GtkWidget *di2, *di3, *di4, *di5, *di6, *mg2, *mg3, *mg4, *mg5, *mg6;
	void abt(GtkWidget*, gpointer);/*display about window*/
	void add(GtkWidget*, gpointer);/*adds or updates currently selected group*/
	void chl(DrawCirc*, gpointer);/*change selected group*/
	void cmv(DrawCirc*, gpointer);/*track mouse position on layout display*/
	void del(GtkWidget*, gpointer);/*removes currently selected group from the layout*/
	void dit(GtkWidget*, gpointer);/*populate dielectric properties*/
	void gmc(GtkWidget*, gpointer);/*change of geometry*/
	void hlp(GtkWidget*, gpointer);/*loads help manual*/
	void mgt(GtkWidget*, gpointer);/*populate magnetic properties*/
	void mtc(GtkWidget*, gpointer);/*change of material*/
#endif
