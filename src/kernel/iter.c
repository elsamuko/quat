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

#include <math.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "iter.h" 
#include "qmath.h" 

int DoCalcbase(struct basestruct *base, struct basestruct *sbase, struct view_struct *v,
   int flag, vec3 proj_up);
int cutaway(vec3 x, double *cutbuf);
int cutnorm(point x1, point x2, point n, double *cutbuf);
/* Experimental functions not being used at the moment */
int extra_iter(point xstart, point c, double bailout, int maxiter, point lastorbit, int *close);
double maxabs_0(point c, double lower);
int newton(struct calc_struct *c, double zstart,/* int actiter*/ double boundary, double tdelta,
   double deltafn, double deltaconv, double *result);
double obj_distance_o(struct calc_struct *c);
double obj_distance_p(struct calc_struct *c);
double obj_distance(struct calc_struct *c);

extern point *orbit; /* Defined in quat.c */

FILE *logfile;       
                   
int iterate_0(struct iter_struct* is)
{
   static point xs = { 0.0, 0.0, 0.0, 0.0};	/* orbit[iter] to square */
   static double xss = 0.0; 			/* sum of components of xs */
   static double tmp = 0.0;
   static int iter = 0, olditer = 0, i = 0;	/* olditer := iter - 1 (to save calculations) */
   static point *orbit;
   
   assert(is->orbit != NULL); 
   orbit = is->orbit;

   xss = 0.0;
   forall(i,4)
   {
      is->orbit[0][i] = is->xstart[i];
      xs[i] = is->orbit[0][i]*is->orbit[0][i];
      xss += xs[i];
   }

   iter = 0; olditer = -1;
   while ( (is->exactiter==0 && xss<is->bailout && iter<is->maxiter)
         ||(is->exactiter!=0 && iter<is->maxiter))
   {
	iter++; olditer++;
      tmp = orbit[olditer][0] * orbit[olditer][1]; orbit[iter][1] = tmp + tmp - is->c[1];
      tmp = orbit[olditer][0] * orbit[olditer][2]; orbit[iter][2] = tmp + tmp - is->c[2];
      tmp = orbit[olditer][0] * orbit[olditer][3]; orbit[iter][3] = tmp + tmp - is->c[3];
      orbit[iter][0] = xs[0] + xs[0] - xss - is->c[0];

	xs[0] = orbit[iter][0]*orbit[iter][0];
	xss = xs[0];
	xs[1] = orbit[iter][1]*orbit[iter][1];
	xss += xs[1];
	xs[2] = orbit[iter][2]*orbit[iter][2];
	xss += xs[2];
	xs[3] = orbit[iter][3]*orbit[iter][3];
	xss += xs[3];	

/*      xss = 0.0;
      forall(i,4)
      {
         xs[i] = orbit[iter][i]*orbit[iter][i];
         xss += xs[i];
      }*/
/*      if (xss>1E100) return(-1);*/
   }  

   return(iter);
}                      

double maxabs_0(point c, double lower)                  
/* returns the maximum absolute value that can be achieved with a value x (|x|=lower)
   in one single iteration */
{
   double absc, absx;
   
   absx = lower*lower;  /* x^2 */
   absc = sqrt(c[0]*c[0]+c[1]*c[1]+c[2]*c[2]+c[3]*c[3]);  /* |c| */
   return(absx+absc);   /* x^2+|c| */
}

int iternorm_0(point xstart, point c, vec3 norm, double bailout, int maxiter)
{
   static point x, ox;
   static point xs;   /* x to square */   
   static double xss; /* sum of components of xs */
   static double diff[4][3], odiff[4][3]; 
   /* differentiations of 1st index to 2nd index */
   /* could be called Jacobi-matrix */
   static double tmp = 0;
   static int iter, i, j;
   
   xss = 0; iter = 0;
   for (j=0; j<3; j++) for (i=0; i<4; i++) odiff[i][j] = 0;
   odiff[0][0] = 1; odiff[1][1] = 1; odiff[2][2] = 1;
   for (i=0; i<4; i++) 
   {
      ox[i] = xstart[i];
      xs[i] = ox[i]*ox[i];
      xss += xs[i];
   }
   while ( xss<bailout && iter<maxiter )
   {
      /* usual quaternion z^2-c iteration */
      tmp = ox[0] * ox[1]; x[1] = tmp + tmp - c[1];
      tmp = ox[0] * ox[2]; x[2] = tmp + tmp - c[2];
      tmp = ox[0] * ox[3]; x[3] = tmp + tmp - c[3];
      x[0] = xs[0] + xs[0] -xss - c[0]; 
      
      /* iteration of gradient formula */ 
      for (i=0; i<3; i++)
      {
         diff[0][i] = 2*(ox[0]*odiff[0][i] - ox[1]*odiff[1][i] - ox[2]*odiff[2][i] - ox[3]*odiff[3][i]);
         diff[1][i] = 2*(ox[0]*odiff[1][i] + ox[1]*odiff[0][i]);
         diff[2][i] = 2*(ox[0]*odiff[2][i] + ox[2]*odiff[0][i]);      
         diff[3][i] = 2*(ox[0]*odiff[3][i] + ox[3]*odiff[0][i]);
      }
      xss = 0;
      for (i=0; i<4; i++)
      {
         xs[i] = x[i]*x[i];
         xss += xs[i];
         ox[i] = x[i];
      }
      for (j=0; j<3; j++) for (i=0; i<4; i++) odiff[i][j] = diff[i][j];
      iter++;
   }
   xss = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2] + x[3]*x[3]);
   forall(i,3) norm[i] = (x[0]*diff[0][i] + x[1]*diff[1][i] + x[2]*diff[2][i]
      + x[3]*diff[3][i])/xss;
   return(iter);
}
   

int iterate_1(struct iter_struct* is)
{
	static double re2 = 0.0, pure2 = 0.0, tmp1 = 0.0, tmp2 = 0.0;
	static int iter=0, olditer=0;
	static double a = 0.0, b = 0.0, c = 0.0, d = 0.0;
	static point *orbit;

	assert(is->orbit != NULL);
	orbit = is->orbit;

	a = is->c[0]; b = is->c[1]; c = is->c[2]; d = is->c[3];

	orbit[0][0] = is->xstart[0]; re2    = is->xstart[0]*is->xstart[0];
	orbit[0][1] = is->xstart[1]; pure2  = is->xstart[1]*is->xstart[1];
	orbit[0][2] = is->xstart[2]; pure2 += is->xstart[2]*is->xstart[2];
	orbit[0][3] = is->xstart[3]; pure2 += is->xstart[3]*is->xstart[3];

	iter = 0; olditer = 0;
	while ( (is->exactiter==0 && (re2+pure2)<is->bailout && iter<is->maxiter)
	   || (is->exactiter!=0 && iter<is->maxiter) )
	{
		++iter;
		tmp1 = orbit[olditer][0]*(1.0-orbit[olditer][0]);
		tmp1 += pure2;
		tmp2 = 1.0-2.0*orbit[olditer][0];
	
		orbit[iter][0] = a*tmp1 - tmp2 *
		  ( b*orbit[olditer][1] + c*orbit[olditer][2] + d*orbit[olditer][3]);
		orbit[iter][1] = b*tmp1 - tmp2 *
		  (-a*orbit[olditer][1] + d*orbit[olditer][2] - c*orbit[olditer][3]);
		orbit[iter][2] = c*tmp1 - tmp2 *
		  (-d*orbit[olditer][1] - a*orbit[olditer][2] + b*orbit[olditer][3]);
		orbit[iter][3] = d*tmp1 - tmp2 *
		  ( c*orbit[olditer][1] - b*orbit[olditer][2] - a*orbit[olditer][3]);
	
		++olditer;
		re2    = orbit[iter][0]*orbit[iter][0];
		pure2  = orbit[iter][1]*orbit[iter][1];
		pure2 += orbit[iter][2]*orbit[iter][2];
		pure2 += orbit[iter][3]*orbit[iter][3];
	
		if (re2+pure2>1E100) return -1;
	}  
	return iter;
}

int iternorm_1(point xstart, point c, vec3 norm, double bailout, int maxiter)
{
   static point x1, x2, x3, x4, x5;
   static point xs;   /* x to square */   
   static double xss; /* sum of components of xs */
   static point diff[3], odiff[3];
/*   static double tmp = 0; */
   static int iter, i, j;
   static point one = {1,0,0,0};
   
   xss = 0; iter = 0;
   for (i=0; i<3; i++) for (j=0; j<4; j++) odiff[i][j] = 0;
   /* 1st index: differentiation for x,y,z */
   /* 2nd index: components (e,j,k,l) */
   odiff[0][0] = 1;
   odiff[1][1] = 1;
   odiff[2][2] = 1;
   for (i=0; i<4; i++) 
   {
      x1[i] = xstart[i];
      xs[i] = x1[i]*x1[i];
      xss += xs[i];
   }
   while ( xss<bailout && iter<maxiter )
   {
      /* usual iteration */
      forall(i,4) x3[i] = x1[i];
      q_sub(x2, one, x1);      /* 1 - x[n] */
      q_mul(x4, c, x3);        /* c*x[n] */
      q_mul(x5, x4, x2);       /* x[n+1] = c*x[n]*(1-x[n]) */
      
      /* iteration of gradient formula */ 
      forall(i,3)
      {
         diff[i][0] = odiff[i][0]*(-2*c[0]*x1[0] + 2*c[1]*x1[1] + 2*c[2]*x1[2] + 2*c[3]*x1[3] + c[0])
            +odiff[i][1]*( 2*c[0]*x1[1] + 2*c[1]*x1[0] - c[1])
            +odiff[i][2]*( 2*c[0]*x1[2] + 2*c[2]*x1[0] - c[2])
            +odiff[i][3]*( 2*c[0]*x1[3] + 2*c[3]*x1[0] - c[3]);
         diff[i][1] = odiff[i][0]*(-2*c[1]*x1[0] - 2*c[0]*x1[1] + 2*c[3]*x1[2] - 2*c[2]*x1[3] + c[1])
            +odiff[i][1]*( 2*c[1]*x1[1] - 2*c[0]*x1[0] + c[0])
            +odiff[i][2]*( 2*c[1]*x1[2] + 2*c[3]*x1[0] - c[3])
            +odiff[i][3]*( 2*c[1]*x1[3] - 2*c[2]*x1[0] + c[2]);
         diff[i][2] = odiff[i][0]*(-2*c[2]*x1[0] - 2*c[3]*x1[1] - 2*c[0]*x1[2] + 2*c[1]*x1[3] + c[2])
            +odiff[i][1]*( 2*c[2]*x1[1] - 2*c[3]*x1[0] + c[3])
            +odiff[i][2]*( 2*c[2]*x1[2] - 2*c[0]*x1[0] + c[0])
            +odiff[i][3]*( 2*c[2]*x1[3] + 2*c[1]*x1[0] - c[1]);
         diff[i][3] = odiff[i][0]*(-2*c[3]*x1[0] + 2*c[2]*x1[1] - 2*c[1]*x1[2] - 2*c[0]*x1[3] + c[3])
            +odiff[i][1]*( 2*c[3]*x1[1] + 2*c[2]*x1[0] - c[2])
            +odiff[i][2]*( 2*c[3]*x1[2] - 2*c[1]*x1[0] + c[1])
            +odiff[i][3]*( 2*c[3]*x1[3] - 2*c[0]*x1[0] + c[0]);
      }
      
      forall(i,4) x1[i] = x5[i];
      for (i=0; i<3; i++) for (j=0; j<4; j++) odiff[i][j] = diff[i][j];
      /* check whether range of double will be enough / underflow? */
      xss = 0;
      forall(j,3) forall(i,4) xss += diff[j][i]*diff[j][i];
      if (xss<1E-16) break;  /* go out of loop, normal vec3 will get zero; do approx. */
      if (xss>1E17) break;   /* go out of loop before overflow; do approximate */
      xss = 0;
      for (i=0; i<4; i++)
      {
         xs[i] = x1[i]*x1[i];
         xss += xs[i];
      }
      iter++;
   }
   xss = sqrt(x1[0]*x1[0] + x1[1]*x1[1] + x1[2]*x1[2] + x1[3]*x1[3]);
   norm[0] = 1; norm[1] = 0; norm[2] = 0;
   if (xss==0) return(0);
   forall(i,3) norm[i] = (x1[0]*diff[i][0] + x1[1]*diff[i][1] + x1[2]*diff[i][2]
      + x1[3]*diff[i][3])/xss;
   /* Above because |x[maxiter]| is what we want to have differentiated partially */
/*   if (norm[0]==0 && norm[1]==0 && norm[2]==0) norm[0] = 1; */
   return(iter);
}
   
int iterate_2(struct iter_struct* is)
{
	static double re2 = 0.0, pure2 = 0.0, pure = 0.0;
	static double lntv = 0.0;
	static double atan2vt = 0.0, atan2vta = 0.0;
	static int iter = 0, olditer = 0;
	static point *orbit;
	
	assert(is->orbit != NULL);
	orbit = is->orbit;
	
	memcpy(orbit[0], is->xstart, sizeof(point));
	re2    = orbit[0][0]*orbit[0][0];
	pure2  = orbit[0][1]*orbit[0][1];
	pure2 += orbit[0][2]*orbit[0][2];
	pure2 += orbit[0][3]*orbit[0][3];

	iter = 0; olditer = 0;
	while ( (is->exactiter==0 && re2+pure2<is->bailout && iter<is->maxiter)
		|| (is->exactiter!=0 && iter<is->maxiter) )
	{
		++iter;
		lntv = 0.5*log(re2+pure2);
		pure = sqrt(pure2);
		if (pure == 0.0) {
			orbit[iter][0] = lntv*orbit[olditer][0] - is->c[0];
			orbit[iter][1] = atan2(0.0, orbit[olditer][0])-is->c[1];
			orbit[iter][2] = -is->c[2];
			orbit[iter][3] = -is->c[3];
		}
		else {
			atan2vt = atan2(pure, orbit[olditer][0]);
			atan2vta = atan2vt*orbit[olditer][0]/pure + lntv;
			orbit[iter][0] = lntv*orbit[olditer][0] - atan2vt*pure - is->c[0];
			orbit[iter][1] = atan2vta*orbit[olditer][1] - is->c[1];
			orbit[iter][2] = atan2vta*orbit[olditer][2] - is->c[2];
			orbit[iter][3] = atan2vta*orbit[olditer][3] - is->c[3];
		}
		++olditer;

		re2    = orbit[iter][0]*orbit[iter][0];
		pure2  = orbit[iter][1]*orbit[iter][1];
		pure2 += orbit[iter][2]*orbit[iter][2];
		pure2 += orbit[iter][3]*orbit[iter][3];

		if (re2+pure2>1E100) return -1;
	}
	
	return iter;
}

int iterate_3(struct iter_struct *is)
{
	static point x2 = { 0.0, 0.0, 0.0, 0.0 };
	static double re2 = 0.0, pure2 = 0.0;
	static int iter = 0, olditer = 0;
	static point *orbit;
	
	assert(is->orbit != NULL);
	orbit = is->orbit;
	
	memcpy(orbit[0], is->xstart, sizeof(point));
	x2[0] = orbit[0][0]*orbit[0][0]; re2    = x2[0];
	x2[1] = orbit[0][1]*orbit[0][1]; pure2  = x2[1];
	x2[2] = orbit[0][2]*orbit[0][2]; pure2 += x2[2];
	x2[3] = orbit[0][3]*orbit[0][3]; pure2 += x2[3];
	
	iter = 0; olditer = 0;
	
	while ( (is->exactiter==0 && re2+pure2<is->bailout && iter<is->maxiter)
		|| (is->exactiter!=0 && iter<is->maxiter))
	{
		++iter;
		orbit[iter][0] = orbit[olditer][0]*(re2 - 3*pure2) - is->c[0];
		orbit[iter][1] = orbit[olditer][1]*(3*re2 - pure2) - is->c[1];
		orbit[iter][2] = orbit[olditer][2]*(3*re2 - pure2) - is->c[2];
		orbit[iter][3] = orbit[olditer][3]*(3*re2 - pure2) - is->c[3];
		++olditer;
		
		x2[0] = orbit[iter][0]*orbit[iter][0]; re2    = x2[0];
		x2[1] = orbit[iter][1]*orbit[iter][1]; pure2  = x2[1];
		x2[2] = orbit[iter][2]*orbit[iter][2]; pure2 += x2[2];
		x2[3] = orbit[iter][3]*orbit[iter][3]; pure2 += x2[3];

		if (re2+pure2 > 1E100) return -1;
	}

	return iter;
}

int iterate_4(struct iter_struct *is)
{
	static point px = { 0.0, 0.0, 0.0, 0.0 };
	static point xs = { 0.0, 0.0, 0.0, 0.0 };	/* x[n] to square */
	static double xss = 0.0; 			/* sum of components of xs */
	static int iter = 0, olditer = -1, i;
	static point *orbit;

	assert(is->orbit != NULL);
	orbit = is->orbit;

	xss = 0.0; 
	forall(i,4)
	{
		orbit[0][i] = is->xstart[i];
		xs[i] = is->xstart[i]*is->xstart[i];
		xss += xs[i];
	}

	iter = 0; olditer = -1; 
	while ( (is->exactiter==0 && xss<is->bailout && iter<is->maxiter)
		|| (is->exactiter!=0 && iter<is->maxiter) )
	{
		++iter; ++olditer;
		q_pow(px, orbit[olditer], is->p[0]);		/* x^p1 */
		q_sub(orbit[iter], px, is->c);			/* x^p1 - c */
		xss = 0.0;
		forall(i,4)
		{
			xs[i] = orbit[iter][i]*orbit[iter][i];
			xss += xs[i];
		}
		if (xss>1E100) return -1;
	}
	return iter;
}

int extra_iter(point xstart, point c, double bailout, int maxiter, point lastorbit, int *close)
/* Only to achieve values like "eorbit", "closest_iteration" */
{
   point x;
   point xs;   /* x to square */   
   double xss; /* sum of components of xs */
   double tmp = 0;
   int iter, i, mindistiter;
   double mindist;
                     
   xss = 0; iter = 0;
   forall(i,4)
   {
      x[i] = xstart[i];
      xs[i] = x[i]*x[i];
      xss += xs[i];
   }
   mindist = xss; mindistiter = 0;
   while ( xss<bailout && iter<maxiter )
   {
      tmp = x[0] * x[1]; x[1] = tmp + tmp - c[1];
      tmp = x[0] * x[2]; x[2] = tmp + tmp - c[2];
      tmp = x[0] * x[3]; x[3] = tmp + tmp - c[3];
      x[0] = xs[0] + xs[0] - xss - c[0]; 
      xss = 0;
      forall(i,4)
      {
         xs[i] = x[i]*x[i];
         xss += xs[i];
      }          
      if (xss<mindist)
      {
         mindist = xss; mindistiter = iter+1;
      }
      if (xss<bailout) iter++;
   }            
   forall(i,4) lastorbit[i] = x[i];      
   *close = mindistiter;
   return(iter);
}

int DoCalcbase(struct basestruct *base, struct basestruct *sbase, struct view_struct *v, 
   int flag, vec3 proj_up)
{
   /* flag: 0 ... don´t use proj_up */
   /*       1 ... use proj_up */
   
   double leny = 0.0;
   double absolute = 0.0, lambda = 0.0;
   vec3 ss, tmp;
   int i;

   if (v->xres==0) return -1;
   leny = v->LXR*v->yres/v->xres;

   /* Norm of z */
   absolute = sqrt(scalarprod(v->s,v->s));
   if (absolute==0) return(-1);
   forall(i,3) base->z[i] = -v->s[i]/absolute;

   /* Norm of up */
   absolute = sqrt(scalarprod(v->up,v->up));
   if (absolute==0) return(-1);
   forall(i,3) v->up[i] /= absolute;

   /* check whether up is multiply of z */
   /* cross product != 0 */
   if ( v->up[1]*base->z[2] - v->up[2]*base->z[1] == 0
      && v->up[2]*base->z[0] - v->up[0]*base->z[2] == 0
      && v->up[0]*base->z[1] - v->up[1]*base->z[0] == 0) return(-1);

   if (flag==0)
   {
      /* Check whether up orthogonal to z */
      if (scalarprod(base->z,v->up) == 0.0)
      {
         /* Yes -> norm(up) = -y-vec3 */
         forall(i,3) base->y[i] = -v->up[i];
      }
      else
      {
         /* calculate projection of up onto pi */
         forall(i,3) tmp[i] = v->s[i] - v->up[i];
         lambda = scalarprod(base->z,tmp);
         forall(i,3) ss[i] = lambda*base->z[i] + v->up[i];
      
         /* tmp-vec3: tmp = s-ss */
         forall(i,3) tmp[i] = v->s[i] - ss[i];
         absolute = sqrt(scalarprod(tmp,tmp));
         assert(absolute != 0.0);

         /* with this: y-vector */                     
         forall(i,3) base->y[i] = tmp[i]/absolute;
      }
   }
   else forall(i,3) base->y[i] = proj_up[i];

   /* calculate x-vector (through cross product) */
   base->x[0] = base->y[1]*base->z[2] - base->y[2]*base->z[1];
   base->x[1] = base->y[2]*base->z[0] - base->y[0]*base->z[2];
   base->x[2] = base->y[0]*base->z[1] - base->y[1]*base->z[0];

   /* calculate origin */
   leny /= 2; v->LXR /= 2;
   forall(i,3) base->O[i] = v->s[i] - leny*base->y[i] - v->LXR*base->x[i];

   /* ready with base, now: calculate a specially normated base */
   /* where the length of the base vectors represent 1 pixel */
   
   if (sbase == NULL) return(0);
   
   if (v->yres==0) return -1;

   v->LXR *= 2; leny *= 2;
   forall(i,3)
   {
      sbase->O[i] = base->O[i];
      sbase->x[i] = v->LXR*base->x[i]/v->xres;
      sbase->y[i] = leny*base->y[i]/v->yres;
   }

   if (v->zres==0) return -1;

   /* how deep into scene */
   absolute = fabs(scalarprod(base->z,v->s));
   absolute *= 2;
   forall(i,3) sbase->z[i] = absolute*base->z[i]/v->zres;

   /* Only thing that has to be done: shift the plane */
   return(0);
}          

int calcbase(struct basestruct *base, struct basestruct *sbase, struct view_struct v, int flag)
/* flag:  0 ... common base */
/*       -1 ... left eye base */
/*        1 ... right eye base */
{         
   int e, i;
   struct basestruct commonbase;             
   vec3 y;
   
   e = DoCalcbase(base, sbase, &v, 0, y);
   commonbase = *base;
   if (e!=0) return(e);
   if (flag != 0)
   {
      struct view_struct v2;
       
      v2 = v;
      forall(i,3) v2.s[i] += (double)flag*v.interocular/2*base->x[i];
      forall(i,3) y[i] = base->y[i];
      e = DoCalcbase(base, sbase, &v2, 1, y);
      if (e!=0) return(e);
   }                                

   /* Shift plane in direction of common plane */
   forall(i,3) base->O[i] += v.Mov[0]*commonbase.x[i] + v.Mov[1]*commonbase.y[i];
   if (sbase!=NULL) forall(i,3) sbase->O[i] = base->O[i];

   return(0);
}

int cutaway(vec3 x, double *cutbuf)
{
   int i, pos, calc;
   vec3 y, n, p;
  
   pos = 0; calc = 1;
   while (*(char *)&cutbuf[pos]!=CUT_TERMINATOR && calc==1)
   {
      switch (*(char *)&cutbuf[pos])
      {
         case CUT_PLANE:
            forall(i,3) n[i] = cutbuf[pos+1+i];
            forall(i,3) p[i] = cutbuf[pos+4+i];
            forall(i,3) y[i] = x[i] - p[i];
            if (scalarprod(n,y)>0) calc = 0;
            pos += 7;
            break;
         default: pos++;
      }
   }
   return(calc);
}

int cutnorm(point x1, point x2, point n, double *cutbuf)
{
   int i, pos, flag, sign1, sign2;
   point y1, y2, n2, p; 
   
   pos = 0; flag = 0;
   while (*(char *)&cutbuf[pos]!=CUT_TERMINATOR  && flag==0)
   {
      switch (*(char *)&cutbuf[pos])
      {
         case CUT_PLANE:
            forall(i,3) n2[i] = cutbuf[pos+1+i]; 
            forall(i,3) p[i] = cutbuf[pos+4+i];
            forall(i,3) y1[i] = x1[i] - p[i];
            forall(i,3) y2[i] = x2[i] - p[i];
            if (scalarprod(n2, y1)>0) sign1 = 1; else sign1 = -1;
            if (scalarprod(n2, y2)>0) sign2 = 1; else sign2 = -1;
            if (sign1!=sign2) flag = 1;
            pos += 7;
      }
   }
   forall(i,3) n[i] = n2[i];
   return(0);
}  

int newton(struct calc_struct *c, double zstart/*, int actiter*/, double boundary, double tdelta,
   double deltafn, double deltaconv, double *result)
{ 
   double z, zold=0, xss1=1, xss2=2, ta=0, taold, conv, convold;
   int i;
   long ct;
   point xr;
   
   xr[3] = c->f.lvalue;
   z = zstart;
   conv = 1E6; xss1 = 1E6; ct = 0;
   while (fabs(xss1)>deltafn && conv>deltaconv)
   {                              
      ct++;
      forall(i,3) xr[i] = c->xcalc[i] + z*c->sbase.z[i];
/*      iterate_0(xr, c->f.c, c->f.bailout, c->f.maxiter, 0, NULL); */
      forall(i,3) xr[i] += tdelta*c->sbase.z[i];
/*      iterate_0(xr, c->f.c, c->f.bailout, c->f.maxiter, 0, NULL); */
      taold = ta;  
      ta = xss2-xss1; 
      if (ct==1) taold = ta;
      xss1 -= boundary;
      if (ta*taold<=0) return(-3); /* high- or low-point inbetween */
      convold = conv;
      conv = fabs(z-zold);                            
      if (convold<conv) return(-4);   /* no convergence */

      zold = z;
      if (fabs(ta)<1E-10) return(-1);    /* Division by zero */
      z = z - tdelta*xss1/ta;
      if (z>=c->v.zres) 
      {
         *result = c->v.zres;
         return(-2);               /* interval out of calculation "box" */
      }
   }                                                     
   /* point found */
   *result = z;
   return(1); 
}

double obj_distance(struct calc_struct *c)
/* determines the distance from object to point c->xcalc on viewplane in z direction */
/* in other words: looks for the object in z direction */ 
{
   int z, z2, iter, i;
   struct iter_struct is;

   assert(orbit != NULL);

	forall(i,4) is.c[i] = c->f.c[i];
	is.bailout = c->f.bailout;
	is.maxiter = c->f.maxiter;
	is.exactiter = 0;
	forall(i,4) {
		is.p[0][i] = c->f.p[0][i]; is.p[1][i] = c->f.p[1][i]; 
		is.p[2][i] = c->f.p[2][i]; is.p[3][i] = c->f.p[3][i];
	}
	is.orbit = orbit;
	is.xstart[3] = c->f.lvalue; 
   iter = 0;
   for (z=0; z<c->v.zres && iter!=c->f.maxiter; z++)
   {
      forall(i,3) is.xstart[i] = c->xcalc[i] + z*c->sbase.z[i];
      if (cutaway(is.xstart, c->cutbuf)==1)
      {
         iter = c->iterate(&is);
      }
      else iter = 0;
   }
   if (z!=c->v.zres)
   {
      z--;
      for(z2=1; z2<21 && iter==c->f.maxiter; z2++)
      {
         forall(i,3) is.xstart[i] = c->xcalc[i] + ((double)z-(double)z2/20)*c->sbase.z[i];  
         if (cutaway(is.xstart, c->cutbuf)==1)
         {
            iter = c->iterate(&is);
         }
         else iter = 0;
      }
      z2 -= 2;
   }
   else z2=0;       
   return((double)z-(double)z2/20);
}

float brightness(const vec3 ls, const vec3 p, const vec3 n, const vec3 z, double phongmax, double phongsharp, 
   double ambient)
{
   /* values >1 are for phong */
   vec3 l, r;
   double absolute, result, a, b, c;
   int i;
   
   /* vector point -> light source */
   forall(i,3) l[i] = ls[i] - p[i];
   absolute = sqrt(scalarprod(l,l));
   if (absolute==0) return((float)255);   /* light source and point are the same! */
   forall(i,3) l[i] /= absolute;
   
   /* Lambert */
   a = scalarprod(n,l);
   
   /* Distance */
   b = 20/absolute; if (b>1) b = 1;

   /* Phong */
   if (a>0)
   {
      forall(i,3) r[i] = 2*a*n[i] - l[i]; 
      /* r...reflected ray */
      c = scalarprod(r,z);
      if (c<0) c = phongmax*exp(phongsharp*log(-c));
      else c = 0;
   }      
   else c = 0;
                                              
   if (a<0) result = ambient;
   else
   { 
      result = a*b+ambient;
      if (result>1.0) result = 1.0;   /* Lambert and ambient together can't get bigger than 1 */
      result += c;                    /* additional phong to get white */
      if (result<0.0) result = 0;
   }
   return((float)result);
}

                         
float brightpoint(long x, int y, float *LBuf, struct calc_struct *c/*, int zflag, vec3 n*/)
/* calculate a brightness value (0.0 ... 1.0) */
/* all fields (except xq) in calc_struct MUST be initialized */
/* should only be called if it´s sure that the object was hit! */
/* (LBuf != c->v.zres) */
/* zflag: 0..calc image from scratch; 1..calc ZBuffer from scratch; 2..calc image from ZBuffer */
{
   long i, xa, ya;
   point xp;
   vec3 n/*,  xp2*/;
   double absolute = 0.0;
   float depth, bright, BBuf, sqranti; 
                                                    
   xp[3] = c->f.lvalue;
   BBuf = (float)0;        
   sqranti = (float)(c->v.antialiasing*c->v.antialiasing);
   for (ya=0; ya<c->v.antialiasing; ya++)
      for (xa=0; xa<c->v.antialiasing; xa++)
      {
         forall(i,3) c->xcalc[i] = c->sbase.O[i] + ((double)x+xa/(double)c->v.antialiasing)
            *c->sbase.x[i]+ ((double)y+ya/(double)c->v.antialiasing)*c->sbase.y[i]; 
         depth = LBuf[((size_t)x+(size_t)(ya+1)*(size_t)c->v.xres)*(size_t)c->v.antialiasing+(size_t)xa];
         if (depth != c->v.zres)
         {
            forall(i,3) xp[i] = c->xcalc[i]+depth*c->sbase.z[i];
            /* Preserve point on object for colorizepoint */
            if (xa==0 && ya==0) forall(i,4) c->xq[i] = xp[i];
            /*if (zflag!=2)    usual calculation of normal vector 
            {
               forall(i,3) xp2[i] = xp[i] - 0.05*c->sbase.z[i];
               if (cutaway(xp2, c->cutbuf)==1)
                  c->iternorm(xp, c->f.c, n, c->f.bailout, c->f.maxiter);
               else cutnorm(xp, xp2, n, c->cutbuf); 
            }                                                       
            else */ /* use the ZBuffer */
            {                                                
               float dz1, dz2;                     
               
               dz1 = LBuf[((size_t)x+(size_t)ya*(size_t)c->v.xres)*(size_t)c->v.antialiasing+xa] - depth;
               if (x+xa>0) dz2 = LBuf[((size_t)x+(size_t)(ya+1)*(size_t)c->v.xres)*(size_t)c->v.antialiasing+(size_t)(xa-1)] - depth;
               else dz2 = (float)0;
               forall(i, 3) n[i] = -c->v.antialiasing*c->absx*c->absy/c->absz*c->sbase.z[i]
                  - dz2*c->absy*c->absz/c->absx*c->sbase.x[i] 
		  - dz1*c->absz*c->absx/c->absy*c->sbase.y[i];
		  /* For a correct cross product, each factor must be multiplied
		     with c->v.antialiasing, but as n gets normalized afterwards,
		     this calculation is not necessary for our purpose. */
            }
            absolute = sqrt(scalarprod(n,n));
            /* ensure that n points to viewer */
            /* if (scalarprod(n,z)>0) absolute = -absolute; */ 
            /* ideally there should stand >0 */ 
assert(absolute != 0.0);
            forall(i,3) n[i] /= absolute;
            bright = brightness(c->v.light, xp, n, c->base.z, c->v.phongmax, c->v.phongsharp, c->v.ambient);
assert(sqranti != 0.0);
            bright /= sqranti;
            BBuf += bright;
         }
      }
   return(BBuf);
}
 
int calcline(long x1, long x2, int y, float *LBuf, float *BBuf, float *CBuf,
   struct calc_struct *c, int zflag)
/* calculate a whole line of depths, brightnesses and colors */
/* All fields (for xs, xq, xcalc only ..[3]=lvalue) in calc_struct MUST be filled in! */
/* especially xp must be set to the beginning of the line to calculate */
/* c->xp used for 4d-beginning of line */
/* c->xs used for 4d-point in view plane that is worked on */
/* c->xq used for 4d-point on object (filled in by brightpoint, used by colorizepoint) */
/* c->xcalc used for 4d point to calculate (fctn. obj_distance) */      
/* (all to avoid double calculations, speed) */
/* zflag: 0..calc image from scratch; 1..calc ZBuffer from scratch; 2..calc image from ZBuffer */
{
   long x, xaa, yaa, i;
   struct iter_struct is;
/*   point orbit[c->f.maxiter+1];*/
/*	point xr;*/
/*   float depth;*/

   assert(orbit != NULL);

	forall(i,4) is.c[i] = c->f.c[i];
	is.bailout = c->f.bailout;
	is.maxiter = c->f.maxiter;
	is.exactiter = 0;
	forall(i,4) {
		is.p[0][i] = c->f.p[0][i]; is.p[1][i] = c->f.p[1][i]; 
		is.p[2][i] = c->f.p[2][i]; is.p[3][i] = c->f.p[3][i];
	}
	is.orbit = &orbit[1];
	is.xstart[3] = c->f.lvalue; 

   forall(i, 3) c->xs[i] = c->xp[i] + (x1-1)*c->sbase.x[i];
   for (x=x1; x<=x2; x++)
   {
      forall(i, 3) c->xs[i] += c->sbase.x[i];
      forall(i, 4) c->xcalc[i] = c->xs[i];
      if (zflag!=2) 
      {
         if ((LBuf[((size_t)x+(size_t)c->v.xres)*(size_t)c->v.antialiasing] 
            = (float)(floor(obj_distance(c)*100+0.5)/100))!=c->v.zres)
         {
            for (yaa=0; yaa<(long)c->v.antialiasing; yaa++)
               for (xaa=0; xaa<(long)c->v.antialiasing; xaa++) if (xaa!=0 || yaa!=0)
               {
                  forall(i,3) c->xcalc[i] = c->xs[i] + yaa*c->aabase.y[i] + xaa*c->aabase.x[i];
                  LBuf[((size_t)(yaa+1)*(size_t)c->v.xres+(size_t)x)*(size_t)c->v.antialiasing+(size_t)xaa]
                     = (float)(floor(obj_distance(c)*100+0.5)/100);
               }
         }
         else
         {
            for (yaa=0; yaa<(long)c->v.antialiasing; yaa++)
               for (xaa=0; xaa<(long)c->v.antialiasing; xaa++)
               LBuf[((size_t)(yaa+1)*(size_t)c->v.xres+(size_t)x)*(size_t)c->v.antialiasing+(size_t)xaa] = 
                  (float)c->v.zres;
         }   
      }
      if (LBuf[((size_t)x+(size_t)c->v.xres)*(size_t)c->v.antialiasing] != c->v.zres)
      {        
         if (zflag==0 || zflag==2)     /* an image has to be calculated */
         {
            BBuf[x] = brightpoint(x, y, LBuf, c/*, zflag*/);
            if (BBuf[x]>0.0001)
		{
/*			xr[3] = c->f.lvalue;
			forall(i,3) xr[i] = c->xcalc[i] + z*c->sbase.z[i];	*/
			forall(i,4) is.xstart[i] = c->xq[i];
			c->iterate(&is);
			orbit[0][0] = (double)c->f.maxiter;
			CBuf[x] = colorizepoint(c);
		}
         }
      }
      else if (zflag==0 || zflag==2) BBuf[x] = (float)0;
   }
   return(0);
}

