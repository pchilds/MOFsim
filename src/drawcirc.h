/***************************************************************************
 *            drawcirc.h
 *
 *  A GTK+ widget that draws circles
 *
 *  Sat Dec  4 17:18:14 2010
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

#ifndef __DRAW_CIRC_H__
#	define __DRAW_CIRC_H__
#	include <gtk/gtk.h>
	G_BEGIN_DECLS
#	define DRAW_TYPE_CIRC (draw_circ_get_type())
#	define DRAW_CIRC(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), DRAW_TYPE_CIRC, DrawCirc))
#	define DRAW_IS_CIRC(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), DRAW_TYPE_CIRC))
#	define DRAW_CIRC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), DRAW_CIRC, DrawCircClass))
#	define DRAW_IS_CIRC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), DRAW_TYPE_CIRC))
#	define DRAW_CIRC_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), DRAW_TYPE_CIRC, DrawCircClass))
	typedef struct _DrawCircData DrawCircData;
	typedef struct _DrawCircGroup DrawCircGroup;
	typedef struct _DrawCirc DrawCirc;
	typedef struct _DrawCircClass DrawCircClass;
	typedef enum
	{
		DRAW_CIRC_ZOOM_OUT = 1 << 0,
		DRAW_CIRC_ZOOM_ENB = 1 << 1,
		DRAW_CIRC_ZOOM_SGL = 1 << 2
	} DrawCircZoom;
	struct _DrawCircData {gdouble x, y, r;};
	struct _DrawCircGroup {gint col; GArray *xyr;};
	struct _DrawCirc
	{
		GtkDrawingArea parent;
		GPtrArray *data; /* x, y and r data sets */
		GArray *rd, *gr, *bl, *al; /* colour and alpha maps for the groupings */
		gint hlc, hlp; /* current and prior highlighted groups */
		gint zmode; /* zoom mode flags */
		gdouble xps, yps; /* x and y position of mouse */
	};
	struct _DrawCircClass
	{
		GtkDrawingAreaClass parent_class;
		void (*moved) (DrawCirc *circ);
		void (*highlight) (DrawCirc *circ);
	};
	gboolean draw_circ_redraw(GtkWidget*);
	gboolean draw_circ_update_scale(GtkWidget *wgt, gdouble xn, gdouble xx, gdouble yn, gdouble yx);
	gboolean draw_circ_print_eps(GtkWidget*, gchar*);
	gboolean draw_circ_print_png(GtkWidget*, gchar*);
	gboolean draw_circ_print_svg(GtkWidget*, gchar*);
	GtkWidget *draw_circ_new(void);
	G_END_DECLS
#endif
