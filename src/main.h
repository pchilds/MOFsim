/***************************************************************************
 *            main.h
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

#ifndef __MAIN_H__
#	define __MAIN_H__
#	include <gtk/gtk.h>
#	include <glib/gi18n.h>
#	include <math.h>
#	include "drawcirc.h"
	G_BEGIN_DECLS
#	ifdef G_OS_WIN32
#		define DLMT "\r\n"
#	else
#		define DLMT "\n"
#	endif
#	define PML 5.0
#	define CLL 150.0
#	define RDF 62.5
#	define PTC 10
#	define FCN 0.65
#	define FWD 0.15
#	define MFD 10
#	define RSN 10
	typedef struct _FdtdMat FdtdMat;
	typedef struct _FdtdLayout FdtdLayout;
	struct _FdtdLayout {gint geo, mat, ring; gdouble xc, yc, rr, pitch, azim;};
	struct _FdtdMat {gchar *nme; gdouble d11, d12, d13, d22, d23, d33, dpi, dpr, dsg, dx2, dx3, m11, m12, m13, m22, m23, m33, mpi, mpr, msg, mx2, mx3;};
	G_END_DECLS
#endif
