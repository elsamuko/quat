/* Quat - A 3D fractal generation program */
/* Copyright (C) 2000 Dirk Meyer */
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

#include <cstring>

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "ViewSelector.h"
#include "iter.h"

ViewSelector::ViewSelector(int x, int y, int w, int h, const char *label)
	: Fl_Double_Window(x, y, w, h, label), _state(0)
{
	end();
	memset(&_v, 0, sizeof(_v));
}

ViewSelector::~ViewSelector()
{
}

void ViewSelector::draw()
{
	fl_color(FL_WHITE);
	fl_rectf(0,0,w(),h());
	CalcCoo();
	return;
}

int ViewSelector::handle(int event)
{
//	int ret=Fl_Double_Window::handle(event);
	Fl_Double_Window::handle(event);
	switch (event)
	{
		int xm; int ym;
		case FL_ENTER:
		case FL_LEAVE:
			return 1;
		case FL_PUSH:
			xm = Fl::event_x(); ym = Fl::event_y();
			if (_vx-3<xm && xm<_vx+3 && _vy-3<ym && ym<_vy+3)
			{
				_state = 1;
				redraw();
			}
			return 1;
			break;
		case FL_DRAG:
			if (_state != 1) return 0;
			xm = Fl::event_x(); ym = Fl::event_y();
			if (xm<0) xm = 0;
			if (ym<0) ym = 0;
			if (xm>w()-1) xm = w()-1;
			if (ym>h()-1) ym = h()-1;
			xm -= static_cast<int>(w()/2);
			ym -= static_cast<int>(h()/2);
			if (_dx<0) xm *= -1;
			if (_dy<0) ym *= -1;
			if (_input_vx != 0) _input_vx->value(xm/10.0); _input_vx->do_callback();
			if (_input_vy != 0) _input_vy->value(ym/10.0); _input_vy->do_callback();
			return 1;
			break;
		case FL_RELEASE:
			_state = 0;
			redraw();
			return 1;
			break;
		default: return Fl_Double_Window::handle(event);
	}
//	return ret;	// unreachable
}

void ViewSelector::Init(int s_x, int s_y, int s_dx, int s_dy)
{
	_x = s_x; _y = s_y; _dx = s_dx; _dy = s_dy;
	return;
}

void ViewSelector::CalcCoo()
{
	struct basestruct base, sbase;
	int pre = 0, pi = 0, pj = 0;
	int plane_x[4] = { 0, 0, 0, 0}, plane_y[4] = { 0, 0, 0, 0};
	int posx = 0, posy = 0, arrowadd = 0;

	memset(&base, 0, sizeof(basestruct));
	memset(&sbase, 0, sizeof(basestruct));
	/* Draw coordinate system */
	fl_color(FL_BLACK);
	/* x axis */
	posy = h()/2;
	if (_dx>0)
	{
		posx = static_cast<int>((w()/2)+20); arrowadd = -6;
	}	
	else
	{
		posx = static_cast<int>((w()/2)-20); arrowadd = 6;
	}
	fl_line(w()/2, h()/2, posx, posy);
	fl_line(posx, posy, posx+arrowadd, posy-3);
	fl_line(posx, posy, posx+arrowadd, posy+3);
	posy += 10;
	fl_font(FL_COURIER, 10);
	switch (_x)
	{
		case 0: fl_draw("1", posx-arrowadd, posy); break;
		case 1: fl_draw("i", posx-arrowadd, posy); break;
		case 2: fl_draw("j", posx-arrowadd, posy); break;
	}
	/* y axis */
	posx = static_cast<int>(w()/2);
	if (_dy>0)
	{
		posy = static_cast<int>((h()/2)+20); arrowadd = -6;
	}
	else
	{
		posy = static_cast<int>((h()/2)-20); arrowadd = 6;
	}
	fl_line(w()/2, h()/2, posx, posy);
	fl_line(posx, posy, posx-3, posy+arrowadd);
	fl_line(posx, posy, posx+3, posy+arrowadd);
	posy += 10;
	if (_dy>0) posy = static_cast<int>(h()/2+20);
	switch (_y)
	{
		case 0: fl_draw("1", posx + 6, posy); break;
		case 1: fl_draw("i", posx + 6, posy); break;
		case 2: fl_draw("j", posx + 6, posy); break;
	}
	/* Draw viewpoint and view plane */
	pre = static_cast<int>(_v.s[0]*10.0);
	pi = static_cast<int>(_v.s[1]*10.0);
	pj = static_cast<int>(_v.s[2]*10.0);
	if (_x==0) _vx = pre;
	if (_x==1) _vx = pi;
	if (_x==2) _vx = pj;
	if (_y==0) _vy = pre;
	if (_y==1) _vy = pi;
	if (_y==2) _vy = pj;
	if (_dx<0) _vx *= -1;
	if (_dy<0) _vy *= -1;
	_vx += static_cast<int>(w()/2); _vy += static_cast<int>(h()/2);
	if (_state == 0) fl_color(FL_BLUE); else fl_color(FL_RED);
	fl_rect(_vx-2, _vy-2, 5, 5);
	if (!calcbase(&base, 0, _v, 0) && (_v.xres != 0) )
	{
		/* Calculate bitmap coordinates of view rectangle */
		plane_x[0] = static_cast<int>(base.O[_x]*10.0);
		plane_x[1] = static_cast<int>((base.O[_x]+_v.LXR*base.x[_x])*10.0);
		plane_x[2] = static_cast<int>((base.O[_x]+_v.LXR*base.x[_x]
			+_v.LXR*(static_cast<double>(_v.yres)/static_cast<double>(_v.xres))*base.y[_x])*10.0);
		plane_x[3] = static_cast<int>((base.O[_x]
			+_v.LXR*(static_cast<double>(_v.yres)/static_cast<double>(_v.xres))*base.y[_x])*10.0);
		plane_y[0] = static_cast<int>(base.O[_y]*10.0);
		plane_y[1] = static_cast<int>((base.O[_y]+_v.LXR*base.x[_y])*10.0);
		plane_y[2] = static_cast<int>((base.O[_y]+_v.LXR*base.x[_y]
			+_v.LXR*(static_cast<double>(_v.yres)/static_cast<double>(_v.xres))*base.y[_y])*10.0);
		plane_y[3] = static_cast<int>((base.O[_y]
			+_v.LXR*(static_cast<double>(_v.yres)/static_cast<double>(_v.xres))*base.y[_y])*10.0);
		/* adjust to orientation of axes */
		if (_dx<0)
			for (int i=0; i<4; i++) plane_x[i] *= -1;
		if (_dy<0)
			for (int i=0; i<4; i++) plane_y[i] *= -1;
		for (int i=0; i<4; i++)
		{
			plane_x[i] += static_cast<int>(w()/2);
			plane_y[i] += static_cast<int>(h()/2);
		}
		fl_color(FL_GREEN);
		fl_loop(plane_x[0], plane_y[0], plane_x[1], plane_y[1],
			plane_x[2], plane_y[2], plane_x[3], plane_y[3]);
		fl_color(FL_CYAN);
		fl_circle(plane_x[0], plane_y[0], 1);
	}
	else
	{
		fl_color(FL_BLUE);
		fl_draw("not def.", 1, 16);
	}
	fl_color(FL_BLACK);
	fl_rect(0, 0, w(), h());
}
