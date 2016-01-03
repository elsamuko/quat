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

#include "common.h"
#include "qmath.h"
#include <math.h>

void q_mul( point c, const point a, const point b ) {
    c[0] = a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3]; /* e part */
    c[1] = a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2]; /* j part */
    c[2] = a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1]; /* k part */
    c[3] = a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0]; /* l part */
    return;
}

void q_add( point c, const point a, const point b ) {
    c[0] = a[0] + b[0];
    c[1] = a[1] + b[1];
    c[2] = a[2] + b[2];
    c[3] = a[3] + b[3];
    return;
}

void q_sub( point c, const point a, const point b ) {
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
    c[3] = a[3] - b[3];
    return;
}

void q_exp( point c, const point q ) {
    static double n, f, ex;
    n = sqrt( q[1] * q[1] + q[2] * q[2] + q[3] * q[3] );
    ex = exp( q[0] );
    f = ex * sin( n );

    if( n != 0.0 ) {
        f /= n;
    }

    c[0] = ex * cos( n );
    c[1] = f * q[1];
    c[2] = f * q[2];
    c[3] = f * q[3];
}

void q_log( point c, const point q ) {
    static double n, f;
    n = sqrt( q[1] * q[1] + q[2] * q[2] + q[3] * q[3] );

    if( n != 0.0 ) {
        f = atan2( n, q[0] );
        f /= n;
        c[0] = 0.5 * log( q[0] * q[0] + n * n );
        c[1] = f * q[1];
        c[2] = f * q[2];
        c[3] = f * q[3];
        return;
    } else { /* special case: real number */
        c[0] = 0.5 * log( q[0] * q[0] );
        c[1] = atan2( 0.0, q[0] );
        c[2] = 0.0;
        c[3] = 0.0;
        return;
        /* comment on asymmetry in i, j and k:
           e^(i*pi)=-1 && e^(j*pi)=-1 && e^(k*pi)=-1
           [ and even linear combinations of i, j and k give:
             e^(a*i+b*j+c*k) = -1 for every a, b, c E R with:
             sqrt(a^2+b^2+c^2) = pi  ]
           the above means that there is an infinite number of solutions to ln(-1).
           Practically, one must choose one of them and thus break symmetry.
           [ Even then there still is an infinite number of solutions due to
             periodicity of sin and cos... ] */
    }
}

void q_pow( point c, const point a, const point b ) {
    /* if a == zero: exp(-inf*b) = 0, if b>0;
       exp(-inf*b) = inf, if b<0
       if b isn't real, exp(-inf*b) isn't defined, because
       lim sin(x) for x->-inf (same with cos) doesn't exists */
    static double an, bnp;
    static point p;
    an = a[0] * a[0] + a[1] * a[1] + a[2] * a[2] + a[3] * a[3];
    bnp = b[1] * b[1] + b[2] * b[2] + b[3] * b[3];

    if( fabs( an ) < 1E-200 && ( b[0] > 0.0 || bnp != 0.0 ) ) {
        c[0] = 0.0;
        c[1] = 0.0;
        c[2] = 0.0;
        c[3] = 0.0;
        return;
    }

    q_log( c, a );
    q_mul( p, c, b );
    q_exp( c, p );
    return;
}
