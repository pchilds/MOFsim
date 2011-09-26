/***************************************************************************
 *            drawcirc.c
 *
 *  A GTK+ widget that draws circles
 *  version 0.1.0
 *  Features:
 *            
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

#include <gtk/gtk.h>
#include <cairo-ps.h>
#include <cairo-svg.h>
#include <math.h>
#include "drawcirc.h"

#define DRAW_CIRC_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), DRAW_TYPE_CIRC, DrawCircPrivate))
#define MY_2PI 6.2831853071795864769252867665590057683943387987502
#define DZE 0.00001 /* divide by zero threshold */
#define NZE -0.00001 /* negative of this */
#define ZS 0.5 /* zoom scale */
#define ZSC 0.5 /* 1 minus this */
#define UZ 2 /* inverse of this */
#define UZC 1 /* this minus 1 */
G_DEFINE_TYPE (DrawCirc, draw_circ, GTK_TYPE_DRAWING_AREA);
enum {PROP_0, PROP_BXN, PROP_BXX, PROP_BYN, PROP_BYX};
enum {MOVED, HLITE, LAST_SIGNAL};
static guint draw_circ_signals[LAST_SIGNAL]={0};
typedef struct _DrawCircPrivate DrawCircPrivate;
struct sd {gdouble xmin, ymin, xmax, ymax;};
struct _DrawCircPrivate {struct sd bounds, rescale; guint flagr, flago;};

static void drwz(GtkWidget *wgt, cairo_t *cr)
{
	gint xw;
	gdouble dt;
	DrawCirc *circ;

	xw=(wgt->allocation.width);
	circ=DRAW_CIRC(wgt);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, xw-21.5, 0.5, 10, 10);
	cairo_rectangle(cr, xw-10.5, 0.5, 10, 10);
	cairo_move_to(cr, xw-9, 5.5);
	cairo_line_to(cr, xw-2, 5.5);
	cairo_move_to(cr, xw-5.5, 2);
	cairo_line_to(cr, xw-5.5, 9);
	if (((circ->zmode)&DRAW_CIRC_ZOOM_OUT)==0)
	{
		cairo_move_to(cr, xw-6.5, 2.5);
		cairo_line_to(cr, xw-5.5, 2);
		cairo_line_to(cr, xw-4.5, 2.5);
		cairo_move_to(cr, xw-2.5, 4.5);
		cairo_line_to(cr, xw-2, 5.5);
		cairo_line_to(cr, xw-2.5, 6.5);
		cairo_move_to(cr, xw-6.5, 8.5);
		cairo_line_to(cr, xw-5.5, 9);
		cairo_line_to(cr, xw-4.5, 8.5);
		cairo_move_to(cr, xw-8.5, 4.5);
		cairo_line_to(cr, xw-9, 5.5);
		cairo_line_to(cr, xw-8.5, 6.5);
	}
	else
	{
		cairo_move_to(cr, xw-7.5, 3.5);
		cairo_line_to(cr, xw-3.5, 7.5);
		cairo_move_to(cr, xw-7.5, 7.5);
		cairo_line_to(cr, xw-3.5, 3.5);
	}
	cairo_stroke(cr);
	if (((circ->zmode)&DRAW_CIRC_ZOOM_ENB)!=0)
	{
		if (((circ->zmode)&DRAW_CIRC_ZOOM_SGL)!=0)
		{
			cairo_move_to(cr, xw-20, 2);
			cairo_line_to(cr, xw-13, 9);
			cairo_move_to(cr, xw-20, 9);
			cairo_line_to(cr, xw-13, 2);
			cairo_stroke(cr);
		}
		else
		{
			cairo_save(cr);
			dt=1;
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xw-20, 2.5);
			cairo_line_to(cr, xw-13, 2.5);
			cairo_move_to(cr, xw-20, 8.5);
			cairo_line_to(cr, xw-13, 8.5);
			cairo_move_to(cr, xw-19.5, 2);
			cairo_line_to(cr, xw-19.5, 9);
			cairo_move_to(cr, xw-13.5, 2);
			cairo_line_to(cr, xw-13.5, 9);
			cairo_stroke(cr);
			cairo_restore(cr);
		}
	}
}

static void drwc(GtkWidget *wgt, cairo_t *cr)
{
	DrawCirc *circ;
	DrawCircData *dtm;
	DrawCircGroup *dcg;
	DrawCircPrivate *priv;
	gdouble av, sx, sy, vv, wv, zv;
	gint ft, j, k, lt, rw, ww, xw, yw;
	GSList *dta;

	xw=((wgt->allocation).width);
	yw=((wgt->allocation).height);
	circ=DRAW_CIRC(wgt);
	priv=DRAW_CIRC_GET_PRIVATE(wgt);
	if (circ->data)
	{
		sx=((priv->bounds.xmax)-(priv->bounds.xmin))/xw;
		sy=((priv->bounds.ymax)-(priv->bounds.ymin))/yw;
		if (sx<sy)
		{
			{(priv->flago)=1; sx=1/sy;}/*need to draw a box to clip outside*/
			for (k=0;k<(circ->data->len);k++)
			{
				dcg=(DrawCircGroup*) g_ptr_array_index((circ->data), k);
				if (k==(circ->hlc)) {vv=0; wv=1; zv=0; av=1;}
				else if ((dcg->col)<0) {vv=1; wv=1; zv=1; av=1;}
				else
				{
					ft=fmod((dcg->col),(circ->rd->len));
					vv=g_array_index((circ->rd), gdouble, ft);
					wv=g_array_index((circ->gr), gdouble, ft);
					zv=g_array_index((circ->bl), gdouble, ft);
					av=g_array_index((circ->al), gdouble, ft);
				}
				cairo_set_source_rgba(cr, vv, wv, zv, av);
				for (j=0;j<((dcg->xyr)->len);j++)
				{
					dtm=(DrawCircData*) g_array_index((dcg->xyr), DrawCircData*, j);
					ww=(xw+(sx*((2*(dtm->x))-(priv->bounds.xmin)-(priv->bounds.xmax))))/2;
					yw=sx*((priv->bounds.ymax)-(dtm->y));
					rw=sx*(dtm->r);
					cairo_arc(cr, ww, yw, rw, 0, MY_2PI);
					cairo_fill(cr);
				}
			}
		}
		else
		{
			{(priv->flago)=0; sx=1/sx;}/*need to draw a box to clip outside*/
			for (k=0;k<(circ->data->len);k++)
			{
				dcg=(DrawCircGroup*) g_ptr_array_index((circ->data), k);
				if (k==(circ->hlc)) {vv=0; wv=1; zv=0; av=1;}
				else
				{
					ft=fmod((dcg->col),(circ->rd->len));
					vv=g_array_index((circ->rd), gdouble, ft);
					wv=g_array_index((circ->gr), gdouble, ft);
					zv=g_array_index((circ->bl), gdouble, ft);
					av=g_array_index((circ->al), gdouble, ft);
				}
				cairo_set_source_rgba(cr, vv, wv, zv, av);
				for (j=0;j<((dcg->xyr)->len);j++)
				{
					dtm=(DrawCircData*) g_array_index((dcg->xyr), DrawCircData*, j);
					xw=sx*((dtm->x)-(priv->bounds.xmin));
					ww=(yw+(sx*((priv->bounds.ymax)+(priv->bounds.ymin)-(2*(dtm->y)))))/2;
					rw=sx*(dtm->r);
					cairo_arc(cr, xw, ww, rw, 0, MY_2PI);
					cairo_fill(cr);
				}
			}
		}
	}
}

gboolean draw_circ_redraw(GtkWidget *wgt)
{
	GdkRegion *rgn;

	if (!(wgt->window)) return;
	rgn=gdk_drawable_get_clip_region(wgt->window);
	gdk_window_invalidate_region((wgt->window), rgn, TRUE);
	gdk_window_process_updates((wgt->window), TRUE);
	gdk_region_destroy(rgn);
	return FALSE;
}

gboolean draw_circ_update_scale(GtkWidget *wgt, gdouble xn, gdouble xx, gdouble yn, gdouble yx)
{
	DrawCircPrivate *priv;

	priv=DRAW_CIRC_GET_PRIVATE(wgt);
	(priv->bounds.xmin)=xn;
	(priv->bounds.xmax)=xx;
	(priv->bounds.ymin)=yn;
	(priv->bounds.ymax)=yx;
	return draw_circ_redraw(wgt);
}

gboolean draw_circ_print_eps(GtkWidget *wgt, gchar* fout)
{
	cairo_t *cr;
	cairo_surface_t *sfc;

	sfc=cairo_ps_surface_create(fout, (gdouble) (wgt->allocation.width), (gdouble) (wgt->allocation.height));
	cairo_ps_surface_set_eps(sfc, TRUE);
	cairo_ps_surface_restrict_to_level(sfc, CAIRO_PS_LEVEL_2);
	cr=cairo_create(sfc);
	drwc(wgt, cr);
	cairo_surface_show_page(sfc);
	cairo_destroy(cr);
	cairo_surface_finish(sfc);
	cairo_surface_destroy(sfc);
	return FALSE;
}

gboolean draw_circ_print_png(GtkWidget *wgt, gchar* fout)
{
	cairo_t *cr;
	cairo_surface_t *sfc;

	sfc=cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (gdouble) (wgt->allocation.width), (gdouble) (wgt->allocation.height));
	cr=cairo_create(sfc);
	drwc(wgt, cr);
	cairo_surface_write_to_png(sfc, fout);
	cairo_destroy(cr);
	cairo_surface_destroy(sfc);
	return FALSE;
}

gboolean draw_circ_print_svg(GtkWidget *wgt, gchar* fout)
{
	cairo_t *cr;
	cairo_surface_t *sfc;

	sfc=cairo_svg_surface_create(fout, (gdouble) (wgt->allocation.width), (gdouble) (wgt->allocation.height));
	cr=cairo_create(sfc);
	drwc(wgt, cr);
	cairo_destroy(cr);
	cairo_surface_destroy(sfc);
	return FALSE;
}

static gboolean draw_circ_button_press(GtkWidget *wgt, GdkEventButton *vnt)
{
	DrawCirc *circ;
	DrawCircPrivate *priv;

	priv=DRAW_CIRC_GET_PRIVATE(wgt);
	circ=DRAW_CIRC(wgt);
	if (((vnt->y)<=11)&&((vnt->x)>=(wgt->allocation.width)-22)) (priv->flagr)=1;
	else if (((circ->zmode)&DRAW_CIRC_ZOOM_ENB)!=0) {(priv->rescale.xmin)=(circ->xps); (priv->rescale.ymin)=(circ->yps); (priv->flagr)=2;}
	else (priv->flagr)=3;
	return FALSE;
}

static gboolean draw_circ_motion_notify(GtkWidget *wgt, GdkEventMotion *vnt)
{
	DrawCirc *circ;
	DrawCircPrivate *priv;
	gdouble ds;

	priv=DRAW_CIRC_GET_PRIVATE(wgt);
	circ=DRAW_CIRC(wgt);
	if ((priv->flago)>0)
	{
		ds=((2*(vnt->x))-(wgt->allocation.width))*((priv->bounds.ymax)-(priv->bounds.ymin))/((wgt->allocation.height)*((priv->bounds.xmax)-(priv->bounds.xmin)));
		if ((ds<=1)&&(ds>=-1))
		{
			(circ->xps)=(((priv->bounds.xmax)*(1+ds))+((priv->bounds.xmin)*(1-ds)))/2;
			ds=(vnt->y)/(wgt->allocation.height);
			(circ->yps)=((priv->bounds.ymax)*(1-ds))+((priv->bounds.ymin)*ds);
			g_signal_emit(circ, draw_circ_signals[MOVED], 0);
		}
	}
	else
	{
		ds=((2*(vnt->y))-(wgt->allocation.height))*((priv->bounds.xmax)-(priv->bounds.xmin))/((wgt->allocation.width)*((priv->bounds.ymax)-(priv->bounds.ymin)));
		if ((ds<=1)&&(ds>=-1))
		{
			(circ->yps)=(((priv->bounds.ymax)*(1-ds))+((priv->bounds.ymin)*(1+ds)))/2;
			ds=(vnt->x)/(wgt->allocation.width);
			(circ->xps)=((priv->bounds.xmax)*ds)+((priv->bounds.xmin)*(1-ds));
			g_signal_emit(circ, draw_circ_signals[MOVED], 0);
		}
	}
	return FALSE;
}

static gboolean draw_circ_button_release(GtkWidget *wgt, GdkEventButton *vnt)
{
	DrawCirc *circ;
	DrawCircData *dtm;
	DrawCircPrivate *priv;
	GArray *ary;
	gdouble s, xn, xx, yn, yx;
	gint j, k, xw;
	GSList *dta;

	priv=DRAW_CIRC_GET_PRIVATE(wgt);
	circ=DRAW_CIRC(wgt);
	if ((priv->flagr)==1)
	{
		if ((vnt->y)<=11)
		{
			xw=(wgt->allocation.width);
			if ((vnt->x)>=xw-22)
			{
				if ((vnt->x)>=xw-11) (circ->zmode)^=DRAW_CIRC_ZOOM_OUT;
				else if (((circ->zmode)&DRAW_CIRC_ZOOM_SGL)!=0) (circ->zmode)&=DRAW_CIRC_ZOOM_OUT;
				else if (((circ->zmode)&DRAW_CIRC_ZOOM_ENB)!=0) (circ->zmode)|=DRAW_CIRC_ZOOM_SGL;
				else (circ->zmode)|=DRAW_CIRC_ZOOM_ENB;
				draw_circ_redraw(wgt);
			}
		}
	}
	else if ((priv->flagr)==2)
	{
		if (((circ->zmode)&DRAW_CIRC_ZOOM_SGL)==0)
		{
			{(priv->rescale.xmax)=(circ->xps); (priv->rescale.ymax)=(circ->yps);}
			xn=(priv->rescale.xmax)-(priv->rescale.xmin);
			yn=(priv->rescale.ymax)-(priv->rescale.ymin);
			if (((xn>DZE)||(xn<NZE))&&((yn>DZE)||(yn<NZE)))
			{
				if (((circ->zmode)&DRAW_CIRC_ZOOM_OUT)==0) draw_circ_update_scale(wgt, (priv->rescale.xmin), (priv->rescale.xmax), (priv->rescale.ymin), (priv->rescale.ymax));
				else
				{
					s=((priv->bounds.xmax)-(priv->bounds.xmin))/xn;
					if (s>0)
					{
						xn=((priv->bounds.xmin)-(priv->rescale.xmin))*s;
						xn+=(priv->bounds.xmin);
						xx=((priv->bounds.xmax)-(priv->rescale.xmax))*s;
						xx+=(priv->bounds.xmax);
						s=((priv->bounds.ymax)-(priv->bounds.ymin))/yn;
						if (s>0)
						{
							yn=((priv->bounds.ymin)-(priv->rescale.ymin))*s;
							yn+=(priv->bounds.ymin);
							yx=((priv->bounds.ymax)-(priv->rescale.ymax))*s;
							yx+=(priv->bounds.ymax);
							draw_circ_update_scale(wgt, xn, xx, yn, yx);
						}
						else if (s<0)
						{
							yn=((priv->rescale.ymax)-(priv->bounds.ymin))*s;
							yn+=(priv->bounds.ymin);
							yn=((priv->rescale.ymin)-(priv->bounds.ymax))*s;
							yx+=(priv->bounds.ymax);
							draw_circ_update_scale(wgt, xn, xx, yn, yx);
						}
					}
					else if (s<0)
					{
						xn=((priv->rescale.xmax)-(priv->bounds.xmin))*s;
						xn+=(priv->bounds.xmin);
						xn=((priv->rescale.xmin)-(priv->bounds.xmax))*s;
						xx+=(priv->bounds.xmax);
						s=((priv->bounds.ymax)-(priv->bounds.ymin))/yn;
						if (s>0)
						{
							yn=((priv->bounds.ymin)-(priv->rescale.ymin))*s;
							yn+=(priv->bounds.ymin);
							yn=((priv->bounds.ymax)-(priv->rescale.ymax))*s;
							yx+=(priv->bounds.ymax);
							draw_circ_update_scale(wgt, xn, xx, yn, yx);
						}
						else if (s<0)
						{
							yn=((priv->rescale.ymax)-(priv->bounds.ymin))*s;
							yn+=(priv->bounds.ymin);
							yn=((priv->rescale.ymin)-(priv->bounds.ymax))*s;
							yx+=(priv->bounds.ymax);
							draw_circ_update_scale(wgt, xn, xx, yn, yx);
						}
					}
				}
			}
		}
		else if (((circ->zmode)&DRAW_CIRC_ZOOM_OUT)==0)
		{
			xn=ZS*(priv->rescale.xmin);
			xx=xn;
			xn+=ZSC*(priv->bounds.xmin);
			xx+=ZSC*(priv->bounds.xmax);
			yn=ZS*(priv->rescale.ymin);
			yx=yn;
			yn+=ZSC*(priv->bounds.ymin);
			yx+=ZSC*(priv->bounds.ymax);
			draw_circ_update_scale(wgt, xn, xx, yn, yx);
		}
		else
		{
			xn=-UZC*(priv->rescale.xmin);
			xx=xn;
			xn+=UZ*(priv->bounds.xmin);
			xx+=UZ*(priv->bounds.xmax);
			yn=-UZC*(priv->rescale.ymin);
			yx=yn;
			yn+=UZ*(priv->bounds.ymin);
			yx+=UZ*(priv->bounds.ymax);
			draw_circ_update_scale(wgt, xn, xx, yn, yx);
		}
	}
	else if ((priv->flagr)==3)
	{
		if (circ->data)
		{
			(circ->hlp)=(circ->hlc);
			(circ->hlc)=-1;
			j=(circ->data->len);
			while (j>0)
			{
				ary=(((DrawCircGroup*) g_ptr_array_index((circ->data), --j))->xyr);
				for (k=0;k<(ary->len);k++)
				{
					dtm=(DrawCircData*) g_array_index(ary, DrawCircData*, k);
					xn=(dtm->x)-(circ->xps);
					yn=(dtm->y)-(circ->yps);
					s=(dtm->r);
					{xn*=xn; yn*=yn; s*=s;}
					if (xn+yn-s<=0)
					{
						(circ->hlc)=j;
						goto done;
					}
				}
			}
			done:
			if ((circ->hlc)!=(circ->hlp)) g_signal_emit(circ, draw_circ_signals[HLITE], 0);
		}
	}
	(priv->flagr)=0;
	return FALSE;
}

static void draw_circ_finalise(DrawCirc *circ)
{
	gint j;

	if (circ->data)
	{
		for (j=0;j<(circ->data->len);j++) g_array_free((((DrawCircGroup*) g_ptr_array_index((circ->data), j))->xyr), FALSE);
		g_ptr_array_free((circ->data), FALSE);
	}
	if (circ->rd) g_array_free((circ->rd), FALSE);
	if (circ->gr) g_array_free((circ->gr), FALSE);
	if (circ->bl) g_array_free((circ->bl), FALSE);
	if (circ->al) g_array_free((circ->al), FALSE);
}

static void draw_circ_set_property(GObject *obj, guint p_id, const GValue *val, GParamSpec *psp)
{
	DrawCircPrivate *priv;

	priv=DRAW_CIRC_GET_PRIVATE(obj);
	switch (p_id)
	{
		case PROP_BXN:
		{
			(priv->bounds.xmin)=g_value_get_double(val);
			break;
		}
		case PROP_BXX:
		{
			(priv->bounds.xmax)=g_value_get_double(val);
			break;
		}
		case PROP_BYN:
		{
			(priv->bounds.ymin)=g_value_get_double(val);
			break;
		}
		case PROP_BYX:
		{
			(priv->bounds.ymax)=g_value_get_double(val);
			break;
		}
		default:
		{
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, p_id, psp);
			break;
		}
	}
}

static void draw_circ_get_property(GObject *obj, guint p_id, GValue *val, GParamSpec *psp)
{
	DrawCircPrivate *priv;

	priv=DRAW_CIRC_GET_PRIVATE(obj);
	switch (p_id)
	{
		case PROP_BXN:
		{
			g_value_set_double(val, (priv->bounds.xmin));
			break;
		}
		case PROP_BXX:
		{
			g_value_set_double(val, (priv->bounds.xmax));
			break;
		}
		case PROP_BYN:
		{
			g_value_set_double(val, (priv->bounds.ymin));
			break;
		}
		case PROP_BYX:
		{
			g_value_set_double(val, (priv->bounds.ymax));
			break;
		}
		default:
		{
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, p_id, psp);
			break;
		}
	}
}

static gboolean draw_circ_expose(GtkWidget *wgt, GdkEventExpose *vnt)
{
	cairo_t *cr;

	cr=gdk_cairo_create(wgt->window);
	cairo_rectangle(cr, (vnt->area.x), (vnt->area.y), (vnt->area.width), (vnt->area.height));
	cairo_clip(cr);
	drwc(wgt, cr);
	drwz(wgt, cr);
	cairo_destroy(cr);
	return FALSE;
}

static void draw_circ_class_init(DrawCircClass *klass)
{
	GObjectClass *obj_klass;
	GtkWidgetClass *widget_klass;

	obj_klass=G_OBJECT_CLASS(klass);
	g_type_class_add_private(obj_klass, sizeof(DrawCircPrivate));
	(obj_klass->finalize)=(GObjectFinalizeFunc) draw_circ_finalise;
	(obj_klass->set_property)=draw_circ_set_property;
	(obj_klass->get_property)=draw_circ_get_property;
	g_object_class_install_property(obj_klass, PROP_BXN, g_param_spec_double("xmin", "Minimum x value", "Minimum value for the horizontal scale", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_BXX, g_param_spec_double("xmax", "Maximum x value", "Maximum value for the horizontal scale", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_BYN, g_param_spec_double("ymin", "Minimum y value", "Minimum value for the vertical scale", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_BYX, g_param_spec_double("ymax", "Maximum y value", "Maximum value for the vertical scale", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE));
	widget_klass=GTK_WIDGET_CLASS(klass);
	(widget_klass->button_press_event)=draw_circ_button_press;
	(widget_klass->motion_notify_event)=draw_circ_motion_notify;
	(widget_klass->button_release_event)=draw_circ_button_release;
	(widget_klass->expose_event)=draw_circ_expose;
	draw_circ_signals[MOVED]=g_signal_new("moved", G_OBJECT_CLASS_TYPE(obj_klass), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET (DrawCircClass, moved), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	draw_circ_signals[HLITE]=g_signal_new("highlight", G_OBJECT_CLASS_TYPE(obj_klass), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET (DrawCircClass, highlight), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void draw_circ_init(DrawCirc *circ)
{
	DrawCircPrivate *priv;
	gdouble val;

	gtk_widget_add_events(GTK_WIDGET(circ), GDK_BUTTON_PRESS_MASK|GDK_POINTER_MOTION_MASK|GDK_BUTTON_RELEASE_MASK);
	priv=DRAW_CIRC_GET_PRIVATE(circ);
	{(priv->bounds.xmin)=0; (priv->bounds.xmax)=1; (priv->bounds.ymin)=0; (priv->bounds.ymax)=1;}
	(priv->flagr)=0;
	(circ->data)=NULL;
	{(circ->zmode)=0; (circ->hlc)=-1; (circ->hlp)=-1;}
	{(circ->xps)=0; (circ->yps)=0;}
	(circ->rd)=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 12);
	(circ->gr)=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 12);
	(circ->bl)=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 12);
	(circ->al)=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 12);
	val=0.5;
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->al), val);
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->gr), val);
	g_array_append_val((circ->gr), val);
	val=1;
	g_array_append_val((circ->rd), val);
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->gr), val);
	val=0.5;
	g_array_append_val((circ->rd), val);
	g_array_append_val((circ->rd), val);
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->gr), val);
	g_array_append_val((circ->gr), val);
	val=0;
	g_array_append_val((circ->rd), val);
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->gr), val);
	val=0.5;
	g_array_append_val((circ->rd), val);
	g_array_append_val((circ->rd), val);
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->rd), val);
	val=1;
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->gr), val);
	g_array_append_val((circ->gr), val);
	g_array_append_val((circ->rd), val);
	g_array_append_val((circ->rd), val);
	val=0.5;
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->gr), val);
	g_array_append_val((circ->rd), val);
	val=1;
	g_array_append_val((circ->bl), val);
	val=0;
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->gr), val);
	g_array_append_val((circ->gr), val);
	g_array_append_val((circ->rd), val);
	g_array_append_val((circ->rd), val);
	val=0.5;
	g_array_append_val((circ->bl), val);
	g_array_append_val((circ->gr), val);
	val=0;
	g_array_append_val((circ->bl), val);
}

GtkWidget *draw_circ_new(void) {return g_object_new(DRAW_TYPE_CIRC, NULL);}
