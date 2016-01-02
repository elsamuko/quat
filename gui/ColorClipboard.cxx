/* Quat - A 3D fractal generation program */
/* Copyright (C) 1997-2000 Dirk Meyer */
/* (email: dirk.meyer@studserv.uni-stuttgart.de) */
/* mail:  Dirk Meyer */
/*        Marbacher Weg 29 */
/*        D-71334 Waiblingen */
/*        Germany */
/* */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License */
/* as published by the Free Software Foundation; either version 2 */
/* of the License, or (at your option) any later version. */
/* */
/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */
/* */
/* You should have received a copy of the GNU General Public License */
/* along with this program; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <FL/fl_draw.H>

#include "ColorClipboard.h"

void ColorClipboard::draw()
{
	draw_box();
	fl_rectf(x()+2, y()+2, w()-4, h()-4,
		static_cast<int>(_r*255.0), static_cast<int>(_g*255.0), static_cast<int>(_b*255.0));
}

int ColorClipboard::handle(int event)
{
//	return Fl_Widget::handle(event);
	switch (event) {
		case FL_ENTER:
		case FL_LEAVE:
			return 1;
		default:
			return Fl_Widget::handle(event);
	}
}
