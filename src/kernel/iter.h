#ifdef __cplusplus
extern "C" {
#endif

struct iter_struct {
    point xstart;
    point c;
    double bailout;
    int maxiter;
    int exactiter;
    point p[4];
    point* orbit;
};

int iterate_0( struct iter_struct* );
/* formula: x[n+1] = x[n]^2 - c */
/* xstart: 4 start iteration values */
/* c: same values as in x^2-c */
/* bailout: square of the value at which iteration is seen to be divergent */
/* maxiter: number of iterations after that iteration is seen to be convergent */
/* exactiter: !=0: bailout is ignored, iteration done up to maxiter. If orbit gets */
/*            too large for double-type, functions returns -1 */
/* returns number of iterations done before bailout */
/* fills in orbit: array of points (MUST hold up to maxiter points!) that make */
/*                 the orbit */

int iternorm_0( point xstart, point c, point norm, double bailout, int maxiter );
/* formula: x[n+1] = x[n]^2 - c */
/* iterates a point and calculates the normal vector "norm" at this point */
/* parameters same as "iterate_0" */
/* returns number of iterations done before bailout */

int iterate_1( struct iter_struct* );
/* formula: x[n+1] = c*x[n]*(1-x[n]^2) */
/* xstart: 4 start iteration values */
/* bailout: square of the value at which iteration is seen to be divergent */
/* maxiter: number of iterations after that iteration is seen to be convergent */
/* exactiter: !=0: bailout is ignored, iteration done up to maxiter. If orbit gets */
/*            too large for double-type, functions returns -1 */
/* returns number of iterations done before bailout */
/* fills in orbit: array of points (MUST hold up to maxiter points!) that make */
/*                 the orbit */

int iternorm_1( point xstart, point c, point norm, double bailout, int maxiter );
int iterate_2( struct iter_struct* );
int iterate_3( struct iter_struct* );
int iterate_4( struct iter_struct* );

float brightness( const vec3 ls, const vec3 p, const vec3 n, const vec3 z, double phongmax, double phongsharp, double ambient );
/* Calculates brightness (diffuse, distance, phong) for point p */
/* ls: vector origin to light source */
/* p: vector origin to lightened point */
/* n: normal vector (length 1 !!!) of object in lightened point */
/* z: vector point to viewer (length 1 !!!) */
/* returns brightness from 0.0 to 1.0 / or 255, if ls=p (=error) */

int calcline( long x1, long x2, int y, float* LBuf, float* BBuf, float* CBuf,
              struct calc_struct* c, int zflag );
/* calculates a whole line of depths (precision: 1/20th of the base step in z direction), */
/* Brightnesses and Colors */
/* y: the number of the "scan"line to calculate */
/* x1, x2: from x1 to x2 */
/* LBuf: buffer for xres floats. The depth information will be stored here */
/* BBuf: buffer for xres floats. The brightnesses will be stored here */
/* CBuf: buffer for xres floats. The colors will be stored here */
/* fields in calc_struct: */
/* sbase: the specially normalized base of the screen's coordinate-system */
/* f, v: structures with the fractal and view information */
/* zflag: 0..calc image from scratch;
          1..calc ZBuffer from scratch;
          2..calc image from ZBuffer */
/* if zflag==2, LBuf must be initialized with the depths from the ZBuffer */

int calcbase( struct basestruct* base, struct basestruct* sbase, struct view_struct v, int flag );
/* calculates the base and specially normalized base according to information in v */
/* base: will be set by function. Origin and base vectors in 3d "float" space */
/* sbase: will be set by function. Origin and base vectors specially normalized for 3d "integer" space */
/*        may be NULL */
/* v: view information from wich base and sbase will be calculated */
/* flag:  0 ... common base */
/*       -1 ... left eye base */
/*        1 ... right eye base */
/* returns -1 if view information makes no sense (a division by zero would occur) */
/* returns 0 if successful */

float brightpoint( long x, int y, float* LBuf, struct calc_struct* c/*, int zflag, vec3 n*/ );
/* calculate the brightness value of a pixel (0.0 ... 1.0) */
/* x: x coordinate of pixel on screen */
/* y: the number of the "scan"line to calculate */
/* LBuf: depth information for the pixel */
/* fields in calc_struct: */
/* base: normalized base */
/* sbase: specially normalized base */
/* f, v: structures with the fractal and view information */
/* returns brightness value */

#ifdef __cplusplus
}
#endif
