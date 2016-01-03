#ifndef _COMMON_H
#define _COMMON_H

#define forall(i,n) for ((i)=0; (i)<(n); (i)++)
#define scalarprod(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])

#define PROGNAME "Quat"
/* Define VERSION through configure.in */
#ifdef VERSION
#define PROGVERSION VERSION
#else
/* WARNING: The following string MUST have 4 characters!!! */
/* (So use "1.20" instead of "1.2" !! ) */
#define PROGVERSION "1.20"
#endif
#define PROGSUBVERSION ""
/*#define PROGSTATE " development\n(Build: " __DATE__ ", " __TIME__ ")"*/
#define PROGSTATE ""
/*#define COMMENT "Development version - Please do not distribute."*/
#define COMMENT ""

/* define codes for "mode" in DoParamSave (win2.c) WriteINI (quat.c) */
#define PS_OBJ 1
#define PS_VIEW 2
#define PS_COL 4
#define PS_OTHER 8
#define PS_USEFILE 128

typedef double vec3[3];                    /* vector in 3d */
struct basestruct {                            /* origin and 3 base vectors */
    vec3 O, x, y, z;
};

typedef double point[4];                     /* point in 4d quaternion space */

/* intersection objects definitions (only planes for now) */
/* every structure in this section must have */
/* a char "cut_type" as first element */
#define CUT_TERMINATOR 0
#define CUT_PLANE 1
/* End of intersection objects definitions */

struct frac_struct {
    point c;
    double bailout;
    int maxiter;
    double lvalue;
    int formula;
    point p[4];
} ;

struct view_struct {
    vec3 s, up, light;
    double Mov[2];
    double LXR;
    int xres, yres, zres;
    double interocular;
    double phongmax, phongsharp, ambient;
    int antialiasing;
};

struct iter_struct;

struct calc_struct {
    point xp, xs, xq, xcalc;
    struct view_struct v;
    struct frac_struct f;
    struct basestruct sbase, base, aabase;
    double absx, absy, absz;
    double* cutbuf;
    int ( *iterate )( struct iter_struct* );
    int ( *iternorm )( point xstart, point c, point norm, double bailout, int maxiter );
} ;

struct col_struct {
    double weight;   /* Weight in a palette from 0.0 to 1.0 */
    /* 0 ... single color */
    /* >0 ... smoothly shaded to next color in */
    /* palette, the higher weight, the slower */
    double col1[3];  /* Red, green, blue from 0.0 to 1.0 */
    double col2[3];  /* dto. */
} ;

struct rgbcol_struct {
    short int r, g, b;
};

struct realpal_struct {
    struct col_struct cols[30];
    int colnum;
};

struct disppal_struct {
    struct rgbcol_struct cols[256];
    int colnum, brightnum;  /* maxcol = colnum*brightnum */
    int maxcol;
    double btoc;    /* "brights":colors */
    int rdepth, gdepth, bdepth;   /* Used by FindNearestColor. Should be filled if this is used. */
};

struct vidinfo_struct {
    int maxcol;
    int rdepth, gdepth, bdepth;
    double rgam, ggam, bgam;
};

#ifdef __cplusplus
extern "C" {
#endif

int IsStereo( struct view_struct* view );
float colorizepoint( /*float diffr, int closest_iteration,*/ struct calc_struct* c );

#ifdef __cplusplus
}
#endif
#endif
