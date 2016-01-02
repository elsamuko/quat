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

#include <new>
//#include <iostream>

#ifndef NO_NAMESPACE
using namespace std;
#endif

#include "ImageWid.h"

const unsigned long ImageWid::TYPE = 0x0a0b0c0d;

ImageWid::ImageWid(int x, int y, int w, int h, const char *label)
	: Fl_Widget(x, y, w, h, label), _data(0),
/*	_damage_x(0), _damage_y(0), _damage_w(0), _damage_h(0),
	_damaged(false),*/
	_type(TYPE)
{
	_data = new (nothrow) unsigned char[w*h*3];
}

ImageWid::~ImageWid()
{
//	is (nothrow) used with new, then the following must be called
//	to free the memory.
	operator delete[] (_data, nothrow);
}

bool ImageWid::newImage(int w, int h)
{
	operator delete[] (_data, nothrow);
	_data = new (nothrow) unsigned char[w*h*3];
	assert (_data != 0);
	memset(_data, 0, w*h*3);
	Fl_Widget::size(w, h);
	return true;
}

void ImageWid::white()
{
	memset(_data, 255, w()*h()*3);
	redraw();
}

/*
void ImageWid::_damage(int x, int y, int w, int h)
{
	damage(1);
	if (!_damaged) {
		_damage_x = x; _damage_y = y;
		_damage_w = w; _damage_h = h;
		_damaged = true;
		return;
	}

	int x2 = x + w, _damage_x2 = _damage_x + _damage_w;
	int y2 = y + h, _damage_y2 = _damage_y + _damage_h;
	int xn = (x < _damage_x) ? x : _damage_x;
	int xn2 = (x2 > _damage_x2) ? x2 : _damage_x2;
	int yn = (y < _damage_y) ? y : _damage_y;
	int yn2 = (y2 > _damage_y2) ? y2 : _damage_y2;
	_damage_x = xn;
	_damage_y = yn;
	_damage_w = xn2 - xn;
	_damage_h = yn2 - yn;

	// clipping
//	assert(_damage_x>=0);
//	assert(_damage_y>=0);
//	assert(_damage_w<=w());
//	assert(_damage_h<=h());
}
*/

void ImageWid::set_pixel(int _x, int _y, unsigned char r, unsigned char g, unsigned char b)
{
	_data[3*(w()*_y+_x)]   = r;
	_data[3*(w()*_y+_x)+1] = g;
	_data[3*(w()*_y+_x)+2] = b;
	damage(1, x()+_x, y()+_y, 1, 1);
}

void ImageWid::set_area(int x1, int x2, int _y, unsigned char *d)
{
	memcpy(_data+3*(w()*_y+x1), d, 3*(x2-x1+1));
	damage(1, x()+x1, y()+_y, x2-x1+1, 1);
}

void ImageWid::set_line(int n, unsigned char *d)
{
	memcpy(_data+3*w()*n, d, 3*w());
	damage(1, x(), y()+n, w(), 1);
}

void ImageWid::get_line(int n, unsigned char *d) const
{
	memcpy(d, _data+3*w()*n, 3*w());
}

/*
unsigned char *ImageWid::line(int n) const
{
	return _data+3*w()*n;
}
*/

void ImageWid::draw()
{
//	int _x = 0, _y = 0, _w = w(), _h = h();
//	if (damage()==1) {
//		_x = _damage_x; _y = _damage_y; _w = _damage_w; _h = _damage_h;
//	}
//cout << "draw: " << (int)damage() << ": " << _x << ", " << _y << ", " << _w << ", " << _h << endl;
//	fl_draw_image(_data+3*(_y*w()+_x), x()+_x, y()+_y, _w, _h, 3, w()-_w);
//	_damage_x = 0; _damage_y = 0; _damage_w = 0; _damage_h = 0;
//	_damaged = false;
	fl_draw_image(_data, x(), y(), w(), h());
}

int ImageWid::handle(int event)
{
	switch (event) {
		case FL_ENTER:
		case FL_LEAVE:
			return 1;
		default:
			return Fl_Widget::handle(event);
	}
}
