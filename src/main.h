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
	typedef struct _FdtdMat FdtdMat;
	typedef struct _FdtdLayout FdtdLayout;
	struct _FdtdLayout {gint geo, mat, ring; gdouble xc, yc, rr, pitch, azim;};
	struct _FdtdMat {gchar *nme; gdouble alp, blu, grn, red;};
	G_END_DECLS
#endif
