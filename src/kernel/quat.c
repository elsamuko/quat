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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>  /* fmod */
#include <time.h> /* time */
#include <ctype.h> /* tolower */
#include "quat.h"  /* includes also common.h */
#include "iter.h"
#include "files.h"
#include "colors.h"
#include "calculat.h"

#ifndef GUI
unsigned char* TEXTVER_zbuf;
#endif

char orbite( double* a, const double* b );
char orbitj( double* a, const double* b );
char orbitk( double* a, const double* b );
char orbitl( double* a, const double* b );
int PrepareKeywords( char* colorscheme, struct frac_struct* f, struct view_struct* v );
int CopyIDAT( int copy, /*struct disppal_struct *disppal, struct vidinfo_struct *vidinfo,*/
              struct png_internal_struct* i, struct png_internal_struct* i2,
              unsigned char* Buf, int ystart, int zflag, int zres );
int InitGraphics( char* Error, struct view_struct* view, struct realpal_struct* realpal,
                  int ready, struct vidinfo_struct* vidinfo, struct disppal_struct* disppal,
                  int* xadd, int* yadd, int zbufflag );
int AllocBufs( struct view_struct* v, int zflag, int pal, float** CBuf, float** BBuf,
               float** LBufR, float** LBufL, unsigned char** line, unsigned char** line2,
               unsigned char** line3 );
int FreeBufs( float** a, float** g, float** b, float** c, unsigned char** d,
              unsigned char** e, unsigned char** f );
int LBuf_ZBuf( struct view_struct* v, float* LBuf, unsigned char* line, long ii,
               long imax, long aa_line, int offs );
int CalculateFractal( char* Error, char* pngfile, FILE** png, /* struct png_info_struct *png_info, */
                      struct png_internal_struct* png_internal, int zflag, int* xstart, int* ystart,
                      /*int xadd, int yadd,*/ int no, struct basestruct* rbase, struct basestruct* srbase,
                      struct basestruct* lbase, struct basestruct* slbase, struct frac_struct* f,
                      struct view_struct* v, struct realpal_struct* realpal,/* char *colscheme,*/ double* cutbuf,
                      struct vidinfo_struct* vidinfo, struct disppal_struct* disppal );
int CalculateINI( const char* inifile, const char* pngfile, char* Error, int zflag );
int CalculatePNG( const char* pngfile1, const char* pngfile2, char* Error, const char* ini, int zflag );

struct progtype prog;

struct parameter_struct Parameters[64];
struct keyword_struct Keywords[25];
time_t calc_time;

point* orbit = 0;

/* This variables (functions) have to be declared in the special version files ! */
/* use an extern statement like "extern VER_Done Done;" */
VER_ReturnVideoInfo ReturnVideoInfo = DUMMY_ReturnVideoInfo;
VER_SetColors SetColors = DUMMY_SetColors;
VER_Initialize Initialize = DUMMY_Initialize;
VER_Done Done = DUMMY_Done;
VER_update_bitmap update_bitmap = DUMMY_update_bitmap;
VER_getline QU_getline = DUMMY_getline;
VER_putline putline = DUMMY_putline;
VER_check_event check_event = DUMMY_check_event;
VER_Change_Name Change_Name = DUMMY_Change_Name;
VER_Debug Debug = DUMMY_Debug;
VER_eol eol = DUMMY_eol;
VER_update_parameters update_parameters = DUMMY_update_parameters;

char slash = '/';

/* DUMMY functions which do nothing / Available to all versions */
int DUMMY_ReturnVideoInfo( struct vidinfo_struct* v ) {
    return( 0 );
}
int DUMMY_SetColors( struct disppal_struct* d ) {
    return( 0 );
}
int DUMMY_Initialize( int x, int y, char* c ) {
    return( 0 );
}
int DUMMY_Done( void ) {
    return( 0 );
}
int DUMMY_update_bitmap( long a, long b, long c, int d, unsigned char* e, int f ) {
    return( 0 );
}
int DUMMY_getline( unsigned char* a, int b, long c, int d ) {
    return( 0 );
}
int DUMMY_putline( long a, long b, long c, int d, unsigned char* e, int f ) {
    return( 0 );
}
int DUMMY_check_event( void ) {
    return( 0 );
}
int DUMMY_Change_Name( char* a ) {
    return( 0 );
}
void DUMMY_Debug( char* a ) {
    return;
}
void DUMMY_eol( int a ) {
    return;
}
void DUMMY_update_parameters( struct frac_struct* a, struct view_struct* b,
                              struct realpal_struct* c, char* d, double* e ) {
    return;
}

void SetSlash( char s ) {
    slash = s;
    return;
}

void ConvertPath( char* Path ) {
    size_t i;

    for( i = 0; i < strlen( Path ); i++ )
        if( Path[i] == '/' || Path[i] == '\\' ) {
            Path[i] = slash;
        }
}

int PrepareKeywords( char* colorscheme, struct frac_struct* f, struct view_struct* v ) {
    int i, j;
    static char s[20];

    /* The parameters for the keywords */
    /* No semicolon behind STDDOUBLE, STDINT, they're macros */
    Parameters[0].type = T_STRING;
    Parameters[0].mem = VVPTR( *colorscheme );
    Parameters[0].max = 250;

    /* 3 * resolution */
    for( i = 1; i < 4; i++ ) {
        Parameters[i].type = T_INTEGER;
        Parameters[i].min = 1;
        Parameters[i].max = 100000;   /* old : 32767, 8192, 2048 */
    }

    Parameters[1].mem = VVPTR( v->xres );
    Parameters[2].mem = VVPTR( v->yres );
    Parameters[3].mem = VVPTR( v->zres );

    /* CE, CJ, CK, CL, bailout, lvalue */
    for( i = 4; i < 10; i++ )
        STDDOUBLE( Parameters, i )
        Parameters[4].mem = VVPTR( f->c[0] );

    Parameters[5].mem = VVPTR( f->c[1] );
    Parameters[6].mem = VVPTR( f->c[2] );
    Parameters[7].mem = VVPTR( f->c[3] );
    Parameters[8].mem = VVPTR( f->bailout );
    Parameters[8].min = 0.1;
    Parameters[8].max = 1E20;
    Parameters[9].mem = VVPTR( f->lvalue );

    /* maxiter, formula */
    for( i = 10; i < 12; i++ )
        STDINT( Parameters, i )
        Parameters[10].mem = VVPTR( f->maxiter );

    Parameters[10].min = 2;
    Parameters[11].mem = VVPTR( f->formula );
    Parameters[11].max = 4;

    /* 3 * viewpoint, 3 * up, 3 * light, 2 * mov, LXR */
    for( i = 12; i < 24; i++ )
        STDDOUBLE( Parameters, i )
        for( i = 0; i < 3; i++ ) {
            Parameters[12 + i].mem = VVPTR( v->s[i] );
            Parameters[15 + i].mem = VVPTR( v->up[i] );
            Parameters[18 + i].mem = VVPTR( v->light[i] );
        }

    Parameters[21].mem = VVPTR( v->Mov[0] );
    Parameters[22].mem = VVPTR( v->Mov[1] );
    Parameters[23].mem = VVPTR( v->LXR );
    Parameters[23].min = 0;

    /* phongmax */
    Parameters[24].type = T_DOUBLE;
    Parameters[24].min = 0.0;
    Parameters[24].max = 1.0;
    Parameters[24].mem = VVPTR( v->phongmax );
    /* phongsharp */
    STDDOUBLE( Parameters, 25 )
    Parameters[25].mem = VVPTR( v->phongsharp );
    /* ambient */
    Parameters[26].type = T_DOUBLE;
    Parameters[26].min = 0.0;
    Parameters[26].max = 1.0;
    Parameters[26].mem = VVPTR( v->ambient );
    /* antialiasing */
    Parameters[27].type = T_INTEGER;
    Parameters[27].min = 1;
    Parameters[27].max = 5;
    Parameters[27].mem = VVPTR( v->antialiasing );

    for( i = 28; i < 39; i++ ) {
        Parameters[i].type = T_DOUBLE;
        Parameters[i].min = 0;
        Parameters[i].max = 1000;
    }

    Parameters[28].max = 1000000;
    Parameters[32].max = 1000000;

    for( i = 39; i < 45; i++ ) {
        STDDOUBLE( Parameters, i );
    }

    Parameters[46].type = T_STRING;
    Parameters[46].mem = NULL;  /* Set before parsing a line by parsing function ! */
    Parameters[46].max = 250;

    Parameters[47].type = T_DOUBLE;
    Parameters[47].min = 0;
    Parameters[47].max = 1000000;
    Parameters[47].mem = VVPTR( v->interocular );

    for( i = 48; i < 64; i++ )
        STDDOUBLE( Parameters, i )
        for( j = 0; j < 4; ++j ) for( i = 0; i < 4; ++i ) {
                Parameters[48 + j * 4 + i].mem = VVPTR( f->p[j][i] );
            }

    /* Now the keywords for parsing */
    for( i = 0; i < 19; i++ ) {
        Keywords[i].which_buf = NO_BUF;
    }

    Keywords[0].name = "colorscheme";
    Keywords[0].paramcount = 1;
    Keywords[0].param = &Parameters[0];
    Keywords[1].name = "resolution";
    Keywords[1].paramcount = 3;
    Keywords[1].param = &Parameters[1];

    Keywords[2].name = "c";
    Keywords[2].paramcount = 4;
    Keywords[2].param = &Parameters[4];

    Keywords[3].name = "bailout";
    Keywords[3].paramcount = 1;
    Keywords[3].param = &Parameters[8];

    Keywords[4].name = "lvalue";
    Keywords[4].paramcount = 1;
    Keywords[4].param = &Parameters[9];

    Keywords[5].name = "maxiter";
    Keywords[5].paramcount = 1;
    Keywords[5].param = &Parameters[10];

    Keywords[6].name = "formula";
    Keywords[6].paramcount = 1;
    Keywords[6].param = &Parameters[11];

    Keywords[7].name = "viewpoint";
    Keywords[7].paramcount = 3;
    Keywords[7].param = &Parameters[12];

    Keywords[8].name = "up";
    Keywords[8].paramcount = 3;
    Keywords[8].param = &Parameters[15];

    Keywords[9].name = "light";
    Keywords[9].paramcount = 3;
    Keywords[9].param = &Parameters[18];

    Keywords[10].name = "move";
    Keywords[10].paramcount = 2;
    Keywords[10].param = &Parameters[21];

    Keywords[11].name = "lxr";
    Keywords[11].paramcount = 1;
    Keywords[11].param = &Parameters[23];

    Keywords[12].name = "phong";
    Keywords[12].paramcount = 2;
    Keywords[12].param = &Parameters[24];

    Keywords[13].name = "ambient";
    Keywords[13].paramcount = 1;
    Keywords[13].param = &Parameters[26];

    Keywords[14].name = "antialiasing";
    Keywords[14].paramcount = 1;
    Keywords[14].param = &Parameters[27];

    Keywords[15].name = "color";
    Keywords[15].paramcount = 4;
    Keywords[15].param = &Parameters[28];
    Keywords[15].format_string = "%1%2%3%4%2%3%4";
    Keywords[15].which_buf = COL_BUF;

    Keywords[16].name = "colorrange";
    Keywords[16].paramcount = 7;
    Keywords[16].param = &Parameters[32];
    Keywords[16].format_string = "%1%2%3%4%5%6%7";
    Keywords[16].which_buf = COL_BUF;

    Keywords[17].name = "plane";
    Keywords[17].paramcount = 6;
    Keywords[17].param = &Parameters[39];
    sprintf( s, "%s", "%b01d%1%2%3%4%5%6\0" );
    s[4] = CUT_PLANE;
    Keywords[17].format_string = &s[0];
    /*Keywords[17].format_string[4] = CUT_PLANE;*/
    Keywords[17].which_buf = CUT_BUF;

    Keywords[18].name = "include";
    Keywords[18].paramcount = 1;
    Keywords[18].param = &Parameters[46];

    Keywords[19].name = "interocular";
    Keywords[19].paramcount = 1;
    Keywords[19].param = &Parameters[47];

    Keywords[20].name = "p1";
    Keywords[20].paramcount = 4;
    Keywords[20].param = &Parameters[48];

    Keywords[21].name = "p2";
    Keywords[21].paramcount = 4;
    Keywords[21].param = &Parameters[52];

    Keywords[22].name = "p3";
    Keywords[22].paramcount = 4;
    Keywords[22].param = &Parameters[56];

    Keywords[23].name = "p4";
    Keywords[23].paramcount = 4;
    Keywords[23].param = &Parameters[60];

    Keywords[24].name = "";

    /* Phew, all done. */

    return( 0 );
}

int IsStereo( struct view_struct* view ) {
    if( view->interocular > 0.0001 ) {
        return( -1 );
    } else {
        return( 0 );
    }
}

int CopyIDAT( int copy, /*struct disppal_struct *disppal, struct vidinfo_struct *vidinfo,*/
              struct png_internal_struct* i, struct png_internal_struct* i2,
              unsigned char* Buf, int ystart,/* int xadd, int yadd, */int zflag, int zres )
/* "copy" decides whether a new IDAT chunk is written */
{
    int j;

    while( ( strncmp( i->chunk_type, "IDAT", 4 ) != 0 ) && ( strncmp( i->chunk_type, "IEND", 4 ) != 0 ) ) {
        GetNextChunk( i );
    }

    if( strncmp( i->chunk_type, "IEND", 4 ) == 0 ) {
        return( -1 );    /* No IDAT chunk found */
    }

    for( j = 0; j < ystart; j++ ) {
        if( ReadPNGLine( i, Buf ) ) {
            return( -2 );
        }

        if( copy ) {
            WritePNGLine( i2, Buf );
        }

        /* store line */
        if( zflag == 0 ) {
            putline( 0L, ( long )i->width - 1, ( long )i->width, j, &Buf[1], 0 );    /* to image buffer */
        } else {
            putline( 0L, ( long )i->width - 1, ( long )i->width, j, &Buf[1], 1 );    /* to zbuffer */
        }

        /* display line */
        if( zflag == 0 ) {
            update_bitmap( 0L, ( long )i->width - 1, ( long )i->width, j, &Buf[1], 0 );
        } else if( zflag == 1 ) {
            update_bitmap( 0L, ( long )i->width - 1, ( long )i->width, j, &Buf[1], zres );
        }

        check_event();
        /*eol(j+1);*/
    }

    return( 0 );
}

int SetDefaults( struct frac_struct* frac, struct view_struct* view, struct realpal_struct* realpal, char* colscheme,
                 double* cutbuf ) {
    int i, j;

    frac->c[0] = 0;
    frac->c[1] = 0;
    frac->c[2] = 0;
    frac->c[3] = 0;
    frac->bailout = 16;
    frac->maxiter = 12;
    frac->lvalue = 0;
    frac->formula = 0;

    for( j = 0; j < 4; ++j ) for( i = 0; i < 4; ++i ) {
            frac->p[j][i] = 0.0;
        }

    view->s[0] = 0;
    view->s[1] = 0;
    view->s[2] = -2;
    view->up[0] = 0;
    view->up[1] = -1;
    view->up[2] = 0;
    view->light[0] = -2;
    view->light[1] = -5;
    view->light[2] = -5;
    view->Mov[0] = 0;
    view->Mov[1] = 0;
    view->LXR = 2.8;
    view->xres = 160;
    view->yres = 120;
    view->zres = 120;
    view->interocular = 0;
    view->phongmax = 0.6;
    view->phongsharp = 30;
    view->ambient = 0.04;
    view->antialiasing = 1;

    realpal->cols[0].col1[0] = 0;
    realpal->cols[0].col1[1] = 0;
    realpal->cols[0].col1[2] = 1;
    realpal->cols[0].col2[0] = 1;
    realpal->cols[0].col2[1] = 0;
    realpal->cols[0].col2[2] = 0;
    realpal->cols[0].weight = 1;
    realpal->colnum = 1;

    strcpy( colscheme, "0" );

    for( i = 0; i < 140; i++ ) {
        cutbuf[i] = 0;
    }

    *( char* )&cutbuf[0] = CUT_TERMINATOR;
    return( 0 );
}

float colorizepoint( /*float diffr, int closest_iteration,*/ struct calc_struct* c )
/* finds color for the point c->xq */
{
    /* Handles for variables */
    static unsigned char xh = 255, yh = 255, zh = 255; /*, xbh=255, ybh=255, zbh=255 */
    /*, diffrh=255, eorbith = 255, jorbith = 255, korbith = 255; */
    /* static unsigned char lorbith = 255, closestith = 255; */
    /*   double x, y, z; */
    float CBuf;
    char notdef;

    /* Init(&prog);*/  /* !!!!!!!! Really necessary??? */
    SetVariable( "x", &xh, c->xq[0], &prog );
    SetVariable( "y", &yh, c->xq[1], &prog );
    SetVariable( "z", &zh, c->xq[2], &prog );
    /*   SetVariable("xb", &xbh, lastorbit[0], &prog);
       SetVariable("yb", &ybh, lastorbit[1], &prog);
       SetVariable("zb", &zbh, lastorbit[2], &prog);*/
    /*   SetVariable("diffr", &diffrh, diffr, &prog);
       SetVariable("eorbit", &eorbith, lastorbit[0], &prog);
       SetVariable("jorbit", &jorbith, lastorbit[1], &prog);
       SetVariable("korbit", &korbith, lastorbit[2], &prog);
       SetVariable("lorbit", &lorbith, lastorbit[3], &prog);
       SetVariable("closestit", &closestith, closest_iteration, &prog); */
    CBuf = ( float )calculate( &notdef, &prog );

    /* Make sure result is between 0 and 1 */
    CBuf = ( float )fmod( ( double )CBuf, ( double )1.0 );

    if( CBuf < 0 ) {
        CBuf = 1 + CBuf;
    }

    return( CBuf );
}

int ParseINI( const char* fil, char* Error, struct frac_struct* frac, struct view_struct* view,
              struct realpal_struct* realpal, char* colscheme, double* cutbuf )
/* file is an ini with path & suffix */
{
    char inipath[256], file[256], ErrorMSG[200], *s;
    int blen, i, colbufc;
    char rc;

    strcpy( inipath, fil );
    ConvertPath( inipath );
    strcpy( file, inipath );
    s = strrchr( inipath, slash );

    if( s == NULL ) {
        inipath[0] = 0;
    } else {
        *( ++s ) = 0;
    }

    PrepareKeywords( colscheme, frac, view );
    rc = 0;
    /* realpal->colnum = 0; */ blen = 0;
    colbufc = 0;

    if( ( i = ParseFile( file, inipath, Keywords, ( char* )realpal->cols, 30 * sizeof( struct col_struct ),
                         &colbufc, cutbuf, 140, &blen, &rc, ErrorMSG ) ) != 0 ) {
        sprintf( Error, "INI file '%s' error:\n%s\n", file, ErrorMSG );
        return( -1 );
    }

    /* Don't clear intersection objects if none defined in INI */
    if( blen != 0 ) {
        cutbuf[blen] = CUT_TERMINATOR;
    }

    if( colscheme[0] == 0 ) {
        strcpy( colscheme, "0" );
    }

    if( colbufc != 0 ) {
        realpal->colnum = colbufc / 56;
    }


    return( 0 );
}

char orbite( double* a, const double* b ) {
    int max = ( int )orbit[0][0];
    int i = ( int ) * b;

    if( i > max ) {
        i = max;
    }

    if( i < 0 ) {
        i = 0;
    }

    *a = orbit[i + 1][0];
    return 0;
}

char orbitj( double* a, const double* b ) {
    int max = ( int )orbit[0][0];
    int i = ( int ) * b;

    if( i > max ) {
        i = max;
    }

    if( i < 0 ) {
        i = 0;
    }

    *a = orbit[i + 1][1];
    return 0;
}

char orbitk( double* a, const double* b ) {
    int max = ( int )orbit[0][0];
    int i = ( int ) * b;

    if( i > max ) {
        i = max;
    }

    if( i < 0 ) {
        i = 0;
    }

    *a = orbit[i + 1][2];
    return 0;
}

char orbitl( double* a, const double* b ) {
    int max = ( int )orbit[0][0];
    int i = ( int ) * b;

    if( i > max ) {
        i = max;
    }

    if( i < 0 ) {
        i = 0;
    }

    *a = orbit[i + 1][3];
    return 0;
}

int TranslateColorFormula( char* colscheme, char* ErrorMSG ) {
    unsigned char dummy;
    double ddummy;

    Init( &prog );
    DeclareFunction( "orbite", orbite, &prog );
    DeclareFunction( "orbitj", orbitj, &prog );
    DeclareFunction( "orbitk", orbitk, &prog );
    DeclareFunction( "orbitl", orbitl, &prog );
    ddummy = 0;
    dummy = 255;
    SetVariable( "x", &dummy, ddummy, &prog );
    dummy = 255;
    SetVariable( "y", &dummy, ddummy, &prog );
    dummy = 255;
    SetVariable( "z", &dummy, ddummy, &prog );

    /*   dummy = 255; SetVariable("xb", &dummy, ddummy, &prog);
       dummy = 255; SetVariable("yb", &dummy, ddummy, &prog);
       dummy = 255; SetVariable("zb", &dummy, ddummy, &prog);*/
    /*   dummy = 255; SetVariable("diffr", &dummy, ddummy, &prog); */
    /*  dummy = 255; SetVariable("eorbit", &dummy, ddummy, &prog);
      dummy = 255; SetVariable("jorbit", &dummy, ddummy, &prog);
      dummy = 255; SetVariable("korbit", &dummy, ddummy, &prog);
      dummy = 255; SetVariable("lorbit", &dummy, ddummy, &prog);
      dummy = 255; SetVariable("closestit", &dummy, ddummy, &prog);  */
    if( Translate( ErrorMSG, colscheme, &prog ) != 0 ) {
        return( -1 );
    }

    return( 0 );
}

int FormatExternToIntern( struct frac_struct* frac, struct view_struct* view ) {
    /* This is to change the input format to internal format */
    /* Input: light relative to viewers position (for convenience of user) */
    /* Internal: light absolute in space (for convenience of programmer) */
    int i;
    struct basestruct base;
    vec3 ltmp;

    if( calcbase( &base, NULL, *view, 0 ) != 0 ) {
        return( -1 );
    }

    forall( i, 3 ) ltmp[i] = view->light[0] * base.x[i] + view->light[1] * base.y[i] +
                             view->light[2] * base.z[i];
    forall( i, 3 ) view->light[i] = view->s[i] + ltmp[i]
                                    + view->Mov[0] * base.x[i] + view->Mov[1] * base.y[i];
    /* bailout */
    /* Input: real bailout value */
    /* Internal: square of bailout value (saves some time) */
    frac->bailout *= frac->bailout;
    return( 0 );
}

int FormatInternToExtern( struct frac_struct* frac, struct view_struct* view ) {
    /* Reverse process of "FormatExternToIntern". see above */
    int i;
    struct basestruct base;
    vec3 ltmp;

    if( calcbase( &base, NULL, *view, 0 ) != 0 ) {
        return( -1 );
    }

    frac->bailout = sqrt( frac->bailout );
    forall( i, 3 ) ltmp[i] = view->light[i] - view->s[i];
    view->light[0] = scalarprod( ltmp, base.x ) - view->Mov[0];
    view->light[1] = scalarprod( ltmp, base.y ) - view->Mov[1];
    view->light[2] = scalarprod( ltmp, base.z );

    return( 0 );
}

int InitGraphics( char* Error, struct view_struct* view, struct realpal_struct* realpal, int ready,
                  struct vidinfo_struct* vidinfo, struct disppal_struct* disppal, int* xadd, int* yadd, int zbufflag ) {
    /* ready ... 0: not ready */
    /* zbufflag ... 0: no zbuffer (reason: AA) */
    int i, j;

    /* Do graphics initialization &  return mode info (in vidinfo) in different versions */
    *yadd = 0;
    *xadd = 0;
    i = view->xres;

    if( IsStereo( view ) ) {
        i *= 2;
    }

    j = view->yres;

    if( zbufflag ) {
        i *= view->antialiasing;
        j *= view->antialiasing;
    }

    /* X11
       X_initialize(i, j);
       X_ReturnVideoInfo(vidinfo);
       if (vidinfo->maxcol!=-1)
       {
          disppal->btoc = 1;
          if (CreateDispPal(disppal, realpal, vidinfo->maxcol, view->phongmax,
    	 vidinfo->rdepth, vidinfo->gdepth, vidinfo->bdepth)==-1)
          {
    	 sprintf(Error, "Too many colours for this display.\n Use text only version.\n");
    	 return(-1);
          }
          {
    	 int nc;
    	 if ((nc=X_SetColors(disppal, 0))!=disppal->maxcol)
    	 {
    	    printf("Could only allocate %i colors (out of %i).\n", nc, disppal->maxcol);
    	    if (ready) printf("XV can probably do a better job. Hope it reads PNG... (patched versions do!)\n");
    	 }
          }
       }
       else CalcWeightsum(realpal);
    */

    if( Initialize != NULL ) if( Initialize( i, j, Error ) != 0 ) {
            return( -1 );
        }

    if( ReturnVideoInfo != NULL ) {
        ReturnVideoInfo( vidinfo );
    }

    if( vidinfo->maxcol != -1 ) {
        int r = 0;
        disppal->btoc = 1;

        if( !zbufflag )
            r = CreateDispPal( disppal, realpal, vidinfo->maxcol, view->phongmax,
                               vidinfo->rdepth, vidinfo->gdepth, vidinfo->bdepth );
        else {
            struct realpal_struct rp;
            rp.colnum = 1;
            rp.cols[0].weight = 1;
            rp.cols[0].col1[0] = 1;
            rp.cols[0].col1[1] = 1;
            rp.cols[0].col1[2] = 1;
            rp.cols[0].col2[0] = 1;
            rp.cols[0].col2[1] = 1;
            rp.cols[0].col2[2] = 1;
            r = CreateDispPal( disppal, &rp, vidinfo->maxcol, view->phongmax,
                               vidinfo->rdepth, vidinfo->gdepth, vidinfo->bdepth );
        }

        if( r == -1 ) {
            sprintf( Error, "Too many colours for this display (%i).\nUse text only version.\n", vidinfo->maxcol );
            return( -1 );
        }

        if( SetColors != NULL ) {
            SetColors( disppal );
        }
    } else {
        CalcWeightsum( realpal );
    }

    return( 0 );
}

int AllocBufs( struct view_struct* v, int zflag, int pal, float** CBuf, float** BBuf, float** LBufR,
               float** LBufL, unsigned char** line, unsigned char** line2, unsigned char** line3 )
/* zflag ... whether img (0), zbuf (1) or zbuf->img (2) is calculated */
/* pal ... whether palette or not */
{
    long LBufSize;
    unsigned int st;

    if( IsStereo( v ) ) {
        st = 2;
    } else {
        st = 1;
    }

    LBufSize = ( long )v->xres * v->antialiasing * ( v->antialiasing + 1L ) * ( long )sizeof( float ) + 10L;

    /*#if _MSC_VER <= 800
       if (LBufSize>65535) return(-2);
      #endif */
    switch( zflag ) {
        case 0:   /* create an image without ZBuffer */
            *LBufR = calloc( ( size_t )LBufSize, 1 );
            *line = calloc( 3 * v->xres * st + 1 + 10, 1 );

            if( pal ) {
                *line2 = calloc( v->xres * st + 10, 1 );
            }

            *CBuf = calloc( v->xres * st * sizeof( float ) + 10, 1 );
            *BBuf = calloc( v->xres * st * sizeof( float ) + 10, 1 );

            if( st == 2 ) {
                *LBufL = calloc( ( size_t )LBufSize + 10, 1 );
            }

            if( pal && !*line2 ) {
                return( -1 );
            }

            if( st == 2 && !*LBufL ) {
                return( -1 );
            }

            if( !*LBufR || !*line || !*CBuf || !*BBuf ) {
                return( -1 );
            }

            break;

        case 1:   /* create a ZBuffer only */
            /* LBufR and LBufL each hold aa lines (mono) */
            *LBufR = calloc( ( size_t )LBufSize, 1 );
            /* line only holds a single stereo line for transferring of LBuf->global ZBuffer */
            *line = calloc( 3 * v->xres * v->antialiasing * st + 1 + 10, 1 );

            if( st == 2 ) {
                *LBufL = calloc( ( size_t )LBufSize, 1 );
            }

            if( st == 2 && !*LBufL ) {
                return( -1 );
            }

            if( !*LBufR || !*line ) {
                return( -1 );
            }

            break;

        case 2:   /* create an image using a ZBuffer */
            *LBufR = calloc( ( size_t )LBufSize, 1 );
            *line = calloc( 3 * v->xres * st + 1 + 10, 1 );

            if( pal ) {
                *line2 = calloc( v->xres * st + 10, 1 );
            }

            *CBuf = calloc( v->xres * st * sizeof( float ) + 10, 1 );
            *BBuf = calloc( v->xres * st * sizeof( float ) + 10, 1 );
            *line3 = calloc( 3 * v->xres * v->antialiasing * st + 10, 1 );

            if( st == 2 ) {
                *LBufL = calloc( ( size_t )LBufSize, 1 );
            }

            if( pal && !*line2 ) {
                return( -1 );
            }

            if( st == 2 && !*LBufL ) {
                return( -1 );
            }

            if( !*LBufR || !*line || !*line3 || !*CBuf || !*BBuf ) {
                return( -1 );
            }

            break;
    }

    return( 0 );
}

int FreeBufs( float** a, float** g, float** b, float** c, unsigned char** d,
              unsigned char** e, unsigned char** f ) {
    free( orbit );

    if( *f ) {
        free( *f );
    }

    if( *e ) {
        free( *e );
    }

    if( *d ) {
        free( *d );
    }

    if( *a ) {
        free( *a );
    }

    if( *c ) {
        free( *c );
    }

    if( *b ) {
        free( *b );
    }

    if( *g ) {
        free( *g );
    }

    return( 0 );
}

int LBuf_ZBuf( struct view_struct* v, float* LBuf, unsigned char* line, long ii,
               long imax, long aa_line, int offs ) {
    int tmp;
    long l, i, k;

    for( k = 0; k < v->antialiasing; k++ )
        for( i = ii; i <= imax; i++ ) {
            tmp = i * v->antialiasing + k + offs;
            l = ( long )floor( LBuf[( ( size_t )i + ( size_t )aa_line * ( size_t )v->xres )
                                    * ( size_t )v->antialiasing + ( size_t )k] * 100 + 0.5 );
            line[tmp * 3 + 1] = ( unsigned char )( l >> 16 & 0xFF );
            line[tmp * 3 + 2] = ( unsigned char )( l >> 8  & 0xFF );
            line[tmp * 3 + 3] = ( unsigned char )( l     & 0xFF );
        }

    return( 0 );
}

int CalculateFractal( char* Error, char* pngfile, FILE** png, /* struct png_info_struct *png_info,*/
                      struct png_internal_struct* png_internal,
                      int zflag, int* xstart, int* ystart,/* int xadd, int yadd, */int no, struct basestruct* rbase, struct basestruct* srbase,
                      struct basestruct* lbase, struct basestruct* slbase,
                      struct frac_struct* f, struct view_struct* v, struct realpal_struct* realpal,/* char *colscheme,*/ double* cutbuf,
                      struct vidinfo_struct* vidinfo, struct disppal_struct* disppal )
/* pngfile: string of filename, without path (only for title bar) */
/* png: _opened_ png file */
/* png file returns _closed_ */
/*      and initialized (png_info, png_internal) */
/* if png==NULL, no file is written */
/* also closes graphics, resets keyboard mode */
/* wants external format for light-source and bailout */
/* zflag: decides what to calculate,
      0..image from scratch, size view.xres*view.yres
      1..ZBuffer from scratch, size view.xres*view.yres*AA^2
      2..image from ZBuffer, size img: xres*yres, buffer *AA^2
      for every case take into account that images can be stereo! */
{
    long i, j;
    long xres_st, xres_aa, xres_st_aa;
    float* LBufR, *LBufL, *CBuf, *BBuf;
    unsigned char* line, *line2, *line3;
    struct calc_struct cr, cl;
    unsigned long my_time;
    time_t dummytime;
    char MSG[300];
    long ii, imax, k, kk;
    struct frac_struct frac;
    struct view_struct view;
    int rrv = 0, firstline;

    frac = *f;
    view = *v;

    xres_st = view.xres;

    if( IsStereo( v ) ) {
        xres_st *= 2;
    }

    xres_aa = view.xres * view.antialiasing;
    xres_st_aa = xres_aa;

    if( IsStereo( v ) ) {
        xres_st_aa *= 2;
    }

    if( FormatExternToIntern( &frac, &view ) != 0 ) {
        sprintf( Error, "Error in view struct!" );
        return( -1 );
    }

    /* Allocate some memory */
    CBuf = NULL;
    BBuf = NULL;
    LBufR = NULL;
    LBufL = NULL;
    line = NULL;
    line2 = NULL;
    line3 = NULL;
    orbit = NULL;
    orbit = calloc( frac.maxiter + 2, 4 * sizeof( double ) ); /* freed by FreeBufs */

    /* "+2", because orbit[0] is special flag for orbite,j,k,l */
    if( AllocBufs( v, zflag, ( vidinfo->maxcol != -1 ), &CBuf, &BBuf, &LBufR, &LBufL, &line,
                   &line2, &line3 ) != 0 || orbit == NULL ) {
        FreeBufs( &LBufR, &LBufL, &CBuf, &BBuf, &line, &line2, &line3 );
        sprintf( Error, "Not enough memory." );
        return( -1 );
    }

    my_time = ( long )time( ( time_t )NULL );

    if( frac.formula == 0 ) {
        cr.iterate = iterate_0;
        cr.iternorm = iternorm_0;
        cl.iterate = iterate_0;
        cl.iternorm = iternorm_0;
    }

    if( frac.formula == 1 ) {
        cr.iterate = iterate_1;
        cr.iternorm = iternorm_1;
        cl.iterate = iterate_1;
        cl.iternorm = iternorm_1;
    }

    if( frac.formula == 2 ) {
        cr.iterate = iterate_2;
        cr.iternorm = 0;
        cl.iterate = iterate_2;
        cl.iternorm = 0;
    }

    if( frac.formula == 3 ) {
        cr.iterate = iterate_3;
        cr.iternorm = 0;
        cl.iterate = iterate_3;
        cl.iternorm = 0;
    }

    if( frac.formula == 4 ) {
        cr.iterate = iterate_4;
        cr.iternorm = 0;
        cl.iterate = iterate_4;
        cl.iternorm = 0;
    }

    /* Initialize variables for calcline (those which don´t change from line to line) */
    cr.v = view;
    cr.f = frac;
    cr.sbase = *srbase;
    cr.base = *rbase;
    cr.cutbuf = cutbuf;
    cr.absx = sqrt( scalarprod( cr.sbase.x, cr.sbase.x ) ) / ( double )cr.v.antialiasing;
    cr.absy = sqrt( scalarprod( cr.sbase.y, cr.sbase.y ) ) / ( double )cr.v.antialiasing;
    cr.absz = sqrt( scalarprod( cr.sbase.z, cr.sbase.z ) );
    cr.aabase = cr.sbase;

    if( ( zflag == 0 || zflag == 1 ) && cr.v.antialiasing > 1 ) {
        forall( i, 3 ) cr.aabase.x[i] /= ( double )cr.v.antialiasing;
        forall( i, 3 ) cr.aabase.y[i] /= ( double )cr.v.antialiasing;
    }

    if( IsStereo( &view ) ) {
        cl.v = view;
        cl.f = frac;
        cl.sbase = *slbase;
        cl.base = *lbase;
        cl.cutbuf = cutbuf;
        cl.absx = sqrt( scalarprod( cl.sbase.x, cl.sbase.x ) ) / ( double )cl.v.antialiasing;
        cl.absy = sqrt( scalarprod( cl.sbase.y, cl.sbase.y ) ) / ( double )cl.v.antialiasing;
        cl.absz = sqrt( scalarprod( cl.sbase.z, cl.sbase.z ) );
        cl.aabase = cl.sbase;

        if( ( zflag == 0 || zflag == 1 ) && cl.v.antialiasing > 1 ) {
            forall( i, 3 ) cl.aabase.x[i] /= ( double )cl.v.antialiasing;
            forall( i, 3 ) cl.aabase.y[i] /= ( double )cl.v.antialiasing;
        }
    }

    /* recalculate last line when resuming an image without ZBuffer */
    firstline = 0;

    if( zflag == 0 ) {
        ( *ystart )--;
        *xstart = 0;
        firstline = 1;
        Change_Name( "recalc." );
    }

    for( j = *ystart; j < view.yres; j++ ) {
        /* Initialize variables for calcline (which change from line to line) */
        cr.xp[3] = frac.lvalue;
        cr.xs[3] = frac.lvalue;
        forall( i, 3 ) cr.xp[i] = srbase->O[i] + j * srbase->y[i];

        if( IsStereo( &view ) ) {
            cl.xp[3] = frac.lvalue;
            cl.xs[3] = frac.lvalue;
            forall( i, 3 ) cl.xp[i] = slbase->O[i] + j * slbase->y[i];
        }

        /* Initialize LBufs from ZBuffer */
        memcpy( &LBufR[0], &LBufR[view.antialiasing * xres_aa], xres_aa * sizeof( float ) );

        if( IsStereo( &view ) ) {
            memcpy( &LBufL[0], &LBufL[view.antialiasing * xres_aa], xres_aa * sizeof( float ) );
        }

        if( zflag == 2 ) {
            for( ii = 0; ii < view.antialiasing + 1; ii++ ) {
                if( j + ii > 0 ) { /* this doesn´t work for the 1st line */
                    QU_getline( line3, j * view.antialiasing + ii - 1, xres_st_aa, 1 );

                    for( i = 0; i < xres_aa; i++ ) LBufR[( size_t )i + ( size_t )ii * ( size_t )xres_aa]
                            = ( float )( ( long )line3[i * 3] << 16 |
                                         ( long )line3[i * 3 + 1] << 8 | ( long )line3[i * 3 + 2] ) / 100;

                    if( IsStereo( &view ) ) {
                        for( i = 0; i < xres_aa; i++ ) LBufL[( size_t )i + ( size_t )ii * ( size_t )xres_aa] =
                                ( float )( ( long )line3[( i + xres_aa ) * 3] << 16
                                           | ( long )line3[( i + xres_aa ) * 3 + 1] << 8 | ( long )line3[( i + xres_aa ) * 3 + 2] ) / 100;
                    }
                } else {
                    for( k = 0; k < xres_aa; k++ ) {
                        LBufR[( size_t )k] = ( float )view.zres;
                    }

                    if( IsStereo( &view ) ) for( k = 0; k < xres_aa; k++ ) {
                            LBufL[( size_t )k] = ( float )view.zres;
                        }
                }
            }
        }

        for( ii = *xstart; ii < view.xres; ii += no ) {
            imax = ii + no - 1;

            if( imax > view.xres - 1 ) {
                imax = view.xres - 1;
            }

            calcline( ii, imax, j, LBufR, BBuf, CBuf, &cr, zflag );  /* right eye image */

            if( ( zflag == 0 || zflag == 2 ) && !firstline ) { /* image to calculate */
                if( vidinfo->maxcol != -1 ) PixelvaluePalMode( ii, imax, disppal->colnum - 1,
                            disppal->brightnum - 1, line2, CBuf, BBuf );

                PixelvalueTrueMode( ii, imax, 255, 255, 255, realpal, &( line[1] ), CBuf, BBuf );
            }

            if( IsStereo( &view ) ) {
                calcline( ii, imax, j, LBufL, &BBuf[view.xres], &CBuf[view.xres],
                          &cl, zflag );  /* left eye image */

                if( ( zflag == 0 || zflag == 2 ) && !firstline ) { /* image to calculate */
                    if( vidinfo->maxcol != -1 ) PixelvaluePalMode( ii, imax, disppal->colnum - 1,
                                disppal->brightnum - 1, &line2[view.xres], &CBuf[view.xres], &BBuf[view.xres] );

                    PixelvalueTrueMode( ii, imax, 255, 255, 255, realpal, &( line[3 * view.xres + 1] ),
                                        &CBuf[view.xres], &BBuf[view.xres] );
                }
            }

            /* Display and Transfer */
            if( zflag == 1 ) { /* the ZBuffer */
                for( kk = 1; kk < view.antialiasing + 1; kk++ ) {
                    LBuf_ZBuf( &view, LBufR, line, ii, imax, kk, 0 );
                    putline( ii * view.antialiasing, ( imax + 1 )*view.antialiasing - 1,
                             xres_st_aa, j * view.antialiasing + kk - 1, line + 1, 1 );
                    update_bitmap( ii * view.antialiasing, ( imax + 1 )*view.antialiasing - 1,
                                   xres_st_aa, j * view.antialiasing + kk - 1, line + 1, view.zres );
                }

                if( IsStereo( &view ) ) {
                    for( kk = 1; kk < view.antialiasing + 1; kk++ ) {
                        LBuf_ZBuf( &view, LBufL, line, ii, imax, kk, xres_aa );
                        putline( ii * view.antialiasing + xres_aa,
                                 ( imax + 1 )*view.antialiasing - 1 + xres_aa,
                                 xres_st_aa, j * view.antialiasing + kk - 1, line + 1, 1 );
                        update_bitmap( ii * view.antialiasing + xres_aa,
                                       ( imax + 1 )*view.antialiasing - 1 + xres_aa,
                                       xres_st_aa, j * view.antialiasing + kk - 1, line + 1, view.zres );
                    }
                }
            } else if( !firstline ) { /* the image */
                putline( ii, imax, xres_st, j, line + 1, 0 );

                if( IsStereo( &view ) ) {
                    putline( ii + view.xres, imax + view.xres, xres_st, j, line + 1, 0 );
                }

                update_bitmap( ii, imax, xres_st, j, line + 1, 0 );

                if( IsStereo( &view ) ) update_bitmap( ii + view.xres, imax + view.xres, xres_st, j, &( line[1] ),
                                                           0 );
            }

            if( ( imax == view.xres - 1 ) && !firstline ) {
                eol( j + 1 );
            }

            rrv = check_event();

            if( rrv != 0 ) {
                i = view.xres * 3;

                if( IsStereo( &view ) ) {
                    i *= 2;
                }

                memset( line, 0, ( size_t )i + 1 );
                calc_time += ( long )time( ( time_t )NULL ) - my_time;

                if( png != NULL ) {
                    PNGEnd( png_internal, line, 0, j );
                    fclose( *png );
                    EndPNG( png_internal );
                }

                FreeBufs( &LBufR, &LBufL, &CBuf, &BBuf, &line, &line2, &line3 );
                *ystart = j;
                *xstart = ii + 1;

                if( *xstart == view.xres ) {
                    *xstart = 0;
                    ( *ystart )++;
                }

                Done();
                return( rrv ); /* return code from check event. (win destroyed / calc stopped) */
            }
        }

        *xstart = 0;

        /* Write to PNG file */
        if( png ) {
            switch( zflag ) {
                case 0:
                    if( !firstline ) {
                        line[0] = 0;       /* Set filter method */
                        DoFiltering( png_internal, line );
                        WritePNGLine( png_internal, line );
                    }

                    break;

                case 1:
                    if( !firstline ) {
                        for( kk = 1; kk < view.antialiasing + 1; kk++ ) {
                            LBuf_ZBuf( &view, LBufR, line, 0, view.xres - 1, kk, 0 );

                            if( IsStereo( &view ) ) LBuf_ZBuf( &view, LBufL, line, 0, view.xres - 1,
                                                                   kk, xres_aa );

                            line[0] = 0;
                            DoFiltering( png_internal, line );
                            WritePNGLine( png_internal, line );
                        }
                    }

                    break;

                case 2:
                    line[0] = 0;      /* Set filter method */
                    DoFiltering( png_internal, line );
                    WritePNGLine( png_internal, line );
            }
        }

        firstline = 0;
    }

    calc_time += ( long )time( &dummytime ) - my_time;

    if( png != NULL ) {
        PNGEnd( png_internal, line, 0, j );
        fclose( *png );
        EndPNG( png_internal );
    }

    FreeBufs( &LBufR, &LBufL, &CBuf, &BBuf, &line, &line2, &line3 );

    if( pngfile != NULL ) {
        sprintf( MSG, "%s", pngfile );
        Change_Name( MSG );
    }

    if( Done != NULL ) {
        Done();
    }

    *xstart = 0;
    *ystart = j;
    return( 0 );
}

int CalculateINI( const char* inif, const char* pngf, char* Error, int zflag ) {
    struct frac_struct frac;
    struct view_struct view;
    struct realpal_struct realpal;
    char colscheme[251];
    double cutbuf[140];
    struct basestruct rbase, srbase, lbase, slbase, cbase;
    struct vidinfo_struct vidinfo;
    struct disppal_struct disppal;
    int xadd, yadd, xstart, ystart;
    FILE* png;
    struct png_info_struct png_info;
    struct png_internal_struct png_internal;
    char ErrorMSG[256], inifile[256], pngfile[256];
    int i;

    strcpy( inifile, inif );
    strcpy( pngfile, pngf );
    ConvertPath( inifile );
    ConvertPath( pngfile );

    SetDefaults( &frac, &view, &realpal, colscheme, &cutbuf[0] );

    if( ParseINI( inifile, Error, &frac, &view, &realpal, colscheme, &cutbuf[0] ) != 0 ) {
        return( -1 );
    }

    if( TranslateColorFormula( colscheme, ErrorMSG ) != 0 ) {
        sprintf( Error, "INI file '%s':\nError in color scheme formula:\n%s\n", inifile, ErrorMSG );
        return( -1 );
    }

    if( calcbase( &cbase, &srbase, view, 0 ) != 0 ) {
        sprintf( Error, "file '%s':\nError in view_struct!\n", inifile );
        return( -1 );
    }

    rbase = cbase;

    if( IsStereo( &view ) ) {
        if( calcbase( &rbase, &srbase, view, 1 ) != 0 ) {
            sprintf( Error, "file '%s':\nError in view_struct (right eye)!\n", inifile );
            return( -1 );
        }

        if( calcbase( &lbase, &slbase, view, -1 ) != 0 ) {
            sprintf( Error, "file '%s':\nError in view_struct (left eye)!\n", inifile );
            return( -1 );
        }
    }

    /* FormatExternToIntern(&frac, &view, &cbase); */
#ifdef GUI
    update_parameters( &frac, &view, &realpal, colscheme, cutbuf );
#endif

#ifndef GUI

    if( zflag == 1 ) {
        size_t xres = view.xres;
        xres *= view.antialiasing;

        if( IsStereo( &view ) ) {
            xres *= 2;
        }

        TEXTVER_zbuf = calloc( ( size_t )xres * ( size_t )view.yres * ( size_t )view.antialiasing * 3L, 1 );

        if( !TEXTVER_zbuf ) {
            sprintf( Error, "Not enough memory for ZBuffer!" );
            return( -1 );
        }
    }

#endif

    if( InitGraphics( Error, &view, &realpal, 0, &vidinfo, &disppal, &xadd, &yadd, zflag ) != 0 ) {
        return( -1 );
    }

    if( PNGInitialization( pngfile, 1, &png, &png_info, &png_internal, 0, 0, 0L, &frac, &view,
                           &disppal, &realpal, colscheme, cutbuf, zflag ) ) {
        sprintf( Error, "Could not create file '%s'\n", pngfile );
#ifndef GUI

        if( zflag == 1 ) {
            free( TEXTVER_zbuf );
        }

#endif
        return( -1 );
    }

    sprintf( Error, "\nCreated file '%s'\n", pngfile );

    xstart = 0;
    ystart = 0;
    calc_time = 0;
    i = CalculateFractal( Error, pngfile, &png, /*&png_info,*/ &png_internal,
                          zflag, &xstart, &ystart,/* xadd, yadd,*/ 4, &rbase, &srbase, &lbase, &slbase, &frac, &view,
                          &realpal,/* colscheme,*/ &cutbuf[0], &vidinfo, &disppal );

    return( i );
}

int CreateImage( char* Error, int* xstart, int* ystart, struct frac_struct* frac, struct view_struct* view,
                 struct realpal_struct* realpal, char* colscheme, double* cutbuf, int noev, int zflag )
/* Creates/Continues image from given parameters. Wants external format of frac & view */
{
    struct basestruct rbase, srbase, lbase, slbase, cbase;
    struct vidinfo_struct vidinfo;
    struct disppal_struct disppal;
    int xadd, yadd;
    char ErrorMSG[256];
    int i;

    if( TranslateColorFormula( colscheme, ErrorMSG ) != 0 ) {
        sprintf( Error, "Error in color scheme formula:\n%s\n", ErrorMSG );
        return( -1 );
    }

    Error[0] = 0;

    if( calcbase( &cbase, &srbase, *view, 0 ) != 0 ) {
        sprintf( Error, "Error in view_struct!\n" );
        return( -1 );
    }

    rbase = cbase;

    if( IsStereo( view ) ) {
        if( calcbase( &rbase, &srbase, *view, 1 ) != 0 ) {
            sprintf( Error, "Error in view_struct (right eye)!\n" );
            return( -1 );
        }

        if( calcbase( &lbase, &slbase, *view, -1 ) != 0 ) {
            sprintf( Error, "Error in view_struct (left eye)!\n" );
            return( -1 );
        }
    }

    /* MSWIN_Initialize will recognize if it is already initialized. Needing vidinfo, disppal...*/
    if( InitGraphics( Error, view, realpal, 0, &vidinfo, &disppal, &xadd, &yadd, ( zflag == 1 ) ) != 0 ) {
        return( -1 );
    }

    /*  FormatExternToIntern(&frac, &view, &cbase); */

    if( ystart == 0 && xstart == 0 ) {
        calc_time = 0;
    }

    i = CalculateFractal( Error, NULL, NULL,/* NULL,*/ NULL,
                          zflag, xstart, ystart,/* xadd, yadd,*/ noev, &rbase, &srbase, &lbase, &slbase, frac, view, realpal,
                          /*colscheme,*/ &cutbuf[0], &vidinfo, &disppal );

    return( i );
}

int CreateZBuf( char* Error, int* xstart, int* ystart, struct frac_struct* frac, struct view_struct* view,
                double* cutbuf, int noev )
/* Creates/Continues ZBuffer from given parameters. Wants external format of frac & view */
{
    struct basestruct rbase, srbase, lbase, slbase, cbase;
    struct realpal_struct realpal;
    struct vidinfo_struct vidinfo;
    struct disppal_struct disppal;
    int xadd, yadd;
    int i;

    /* Set a grayscale palette */
    realpal.colnum = 1;
    realpal.cols[0].weight = 1;

    for( i = 0; i < 3; i++ ) {
        realpal.cols[0].col1[i] = 1;
        realpal.cols[0].col2[i] = 1;
    }

    Error[0] = 0;

    if( calcbase( &cbase, &srbase, *view, 0 ) != 0 ) {
        sprintf( Error, "Error in view_struct!\n" );
        return( -1 );
    }

    rbase = cbase;

    if( IsStereo( view ) ) {
        if( calcbase( &rbase, &srbase, *view, 1 ) != 0 ) {
            sprintf( Error, "Error in view_struct (right eye)!\n" );
            return( -1 );
        }

        if( calcbase( &lbase, &slbase, *view, -1 ) != 0 ) {
            sprintf( Error, "Error in view_struct (left eye)!\n" );
            return( -1 );
        }
    }

    /* MSWIN_Initialize will recognize if it is already initialized. Needing vidinfo, disppal...*/
    if( InitGraphics( Error, view, &realpal, 0, &vidinfo, &disppal, &xadd, &yadd, 1 ) != 0 ) {
        return( -1 );
    }

    i = CalculateFractal( Error, NULL, NULL, NULL,
                          1, xstart, ystart, noev, &rbase, &srbase, &lbase, &slbase, frac, view, &realpal, /*"", */
                          &cutbuf[0], &vidinfo, &disppal );

    return( i );
}

int CalculatePNG( const char* pngf1, const char* pngf2, char* Error, const char* ini, int zflag )
/* if zflag==0 || zflag==1: */
/* pngf1 ... file to read from */
/* pngf2 ... file to create ("" if no name available) */
/* else: */
/* pngf1 ... ZBuffer */
/* pngf2 ... file to create */
/* ini      ... ini file with parameters to replace ZBuffer parameters */
{
    struct frac_struct frac, f;
    struct view_struct view, v;
    struct realpal_struct realpal, rp;
    char colscheme[251], cols[251];
    double cutbuf[140], cbuf[140];
    struct basestruct rbase, srbase, lbase, slbase, cbase;
    struct vidinfo_struct vidinfo;
    struct disppal_struct disppal;
    int xadd, yadd, xstart, ystart;
    FILE* png1, *png2;
    struct png_info_struct png_info1, png_info2;
    struct png_internal_struct png_internal1, png_internal2;
    unsigned char* line;
    int i;
    char ErrorMSG[256], pngfile1[256], pngfile2[256], ready;
    int ys;

    strcpy( pngfile1, pngf1 );
    strcpy( pngfile2, pngf2 );
    ConvertPath( pngfile1 );
    ConvertPath( pngfile2 );

    SetDefaults( &frac, &view, &realpal, colscheme, &cutbuf[0] );

    /* Do PNG-Init for loading */
    if( ( png1 = fopen( pngfile1, "rb" ) ) == NULL ) {
        sprintf( Error, "Cannot open file '%s'.'\n", pngfile1 );
        return( -1 );
    }

    if( InitPNG( png1, &png_info1, &png_internal1 ) == -1 ) {
        sprintf( Error, "File '%s' is not a valid png-file.\n", pngfile1 );
        fclose( png1 );
        EndPNG( &png_internal1 );
        return( -1 );
    }

    i = ReadParameters( Error, &xstart, &ystart, &png_internal1,
                        &frac, &view, &realpal, colscheme, &cutbuf[0] );

    /* if (i==-128) sprintf(Error, "Warning: File version higher than %s.\n",PROGVERSION); */
    if( i < 0 && i > -128 ) {
        fclose( png1 );
        EndPNG( &png_internal1 );
        return( -1 );
    }

    if( zflag == 2 && ystart != view.yres ) {
        sprintf( Error, "Calculation of ZBuffer '%s' is not ready.\n", pngfile1 );
        return( -1 );
    } else if( ystart == view.yres ) {
        sprintf( Error, "Calculation of image '%s' was complete.\n", pngfile1 );
        ready = 1;
    } else {
        ready = 0;
    }

    if( ready == 0 && pngfile2[0] == 0 ) {
        sprintf( Error, "Couldn't find a free filename based on '%s'\n", pngfile1 );
        fclose( png1 );
        EndPNG( &png_internal1 );
        return( -1 );
    }

    rp = realpal;

    if( zflag == 2 && ini[0] != 0 ) {
        f = frac;
        v = view;
        strncpy( cols, colscheme, 251 );
        i = ParseINI( ini, Error, &f, &v, &rp, cols, cbuf );

        if( i ) {
            fclose( png1 );
            EndPNG( &png_internal1 );
            return( -1 );
        }

        for( i = 0; i < 3; i++ ) {
            view.light[i] = v.light[i];
        }

        view.ambient = v.ambient;
        view.phongmax = v.phongmax;
        view.phongsharp = v.phongsharp;
        realpal = rp;
        strncpy( colscheme, cols, 251 );
    }

    if( TranslateColorFormula( colscheme, ErrorMSG ) != 0 ) {
        sprintf( Error, "Strange error:\nPNG-File '%s':\nError in color scheme formula:\n%s\n",
                 pngfile1, ErrorMSG );
        fclose( png1 );
        EndPNG( &png_internal1 );
        return( -1 );
    }

    if( calcbase( &cbase, &srbase, view, 0 ) != 0 ) {
        sprintf( Error, "file '%s':\nError in view_struct!\n", pngfile1 );
        fclose( png1 );
        EndPNG( &png_internal1 );
        return( -1 );
    }

    rbase = cbase;

    if( IsStereo( &view ) ) {
        if( calcbase( &rbase, &srbase, view, 1 ) != 0 ) {
            sprintf( Error, "file '%s':\nError in view_struct (right eye)!\n", pngfile1 );
            fclose( png1 );
            EndPNG( &png_internal1 );
            return( -1 );
        }

        if( calcbase( &lbase, &slbase, view, -1 ) != 0 ) {
            sprintf( Error, "file '%s':\nError in view_struct (left eye)!\n", pngfile1 );
            fclose( png1 );
            EndPNG( &png_internal1 );
            return( -1 );
        }
    }

#ifdef GUI
    update_parameters( &frac, &view, &realpal, colscheme, cutbuf );
#else

    if( zflag == 2 || zflag == 1 ) {
        size_t xres = view.xres;

        xres *= view.antialiasing;

        if( IsStereo( &view ) ) {
            xres *= 2;
        }

        TEXTVER_zbuf = calloc( ( size_t )xres * ( size_t )view.yres * ( size_t )view.antialiasing * 3L, 1 );

        if( !TEXTVER_zbuf ) {
            sprintf( Error, "Not enough memory for ZBuffer!" );
            fclose( png1 );
            EndPNG( &png_internal1 );
            return( -1 );
        }
    }

#endif

    if( InitGraphics( Error, &view, &rp, ready, &vidinfo, &disppal, &xadd, &yadd, zflag ) != 0 ) {
        return( -1 );
    }

    /* Allocate memory for "CopyIDAT" */
    i = view.xres;

    if( IsStereo( &view ) ) {
        i *= 2;
    }

    if( zflag == 1 || zflag == 2 ) {
        i *= view.antialiasing;
    }

    line = malloc( ( size_t )i * 3 + 1 );

    if( !line ) {
        sprintf( Error, "Not enough memory for CopyIDAT" );
        return( -1 );
    }

    ys = ystart;

    if( zflag == 1 || zflag == 2 ) {
        ys *= view.antialiasing;
    }

    if( zflag == 2 ) {
        if( PNGInitialization( pngfile2, 1, &png2, &png_info2, &png_internal2, 0, 0, 0L, &frac, &view,
                               &disppal, &realpal, colscheme, cutbuf, zflag ) ) {
            sprintf( Error, "Could not create file '%s'\n", pngfile2 );
#ifndef GUI
            free( TEXTVER_zbuf );
#endif
            return( -1 );
        }

        sprintf( Error, "\nCreated new file '%s'\n", pngfile2 );
        ystart = 0;
        xstart = 0;
    }

    if( !ready ) {
        if( PNGInitialization( pngfile2, 1, &png2, &png_info2, &png_internal2, 0, 0, 0L, &frac, &view,
                               &disppal, &realpal, colscheme, cutbuf, zflag ) ) {
            sprintf( Error, "Could not create file '%s'\n", pngfile2 );
            return( -1 );
        }

        CopyIDAT( 1, /*&disppal, &vidinfo,*/ &png_internal1, &png_internal2, line, ys, zflag, view.zres );
        sprintf( Error, "\nCreated new file '%s'\n", pngfile2 );
    } else {
        if( zflag == 0 ) {
            CopyIDAT( 0, /*&disppal, &vidinfo,*/ &png_internal1, &png_internal2, line, ys, 0, view.zres );
        } else {
            CopyIDAT( 0, /*&disppal, &vidinfo,*/ &png_internal1, &png_internal2, line, ys, 1, view.zres );
        }

        if( zflag != 2 ) {
            Done();
        }
    }

    free( line );

    i = 0;

    if( !ready || zflag == 2 ) {
        fclose( png1 );
        EndPNG( &png_internal1 );

        if( zflag == 2 && vidinfo.maxcol != -1 ) {
            InitGraphics( Error, &view, &rp, ready, &vidinfo, &disppal, &xadd,
                          &yadd, 0 );
        }

        i = CalculateFractal( Error, pngfile2, &png2, /*&png_info2,*/ &png_internal2,
                              zflag, &xstart, &ystart,/* xadd, yadd,*/ 4, &rbase, &srbase, &lbase, &slbase, &frac, &view, &realpal,
                              /*colscheme,*/ &cutbuf[0], &vidinfo, &disppal );
    } else {
        /*      i = CalculateFractal(Error, pngfile1, &png1, &png_info1, &png_internal1,
                 zflag, &xstart, &ystart, xadd, yadd, 4, &rbase, &srbase, &lbase, &slbase, &frac, &view, &realpal,
                 colscheme, &cutbuf[0], &vidinfo, &disppal);  */
    }

    /* Done();*/
#ifndef GUI

    if( zflag == 2 || zflag == 1 ) {
        free( TEXTVER_zbuf );
    }

#endif
    return( i );
}

int ParseAndCalculate( const char* fil, char* Error, char zflag )
/* file is an ini or a png file, which gets calculated/continued */
/* called by command-line versions (non-Windows), which are given just a file */
/* which can be both ini and png */
{
    char inifile[256], pngfile1[256], pngfile2[256], inipath[256];
    char* s, *s2, file[256];

    strcpy( inipath, fil );
    ConvertPath( inipath );
    strcpy( file, inipath );
    s = strrchr( inipath, slash );

    if( s == NULL ) {
        inipath[0] = 0;
    } else {
        *( ++s ) = 0;
    }

    inifile[0] = 0;
    pngfile1[0] = 0;
    pngfile2[0] = 0;

    /* Check if file is INI-, ZPN- or PNG-file */
    if( strncmp( &file[strlen( file ) - 4], ".png", 4 ) == 0 ) {
        strncpy( pngfile1, file, 256 );
    } else if( strncmp( &file[strlen( file ) - 4], ".zpn", 4 ) == 0 ) {
        strncpy( pngfile1, file, 256 );
    } else {
        strncpy( inifile, file, 256 );
    }

    if( inifile[0] != 0 ) { /* it is an INI file */
        strncpy( pngfile2, inifile, 256 );

        /* Make a pointer to the filename (without path) */
        if( ( s2 = strrchr( pngfile2, slash ) ) == NULL ) {
            s2 = pngfile2;
        } else {
            s2++;
        }

        /* Delete suffixes and append .png or .zpn */
        if( zflag == 0 ) {
            if( ( s = strstr( s2, "." ) ) == NULL ) {
                strcat( pngfile2, ".png" );
            } else {
                strcpy( s, ".png" );
            }
        } else if( zflag == 1 ) {
            if( ( s = strstr( s2, "." ) ) == NULL ) {
                strcat( pngfile2, ".zpn" );
            } else {
                strcpy( s, ".zpn" );
            }
        }

        /* zflag==2 should never occur here, because file is an ini file */
        if( GetNextName( pngfile2, NULL ) != 0 ) {
            sprintf( Error, "Couldn't find a free filename based on '%s'\n", pngfile2 );
            return( -1 );
        }

        return( CalculateINI( inifile, pngfile2, Error, zflag ) );
    } else { /* it´s a PNG */
        strncpy( pngfile2, pngfile1, 256 );

        if( GetNextName( pngfile2, NULL ) != 0 ) {
            pngfile2[0] = 0;
        }

        return( CalculatePNG( pngfile1, pngfile2, Error, "", zflag ) );
    }
}

int ReadParametersAndImage( char* Error, const char* pngf,
                            unsigned char* ready, int* xstart, int* ystart, struct frac_struct* frac, struct view_struct* view,
                            struct realpal_struct* realpal, char* colscheme, double* cutbuf, int zflag )
/* Opens pngfile, reads parameters from it (no checking if valid)
   it initializes graphics and shows image data line per line (using ???_update_bitmap)  */
{
    FILE* png;
    struct png_info_struct png_info;
    struct png_internal_struct png_internal;
    struct vidinfo_struct vidinfo;
    struct disppal_struct disppal;
    int xadd, yadd, i, xres, yr;
    struct basestruct base, sbase;
    unsigned char* line;
    char pngfile[256];

    strcpy( pngfile, pngf );
    ConvertPath( pngfile );

    SetDefaults( frac, view, realpal, colscheme, cutbuf );

    /* Do PNG-Init for loading */
    if( ( png = fopen( pngfile, "rb" ) ) == NULL ) {
        sprintf( Error, "Cannot open png-file '%s'.'\n", pngfile );
        return( -1 );
    }

    if( InitPNG( png, &png_info, &png_internal ) == -1 ) {
        sprintf( Error, "File '%s' is not a valid png-file.\n", pngfile );
        fclose( png );
        EndPNG( &png_internal );
        return( -1 );
    }

    i = ReadParameters( Error, xstart, ystart, &png_internal,
                        frac, view, realpal, colscheme, cutbuf );

    /* if (i==-128) sprintf(Error, "Warning: File version higher than %s.\n",PROGVERSION); */
    if( i < 0 && i > -128 ) {
        fclose( png );
        EndPNG( &png_internal );
        return( -1 );
    }

    if( calcbase( &base, &sbase, *view, 0 ) != 0 ) {
        fclose( png );
        EndPNG( &png_internal );
        sprintf( Error, "file '%s':\nError in view_struct!\n", pngfile );
        return( -1 );
    }

    if( *ystart == view->yres ) {
        *ready = 1;
    } else {
        *ready = 0;
    }

    xres = view->xres;

    if( IsStereo( view ) ) {
        xres *= 2;
    }

    if( zflag == 1 ) {
        xres *= view->antialiasing;
    }

    line = malloc( ( size_t )xres * 3 + 1 );

    if( !line ) {
        sprintf( Error, "Not enough memory to hold a line" );
        fclose( png );
        EndPNG( &png_internal );
        return( -1 );
    }

    if( InitGraphics( Error, view, realpal, *ready, &vidinfo, &disppal, &xadd, &yadd, zflag ) != 0 ) {
        free( line );
        fclose( png );
        EndPNG( &png_internal );
        return( -1 );
    }

    yr = *ystart;

    if( zflag == 1 ) {
        yr *= view->antialiasing;
    }

    if( CopyIDAT( 0, /*&disppal, &vidinfo,*/ &png_internal, NULL, line, yr, zflag, view->zres ) != 0 ) {
        free( line );
        fclose( png );
        EndPNG( &png_internal );
        sprintf( Error, "Error reading file '%s'.", pngfile );
        return( -1 );
    }

    free( line );
    fclose( png );
    EndPNG( &png_internal );
    return( 0 );
}

int SavePNG( char* Error, const char* pngf, int xstart, int ystart,
             struct disppal_struct* disppal, struct frac_struct* f, struct view_struct* v,
             struct realpal_struct* realpal, char* colscheme, double* cutbuf, int zflag ) {
    FILE* png;
    struct png_info_struct png_info;
    struct png_internal_struct png_internal;
    struct frac_struct frac;
    struct view_struct view;
    /*   basestruct base, sbase; */
    unsigned char* line, dummy;
    int xres, yres, i;
    char pngfile[256];

    strcpy( pngfile, pngf );
    ConvertPath( pngfile );

    if( ystart < 0 ) {
        ystart = 0;    /* Else bug when saving empty image. */
    }

    frac = *f;
    view = *v;
    xres = v->xres;

    if( IsStereo( v ) ) {
        xres *= 2;
    }

    if( zflag == 1 ) {
        xres *= view.antialiasing;
    }

    yres = v->yres;

    if( zflag == 1 ) {
        yres *= view.antialiasing;
    }

    line = malloc( ( size_t )xres * 3 + 2 );

    if( !line ) {
        sprintf( Error, "Not enough memory to save a line." );
        return( -1 );
    }

    if( PNGInitialization( pngfile, 1, &png, &png_info, &png_internal, xstart, ystart, calc_time,
                           &frac, &view, disppal, realpal, colscheme, cutbuf, zflag ) ) {
        sprintf( Error, "Error writing file '%s'\n", pngfile );
        free( line );
        return( -1 );
    }

    for( i = 0; i < yres; i++ ) {
        QU_getline( &line[1], i, xres, zflag );
        line[0] = 0; /* Set filter method */
        DoFiltering( &png_internal, line );

        if( WritePNGLine( &png_internal, line ) ) {
            sprintf( Error, "Error writing file '%s'\n", pngfile );
            EndPNG( &png_internal );
            free( line );
            return( -1 );
        }
    }

    i = EndIDAT( &png_internal );
    png_internal.length = 0;
    strncpy( png_internal.chunk_type, "IEND", 4 );
    i += WriteChunk( &png_internal, &dummy );
    i += fclose( png );
    EndPNG( &png_internal );
    free( line );

    if( i != 0 ) {
        sprintf( Error, "Error writing file '%s'\n", pngfile );
        return( -1 );
    }

    return( 0 );
}

int BuildName( char* name, char* namewop, const char* ext, const char* file, char* Error ) {
    char* c;

    strncpy( name, file, 256 );
    c = strrchr( name, '.' );

    if( c ) {
        strcpy( c, ext );
    } else {
        strcat( name, ext );
    }

    if( GetNextName( name, namewop ) ) {
        sprintf( Error, "Couldn't find a free filename based on '%s'.\n", name );
        return( -1 );
    }

    return( 0 );
}

int CleanNumberString( char* s ) {
    int i;

    if( strrchr( s, '.' ) != NULL ) {
        i = strlen( s ) - 1;

        while( s[i] == '0' ) {
            i--;
        }

        s[i + 1] = 0;

        if( s[i] == '.' ) {
            s[i] = 0;
        }
    }

    if( strlen( s ) == 0 ) {
        strcpy( s, "0" );
    }

    return( 0 );
}

int ReadParametersPNG( const char* fil, char* Error, struct frac_struct* frac, struct view_struct* view,
                       struct realpal_struct* realpal, char* colscheme, double* cutbuf ) {
    FILE* png;
    struct png_info_struct png_info;
    struct png_internal_struct png_internal;
    int xstart, ystart;
    int i;
    struct basestruct base, sbase;
    char file[256];

    strcpy( file, fil );
    ConvertPath( file );

    SetDefaults( frac, view, realpal, colscheme, cutbuf );

    if( ( png = fopen( file, "rb" ) ) == NULL ) {
        sprintf( Error, "Cannot open png-file '%s'.'", file );
        return( -1 );
    }

    if( InitPNG( png, &png_info, &png_internal ) == -1 ) {
        sprintf( Error, "File '%s' is not a valid png-file.", file );
        fclose( png );
        EndPNG( &png_internal );
        return( -1 );
    }

    i = ReadParameters( Error, &xstart, &ystart, &png_internal, frac, view, realpal, colscheme, cutbuf );

    /* if (i==-128) sprintf(Error, "Warning: File version higher than %s.",PROGVERSION); */
    if( i < 0 && i > -128 ) {
        return( -1 );
    }

    if( calcbase( &base, &sbase, *view, 0 ) != 0 ) {
        sprintf( Error, "Strange error in '%s'\nError in view_struct!", file );
        return( -1 );
    }

    fclose( png );
    EndPNG( &png_internal );

    return( 0 );
}

int WriteINI( char* Error, const char* fil, int mode, struct frac_struct* frac, struct view_struct* view,
              struct realpal_struct* realpal, char* colscheme, double* cut_buf ) {
    FILE* txt;
    int i, pos;
    vec3 n, p;
    char s1[30], s2[30], s3[30], s4[30], s5[30], s6[30], s7[30];
    char file[256];

    strcpy( file, fil );
    ConvertPath( file );

    if( ( txt = fopen( file, "w" ) ) == NULL ) {
        sprintf( Error, "Could not create file '%s'.", file );
        return( -1 );
    }

    fprintf( txt, "# This file was generated by '%s %s'.\n\n", PROGNAME, PROGVERSION );

    if( ( mode & PS_OTHER ) != 0 ) {
        fprintf( txt, "resolution %i %i %i\n", view->xres, view->yres, view->zres );
        sprintf( s1, "%.15f", view->phongmax );
        CleanNumberString( s1 );
        sprintf( s2, "%.15f", view->phongsharp );
        CleanNumberString( s2 );
        sprintf( s3, "%.15f", view->ambient );
        CleanNumberString( s3 );
        fprintf( txt, "phong %s %s\nambient %s\n", s1, s2, s3 );
        fprintf( txt, "antialiasing %i\n", view->antialiasing );
    }

    if( ( mode & PS_OBJ ) != 0 ) {
        fprintf( txt, "\n# OBJ\n" );
        sprintf( s1, "%.15f", frac->c[0] );
        CleanNumberString( s1 );
        sprintf( s2, "%.15f", frac->c[1] );
        CleanNumberString( s2 );
        sprintf( s3, "%.15f", frac->c[2] );
        CleanNumberString( s3 );
        sprintf( s4, "%.15f", frac->c[3] );
        CleanNumberString( s4 );
        fprintf( txt, "c %s %s %s %s\n\n", s1, s2, s3, s4 );
        sprintf( s1, "%.15f", frac->bailout );
        CleanNumberString( s1 );
        sprintf( s2, "%.15f", frac->lvalue );
        CleanNumberString( s2 );
        fprintf( txt, "bailout %s\nmaxiter %i\nlvalue %s\nformula %i\n\n",
                 s1, frac->maxiter, s2, frac->formula );

        for( i = 0; i < 4; ++i ) {
            sprintf( s1, "%.15f", frac->p[i][0] );
            CleanNumberString( s1 );
            sprintf( s2, "%.15f", frac->p[i][1] );
            CleanNumberString( s2 );
            sprintf( s3, "%.15f", frac->p[i][2] );
            CleanNumberString( s3 );
            sprintf( s4, "%.15f", frac->p[i][3] );
            CleanNumberString( s4 );
            fprintf( txt, "p%i %s %s %s %s\n", i + 1, s1, s2, s3, s4 );
        }

        fprintf( txt, "\n" );

    }

    if( ( mode & PS_VIEW ) != 0 ) {
        sprintf( s1, "%.15f", view->s[0] );
        CleanNumberString( s1 );
        sprintf( s2, "%.15f", view->s[1] );
        CleanNumberString( s2 );
        sprintf( s3, "%.15f", view->s[2] );
        CleanNumberString( s3 );
        fprintf( txt, "viewpoint %s %s %s\n", s1, s2, s3 );
        sprintf( s1, "%.15f", view->up[0] );
        CleanNumberString( s1 );
        sprintf( s2, "%.15f", view->up[1] );
        CleanNumberString( s2 );
        sprintf( s3, "%.15f", view->up[2] );
        CleanNumberString( s3 );
        fprintf( txt, "up %s %s %s\n", s1, s2, s3 );
        sprintf( s1, "%.15f", view->light[0] );
        CleanNumberString( s1 );
        sprintf( s2, "%.15f", view->light[1] );
        CleanNumberString( s2 );
        sprintf( s3, "%.15f", view->light[2] );
        CleanNumberString( s3 );
        fprintf( txt, "light %s %s %s\n\n", s1, s2, s3 );
        sprintf( s1, "%.15f", view->Mov[0] );
        CleanNumberString( s1 );
        sprintf( s2, "%.15f", view->Mov[1] );
        CleanNumberString( s2 );
        sprintf( s3, "%.15f", view->LXR );
        CleanNumberString( s3 );
        sprintf( s4, "%.15f", view->interocular );
        CleanNumberString( s4 );
        fprintf( txt, "move %s %s\nlxr %s\ninterocular %s\n\n", s1, s2, s3, s4 );
    }

    if( ( mode & PS_COL ) != 0 ) {
        fprintf( txt, "# COL\n" );
        fprintf( txt, "colorscheme %s\n", colscheme );

        for( i = 0; i < realpal->colnum; i++ ) {
            sprintf( s1, "%.15f", realpal->cols[i].weight );
            CleanNumberString( s1 );
            sprintf( s2, "%.15f", realpal->cols[i].col1[0] );
            CleanNumberString( s2 );
            sprintf( s3, "%.15f", realpal->cols[i].col1[1] );
            CleanNumberString( s3 );
            sprintf( s4, "%.15f", realpal->cols[i].col1[2] );
            CleanNumberString( s4 );

            if( realpal->cols[i].col1[0] == realpal->cols[i].col2[0]
                    && realpal->cols[i].col1[1] == realpal->cols[i].col2[1]
                    && realpal->cols[i].col1[2] == realpal->cols[i].col2[2] ) {
                fprintf( txt, "color  %s   %s %s %s\n", s1, s2, s3, s4 );
            } else {
                sprintf( s5, "%.15f", realpal->cols[i].col2[0] );
                CleanNumberString( s5 );
                sprintf( s6, "%.15f", realpal->cols[i].col2[1] );
                CleanNumberString( s6 );
                sprintf( s7, "%.15f", realpal->cols[i].col2[2] );
                CleanNumberString( s7 );
                fprintf( txt, "colorrange %s   %s %s %s    %s %s %s\n", s1, s2, s3, s4, s5, s6, s7 );
            }
        }
    }

    if( *( char* )&cut_buf[0] != CUT_TERMINATOR && ( ( mode & PS_OTHER ) != 0 ) ) {
        fprintf( txt, "\n# Intersection\n" );
        pos = 0;

        while( *( char* )&cut_buf[pos] != CUT_TERMINATOR ) {
            switch( *( char* )&cut_buf[pos] ) {
                case CUT_PLANE:
                    forall( i, 3 ) n[i] = cut_buf[pos + i + 1];
                    forall( i, 3 ) p[i] = cut_buf[pos + i + 4];
                    pos += 7;
                    sprintf( s1, "%.15f", n[0] );
                    CleanNumberString( s1 );
                    sprintf( s2, "%.15f", n[1] );
                    CleanNumberString( s2 );
                    sprintf( s3, "%.15f", n[2] );
                    CleanNumberString( s3 );
                    sprintf( s4, "%.15f", p[0] );
                    CleanNumberString( s4 );
                    sprintf( s5, "%.15f", p[1] );
                    CleanNumberString( s5 );
                    sprintf( s6, "%.15f", p[2] );
                    CleanNumberString( s6 );
                    fprintf( txt, "plane  %s %s %s   %s %s %s\n", s1, s2, s3, s4, s5, s6 );
                    break;

                default :
                    pos++;
            }
        }
    }

    fclose( txt );
    return( 0 );
}

int GetParameters( const char* afile, char* Error ) {
    char file[256], ini[256], *filewop;
    struct frac_struct frac;
    struct view_struct view;
    struct realpal_struct realpal;
    char colscheme[251];
    double cutbuf[140];

    strcpy( file, afile );
    ConvertPath( file );
    /*   for (i=0; i<(int)strlen(afile)+1; i++) file[i] = tolower((int)afile[i]);
    */
    filewop = strrchr( file, slash );

    if( filewop == NULL ) {
        filewop = file;
    } else {
        filewop++;
    }

    if( ReadParametersPNG( file, Error, &frac, &view, &realpal, colscheme, &cutbuf[0] ) != 0 ) {
        return( -1 );
    }

    if( BuildName( ini, NULL, ".ini", filewop, Error ) ) {
        return( -1 );
    }

    if( !WriteINI( Error, ini, PS_OTHER | PS_OBJ | PS_VIEW | PS_COL,
                   &frac, &view, &realpal, colscheme, cutbuf ) ) {
        sprintf( Error, "Generated file '%s'.", ini );
    } else {
        return( -1 );
    }

    return( 0 );
}

int ImgFromZBuf( const char* file, const char* file2, char* Error )
/* file is a ZBuffer, which gets turned into an image */
/* called by command-line versions (non-Windows), which are given just a file */
{
    char zpnfile[256], pngfile[256];
    char* s, *s2;

    strcpy( zpnfile, file );
    ConvertPath( zpnfile );
    strcpy( pngfile, zpnfile );
    /*   i = i;
       strncpy(zpnpath, file, 256);
       s = strrchr(zpnpath, '/');
       if (s==NULL) zpnpath[0]=0;
       else *(++s) = 0; */
    /*   zpnfile[0]=0;
       pngfile[0]=0;
    */

    /*   strncpy(zpnfile, file, 256);
       strncpy(pngfile, file, 256);
    */
    /* Make a pointer to the filename (without path) */
    if( ( s2 = strrchr( pngfile, slash ) ) == NULL ) {
        s2 = pngfile;
    } else {
        s2++;
    }

    /* Delete suffixes and append .png */
    if( ( s = strstr( s2, "." ) ) == NULL ) {
        strcat( pngfile, ".png" );
    } else {
        strcpy( s, ".png" );
    }

    if( GetNextName( pngfile, NULL ) != 0 ) {
        sprintf( Error, "Couldn't find a free filename based on '%s'\n", pngfile );
        return( -1 );
    }

    return( CalculatePNG( zpnfile, pngfile, Error, file2, 2 ) );
}

