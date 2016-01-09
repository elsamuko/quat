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

#include "JuliaPreview.h"
#include "quat.h"
#include "MainWindow.h"

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>

#include <cassert>
#include <cmath>
#include <ctime>

extern "C" VER_ReturnVideoInfo ReturnVideoInfo;
extern "C" VER_SetColors SetColors;
extern "C" VER_Initialize Initialize;
extern "C" VER_Done Done;
extern "C" VER_update_bitmap update_bitmap;
extern "C" VER_getline QU_getline;
extern "C" VER_putline putline;
extern "C" VER_check_event check_event;
extern "C" VER_Change_Name Change_Name;
extern "C" VER_Debug Debug;
extern "C" VER_eol eol;

extern "C" time_t calc_time;
extern time_t old_time;

JuliaPreview* p_this = NULL;  // Quite nasty interface to C...

int JuliaPreview::FLTK_putline2( long x1, long x2, long, int y, unsigned char* Buf, int ) {
    p_this->set_area( x1, x2, y, Buf + x1 * 3 );
    return 0;
}

void JuliaPreview::FLTK_eol2( int ) {
}

JuliaPreview::JuliaPreview( int X, int Y, int W, int H, const char* label )
    : ImageWid( X, Y, W, H, label ), _updated( false ), _stereo( false ), _ownView( false ),
      _imagestereo( false ), _pic_ownView( false ) {
    // Well, only one instance of JuliaPreview can be created ...
    assert( p_this == NULL );

    white();
    p_this = this;
}

JuliaPreview::~JuliaPreview() {
    p_this = NULL;
}

void JuliaPreview::CalcImage3D() {
    //	QWidget *w = topLevelWidget();
    int xs = 0, ys = 0;
    char Error[1024];

    //	Image = new QImage(len_x, len_y, 32, 0, QImage::IgnoreEndian);
    //	*Error = 0;
    ReturnVideoInfo = ( VER_ReturnVideoInfo )MainWindow::FLTK_ReturnVideoInfo;
    SetColors = ( VER_SetColors )MainWindow::FLTK_SetColors;
    Initialize = DUMMY_Initialize;
    Done = DUMMY_Done;
    update_bitmap = ( VER_update_bitmap )DUMMY_update_bitmap;
    QU_getline = DUMMY_getline;
    putline = ( VER_putline )FLTK_putline2;
    check_event = ( VER_check_event )MainWindow::FLTK_check_event;
    Change_Name = DUMMY_Change_Name;
    Debug = ( VER_Debug )MainWindow::FLTK_Debug;
    eol = ( VER_eol )FLTK_eol2;
    _calc_f.reset();
    _calc_v.reset();
    _calc_pal.reset();
    _calc_col.reset();
    _calc_cut.reset();

    for( int j = 0; j < 4; ++j )
        for( int i = 0; i < 4; ++i ) {
            _calc_f.p[j][i] = _f.p[j][i];
        }

    _calc_v.LXR = 4.0;
    _calc_pal.cols[0].col1[0] = 1.0;
    _calc_pal.cols[0].col1[1] = 1.0;
    _calc_pal.cols[0].col1[2] = 0.0;

    if( _ownView ) {
        _calc_v = _v;
        _calc_col = _col;
        _calc_cut = _cut;
        _calc_pal = _pal;
    }

    _calc_f.bailout = _f.bailout;
    _calc_f.maxiter = _f.maxiter;
    _calc_f.lvalue = _f.lvalue;
    _calc_f.formula = _f.formula;

    if( !_stereo ) {
        _calc_v.interocular = 0.0;
    }

    _calc_f.c[0] = _f.c[0];
    _calc_f.c[1] = _f.c[1];
    _calc_f.c[2] = _f.c[2];
    _calc_f.c[3] = _f.c[3];
    _calc_v.xres = w();

    if( _stereo ) {
        _calc_v.xres /= 2;
    }

    _calc_v.yres = h();
    _calc_v.zres = 60;
    _calc_v.antialiasing = 1;

    //	w->setCursor(waitCursor);
    button3d->deactivate();

    bool oldstereo = _imagestereo;
    _imagestereo = IsStereo( &_calc_v );
    old_time = calc_time;
    CreateImage( Error, &xs, &ys, &_calc_f, &_calc_v, &_calc_pal,
                 _calc_col, _calc_cut, 80, 0 );
    calc_time = old_time;

    if( Error[0] != 0 ) {
        _imagestereo = oldstereo;
        fl_alert( "%s", Error );
    }

    _pic_ownView = _ownView;
    _pic_f = _calc_f;
    _pic_v = _calc_v;
    _pic_pal = _calc_pal;
    _pic_col = _calc_col;
    _pic_cut = _calc_cut;

    button3d->activate();
    //	w->setCursor(arrowCursor);
    _updated = true;
}

