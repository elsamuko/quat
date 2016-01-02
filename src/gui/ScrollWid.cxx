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

#include <FL/Fl.H>	// Fl::flush()
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

//#include <iostream>
//using namespace std;

#include "ScrollWid.h"

const int ScrollWid::_scrollSize = 17;
const unsigned long ScrollWid::TYPE = 0x0a0b0c0e;

void ScrollWid::hscroll_cb(Fl_Widget *w, void *)
{
	ScrollWid *p = reinterpret_cast<ScrollWid*>(w->parent());
	Fl_Scrollbar *s = reinterpret_cast<Fl_Scrollbar*>(w);
	p->posX(s->value());
}

void ScrollWid::vscroll_cb(Fl_Widget *w, void *)
{
	ScrollWid *p = reinterpret_cast<ScrollWid*>(w->parent());
	Fl_Scrollbar *s = reinterpret_cast<Fl_Scrollbar*>(w);
	p->posY(s->value());	
}

ScrollWid::ScrollWid(int x, int y, int w, int h, const char *label)
	: Fl_Group(x, y, w, h, label), _effw(w), _effh(h), _widget(0), _type(TYPE)
//	HBar(0,0,100,_scrollSize), VBar(0,0,_scrollSize,100)
{
	HBar = new Fl_Scrollbar(0, h-_scrollSize, w, _scrollSize); HBar->hide();
	HBar->type(FL_HORIZONTAL);
	VBar = new Fl_Scrollbar(w-_scrollSize, 0, _scrollSize, h); VBar->hide();
	end();
	HBar->callback(hscroll_cb);
	VBar->callback(vscroll_cb);
	resizable(0);
	// DON'T delete children in destructor (or elsewhere) 
	// it's automatically deleted by Fl_Group}
}

ScrollWid::~ScrollWid()
{
	if (_widget != 0) {
		remove(*_widget);
		delete _widget;
	}
}

void ScrollWid::widget(Fl_Widget *wd)
{
	if (_widget != 0) {
		remove(*_widget);
		delete _widget;
	}
	_widget = wd; insert(*wd, 0);
	_widget->position(x(), y());   // first approximation of widget pos.
	resize(x(), y(), w(), h());    // get widget position correctly
	damage(FL_DAMAGE_ALL, x(), y(), w(), h());
	Fl::flush();	// This wasn't neccessary before 1.1.1/Windows
}

void ScrollWid::draw()
{
	if (damage() & (FL_DAMAGE_ALL | 1)) {
		fl_push_clip(x(), y(), _effw, _effh);
		draw_child(*_widget);
		fl_pop_clip();
	}

	if (!(damage() & FL_DAMAGE_ALL)) return; 

	fl_push_clip(x(), y(), w(), h());
	fl_color(color());
	// erase upper & lower space (if img not tall enough)
	if (_widget->h()<_effh) {
		fl_rectf(x(), y(), _effw, _widget->y()-y());
		fl_rectf(x(), _widget->y()+_widget->h(), _effw, 
			y()+_effh-_widget->y()-_widget->h());
	}
	// erase left & right space completely (if img too tall)
	if ((_widget->w()<_effw) && (_widget->h()>=_effh)) {
		fl_rectf(x(), y(), _widget->x()-x(), _effh);
		fl_rectf(_widget->x()+_widget->w(), y(), 
			x()+_effw-_widget->x()-_widget->w(), _effh);
	}
	// erase left & right space next to image only (if img not tall enough)
	if ((_widget->w()<_effw) && (_widget->h()<_effh)) {
		fl_rectf(x(), _widget->y(), _widget->x()-x(), _widget->h());
		fl_rectf(_widget->x()+_widget->w(), _widget->y(), 
			x()+_effw-_widget->x()-_widget->w(), _widget->h());
	}

	if (HBar->visible()) draw_child(*HBar);
	if (VBar->visible()) draw_child(*VBar);

	// erase small lower right rectangle between scollbars
	if (VBar->visible() && HBar->visible())
	{
		fl_color(color());
		fl_rectf(VBar->x(), HBar->y(), VBar->w(), HBar->h());
	}
	fl_pop_clip();
}

void ScrollWid::resize(int x, int y, int w, int h)
{
	bool _needVBar = (h<_widget->h()) || ( (h<_widget->h()+HBar->h()) 
		&& (w<_widget->w()) );
	bool _needHBar = (w<_widget->w()) || ( (w<_widget->w()+VBar->w()) 
		&& _needVBar);

	if (_needVBar) _effw = w - VBar->w();
	else _effw = w;
	if (_needHBar) _effh = h - HBar->h();
	else _effh = h;
	if (_needVBar)
	{
		int height = h; if (_needHBar) height -= HBar->h();
		VBar->resize(x+w-VBar->w(), y, VBar->w(), height);
		VBar->value(VBar->value(), _effh, 0, _widget->h());
	}
	else
	{
		_widget->position(_widget->x(), y+(_effh-_widget->h())/2);
	}
	if (_needHBar)
	{
		int width = w; if (_needVBar) width -= VBar->w();
		HBar->resize(x, y+h-HBar->h(), width, HBar->h());
		HBar->value(HBar->value(), _effw, 0, _widget->w());
	}
	else
	{
		_widget->position(x+(_effw-_widget->w())/2, _widget->y());
	}
	if (_needVBar && !VBar->visible())
	{
		VBar->show(); posY(0, false);
	}
	else if (!_needVBar && VBar->visible())
	{
		VBar->hide(); // posY(0, false);
	}
	if (_needHBar && !HBar->visible())
	{
		HBar->show(); posX(0, false);
	}
	else if (!_needHBar && HBar->visible())
	{
		HBar->hide(); // posX(0, false);
	}

	if (_widget->x() < x && -_widget->x()+x+_effw > _widget->w())
		posX(_widget->w()-_effw);
	if (_widget->y() < y && -_widget->y()+y+_effh > _widget->h())
		posY(_widget->h()-_effh);

	Fl_Group::resize(x, y, w, h);
}

void ScrollWid::posX(int _x, bool do_redraw)
{
	if (HBar->visible() && HBar->value() != _x) 
		HBar->value(_x, _effw, 0, _widget->w());
	_widget->position(-_x+x(), _widget->y());
	if (!do_redraw) return;
	redraw();
}

void ScrollWid::posY(int _y, bool do_redraw)
{
	if (VBar->visible() && VBar->value() != _y) 
		VBar->value(_y, _effh, 0, _widget->h());
	_widget->position(_widget->x(), -_y+y());
	if (!do_redraw) return;
	redraw();
}

