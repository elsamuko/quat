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

#include <iostream>

#ifndef NO_NAMESPACE
using namespace std;
#endif

#include "structs.h"
#include "quat.h"

void frac_cpp::reset() {
    view_struct v;
    realpal_struct r;
    char col[251];
    double cut[140];

    SetDefaults( this, &v, &r, col, cut );
    //	*this = f;
}

frac_cpp frac_cpp::operator = ( frac_struct& v ) {
    memcpy( this, &v, sizeof( frac_struct ) );
    return *this;
}

void frac_cpp::print() {
    cout << "(" << c[0] << "," << c[1] << "," << c[2] << "," << c[3] << "), "
         << bailout << ", " << maxiter << ", " << lvalue << ", " << formula << endl;

    for( int j = 0; j < 4; ++j ) {
        cout << "Parameter " << j << ": ";

        for( int i = 0; i < 4; ++i ) {
            cout << p[j][i] << "  ";
        }

        cout << endl;
    }
}

void view_cpp::reset() {
    frac_struct f;
    realpal_struct r;
    char col[251];
    double cut[140];

    SetDefaults( &f, this, &r, col, cut );
    //	*this = v;
}

view_cpp view_cpp::operator = ( view_struct& v ) {
    memcpy( this, &v, sizeof( view_struct ) );
    return *this;
}

void view_cpp::print() {
    cout << "(" << s[0] << "," << s[1] << "," << s[2] << "), "
         << "(" << up[0] << "," << up[1] << "," << up[2] << "), "
         << "(" << light[0] << "," << light[1] << "," << light[2] << "), "
         << "(" << Mov[0] << "," << Mov[1] << "), "
         << LXR << ", "
         << "(" << xres << "," << yres << "," << zres << "), "
         << interocular << ", " << phongmax << ", " << phongsharp << ", "
         << ambient << ", " << antialiasing << endl;
}

void realpal_cpp::reset() {
    frac_struct f;
    view_struct v;
    char col[251];
    double cut[140];

    SetDefaults( &f, &v, this, col, cut );
    //	*this = v;
}

realpal_cpp realpal_cpp::operator = ( realpal_struct& v ) {
    memcpy( this, &v, sizeof( realpal_struct ) );
    return *this;
}

void realpal_cpp::print() {
    cout << colnum << endl;

    for( int i = 0; i < colnum; i++ )
        cout << "(" << cols[i].col1[0] << "," << cols[i].col1[1] << "," << cols[i].col1[2] << "), "
             << "(" << cols[i].col2[0] << "," << cols[i].col2[1] << "," << cols[i].col2[2] << ")"
             << endl;
}

void colscheme_cpp::reset() {
    frac_struct f;
    view_struct v;
    realpal_struct r;
    double cut[140];

    SetDefaults( &f, &v, &r, _data, cut );
    //	*this = v;
}

void cutbuf_cpp::reset() {
    frac_struct f;
    view_struct v;
    realpal_struct r;
    char col[251];

    SetDefaults( &f, &v, &r, col, _data );
    //	num = getnum();
}

void cutbuf_cpp::print() {
    int num = getnum();

    //	int idx = 0;
    //	while (*reinterpret_cast<char *>(_data+idx) != CUT_TERMINATOR && idx<20)
    for( int idx = 0; idx < num; idx++ ) {
        cout << "CUT_PLANE (" << _data[idx + 1] << "," << _data[idx + 2] << "," << _data[idx + 3] << "), ("
             << _data[idx + 4] << "," << _data[idx + 5] << "," << _data[idx + 6] << ")" << endl;
        idx++;
    }

    if( num == 0 ) {
        cout << "empty." << endl;
    }

}

int cutbuf_cpp::getnum() {
    int tnum = 0, idx = 0;

    while( tnum < 20 && *reinterpret_cast<char*>( _data + idx ) != CUT_TERMINATOR )
        switch( *reinterpret_cast<char*>( _data + idx ) ) {
            case CUT_PLANE:
                ++tnum;
                idx += 7;
                break;

            default:
                idx++;
        }

    return tnum;
}

void cutbuf_cpp::add() {
    int num = getnum();

    if( num >= 20 ) {
        return;
    }

    if( num < 19 ) {
        *reinterpret_cast<char*>( &_data[7 * ( num + 1 )] ) = CUT_TERMINATOR;
    }

    *reinterpret_cast<char*>( &_data[7 * num] ) = CUT_PLANE;
    num++;
}

void cutbuf_cpp::del( int idx ) {
    int num = getnum();

    if( num == 0 ) {
        return;
    }

    if( idx > 19 ) {
        return;
    }

    memmove( &_data[7 * idx], &_data[7 * idx + 7], ( 140 - 7 * ( idx + 1 ) )*sizeof( double ) );

    if( num == 20 ) {
        *reinterpret_cast<char*>( &_data[133] ) = CUT_TERMINATOR;
        _data[134] = 0.0;
        _data[135] = 0.0;
        _data[136] = 0.0;
        _data[137] = 0.0;
        _data[138] = 0.0;
        _data[139] = 0.0;
    }

    //for (int i=0; i<20; ++i)
    //	cout << (int)*reinterpret_cast<char*>(&_data[7*i]) << ": "
    //		<< _data[7*i+1] << ", " << _data[7*i+2] << ", " << _data[7*i+3] << ", "
    //		<< _data[7*i+4] << ", " << _data[7*i+5] << ", " << _data[7*i+6] << endl;
}
