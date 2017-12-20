/* Quat - A 3D fractal generation program */
/* Copyright (C) 1997,98 Dirk Meyer */
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

#include <math.h>   /* sqrt */
#include "common.h"
#include "colors.h"
#include <stdlib.h> /* labs */

int GetTrueColor( struct realpal_struct* rp, double color, double* r, double* g, double* b );
int DimAndGammaTrueColor( double RR, double GG, double BB, double* R, double* G, double* B,
                          double rgam, double ggam, double bgam, double bright );

double weightsum;
const double GAMMA = 0.7;

int GetTrueColor( struct realpal_struct* rp, double color, double* r, double* g, double* b ) {
    int i;
    double nc;

    nc = 0;

    if( color == 1 ) {
        color -= 0.000000000001;
    }

    for( i = 0; ( ( nc < color ) && ( i < rp->colnum ) ); i++ ) {
        nc += rp->cols[i].weight / weightsum;
    }

    if( nc > color ) {
        i -= 1;
        nc -= rp->cols[i].weight / weightsum;
    }

    nc = ( color - nc ) / ( rp->cols[i].weight / weightsum );
    *r = nc * ( rp->cols[i].col2[0] - rp->cols[i].col1[0] ) + rp->cols[i].col1[0];
    *g = nc * ( rp->cols[i].col2[1] - rp->cols[i].col1[1] ) + rp->cols[i].col1[1];
    *b = nc * ( rp->cols[i].col2[2] - rp->cols[i].col1[2] ) + rp->cols[i].col1[2];
    return( 0 );
}

int DimAndGammaTrueColor( double RR, double GG, double BB, double* R, double* G, double* B,
                          double rgam, double ggam, double bgam, double bright ) {
    double dr, dg, db;
    double dbright;

    if( bright > 1 ) { /* this is to get white as maximum brightness (for phong) bright=2 is white */
        dr = 1 - RR;
        dg = 1 - GG;
        db = 1 - BB;
        dbright = bright - 1;
        *R = pow( RR + dbright * dr, rgam );
        *G = pow( GG + dbright * dg, ggam );
        *B = pow( BB + dbright * db, bgam );
    } else {
        *R = pow( bright * RR, rgam );
        *G = pow( bright * GG, ggam );
        *B = pow( bright * BB, bgam );
    }

    return( 0 );
}

int FindNearestColor( struct disppal_struct* disppal, unsigned char r, unsigned char g, unsigned char b ) {
    long dist1, dist, mindist;
    int i, index = 0, rshift, gshift, bshift;

    mindist = 200000;
    rshift = 8 - disppal->rdepth;
    gshift = 8 - disppal->gdepth;
    bshift = 8 - disppal->bdepth;
    dist = 1;

    for( i = 0; ( i < disppal->maxcol ) && ( dist != 0 ); i++ ) {
        dist = 0;
        dist1 = ( disppal->cols[i].r << rshift ) - r;
        dist += labs( dist1 ); /*pow(fabs(dist1),0.6);*/
        dist1 = ( disppal->cols[i].g << gshift ) - g;
        dist += labs( dist1 ); /*pow(fabs(dist1),0.8)*/;
        dist1 = ( disppal->cols[i].b << bshift ) - b;
        dist += labs( dist1 ); /*pow(fabs(dist1),0.6); */

        if( dist < mindist ) {
            mindist = dist;
            index = i;
        }
    }

    return( index );
}

int CalcWeightsum( struct realpal_struct* realpal ) {
    int i;

    /* Sum the weights in realpal */
    weightsum = 0;

    for( i = 0; i < realpal->colnum; i++ ) {
        weightsum += realpal->cols[i].weight;
    }

    return( 0 );
}

int CreateDispPal( disppal, realpal, maxcol, phongmax, rdepth, gdepth, bdepth )
struct disppal_struct* disppal;
struct realpal_struct* realpal;
int maxcol, rdepth, gdepth, bdepth;
double phongmax;
{
    int i, j, requiredcols, maxdepth, maxbright, oswcols;
    int disppos;
    double weight;
    double r, g, b, rr, gg, bb;
    int rncol, gncol, bncol;

    /* how many colours did the user specify at least ? */
    requiredcols = 0;
    weight = realpal->cols[0].weight;
    oswcols = 1;

    for( i = 0; i < realpal->colnum; i++ ) {
        if( ( realpal->cols[i].col1[0] == realpal->cols[i].col2[0] )
                && ( realpal->cols[i].col1[1] == realpal->cols[i].col2[1] )
                && ( realpal->cols[i].col1[2] == realpal->cols[i].col2[2] ) ) {
            requiredcols++;

            if( weight != realpal->cols[i].weight ) {
                oswcols = 0;
            }
        } else {
            requiredcols += 2;
            oswcols = 0;
        }
    }

    /* how many brights fit into pal of 'maxcol' colors ? */
    disppal->brightnum = ( int )floor( sqrt( disppal->btoc * ( double )maxcol ) );
    /* what is the maximum color-depth of r,g,b ? */
    maxdepth = 0;

    if( rdepth > maxdepth ) {
        maxdepth = rdepth;
    }

    if( gdepth > maxdepth ) {
        maxdepth = gdepth;
    }

    if( bdepth > maxdepth ) {
        maxdepth = bdepth;
    }

    if( requiredcols && ( oswcols == 1 ) ) {
        disppal->brightnum = maxcol / requiredcols;
    }

    /* is it possible to achieve 'brightnum' brights ? */
    maxbright = 1 << maxdepth;

    if( maxbright < disppal->brightnum ) {
        disppal->brightnum = maxbright;    /* No. */
    }

    disppal->colnum = ( int )( ( double )maxcol / ( double )disppal->brightnum );

    if( ( oswcols == 1 ) && ( disppal->colnum > requiredcols ) ) {
        disppal->colnum = requiredcols;
    }

    /* It could all be so easy, if the user wasn't able to declare more */
    /* colours than we can display */
    if( disppal->colnum < requiredcols ) {
        return( -1 );
    }

    if( disppal->brightnum < 7 ) {
        return( -1 );
    }

    disppal->maxcol = disppal->colnum * disppal->brightnum;

    CalcWeightsum( realpal );

    rncol = 1 << rdepth; /* number of reds */
    gncol = 1 << gdepth; /* number of greens */
    bncol = 1 << bdepth; /* number of blues */

    disppos = 0;

    for( i = 0; i < disppal->colnum; i++ ) { /* Go through every colour */
        if( disppal->colnum == 1 ) {
            GetTrueColor( realpal, 0.5, &r, &g, &b );
        } else {
            GetTrueColor( realpal, ( double )i / ( double )( disppal->colnum - 1 ), &r, &g, &b );
        }

        for( j = 0; j < disppal->brightnum - 4; j++ ) { /* Go through every possible brightness */
            DimAndGammaTrueColor( r, g, b, &rr, &gg, &bb, GAMMA, GAMMA, GAMMA, ( double )j / ( double )( disppal->brightnum - 5 ) );
            disppal->cols[disppos].r = ( int )( ( double )( rncol - 1 ) * rr );
            disppal->cols[disppos].g = ( int )( ( double )( gncol - 1 ) * gg );
            disppal->cols[disppos].b = ( int )( ( double )( bncol - 1 ) * bb );
            disppos++;
        }

        for( j = 0; j < 4; j++ ) { /* create phong colours */
            DimAndGammaTrueColor( r, g, b, &rr, &gg, &bb, 1.0, 1.0, 1.0, 1.0 + phongmax * ( double )( j + 1 ) / 4.0 );
            disppal->cols[disppos].r = ( int )( ( double )( rncol - 1 ) * rr );
            disppal->cols[disppos].g = ( int )( ( double )( gncol - 1 ) * gg );
            disppal->cols[disppos].b = ( int )( ( double )( bncol - 1 ) * bb );
            disppos++;
        }
    }

    return( 0 );
}

int PixelvaluePalMode( int x1, int x2, int colmax, int brightmax, unsigned char* line, float* CBuf,
                       float* BBuf ) {
    int i;
    /* colmax+1 = colnum, brightmax dto.  */

    for( i = x1; i <= x2; i++ ) {
        if( BBuf[i] <= 1 ) {
            line[i] = ( int )floor( ( double )colmax * CBuf[i] ) * ( brightmax + 1 ) + ( int )floor( ( double )( brightmax - 4 ) * BBuf[i] );
        } else {
            line[i] = ( int )floor( ( double )( colmax ) * CBuf[i] ) * ( brightmax + 1 ) + brightmax - 4 + ( int )floor( 4.0 * ( BBuf[i] - 1.0 ) );
        }
    }

    return( 0 );
}

int PixelvalueTrueMode( int x1, int x2, int rmax, int gmax, int bmax,
                        struct realpal_struct* realpal, unsigned char* line, float* CBuf, float* BBuf ) {
    int i;
    double r, g, b;
    int ir, ig, ib;

    for( i = x1; i <= x2; i++ ) {
        if( BBuf[i] > 0.0001 ) {
            GetTrueColor( realpal, CBuf[i], &r, &g, &b );
            DimAndGammaTrueColor( r, g, b, &r, &g, &b, GAMMA, GAMMA, GAMMA, BBuf[i] );
            ir = ( int )floor( r * ( double )rmax );
            ig = ( int )floor( g * ( double )gmax );
            ib = ( int )floor( b * ( double )bmax );
            line[3 * i] = ir;
            line[3 * i + 1] = ig;
            line[3 * i + 2] = ib;
        } else {
            line[3 * i] = 0;
            line[3 * i + 1] = 0;
            line[3 * i + 2] = 0;
        }
    }

    return( 0 );
}


