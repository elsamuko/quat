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

#include <string.h>
#include <stdio.h>    /* "sprintf" */
#include <ctype.h>    /* "tolower" */
#include <stdlib.h>
#include <math.h>
#include "calculat.h"

char f_set(double *a, const double *b);
char f_add(double *a, const double *b);
char f_sub(double *a, const double *b);
char f_mul(double *a, const double *b);
char f_div(double *a, const double *b);
char f_pow(double *a, const double *b);
char f_sin(double *a, const double *b);
char f_cos(double *a, const double *b);
char f_sqr(double *a, const double *b);
char f_sqrt(double *a, const double *b);
char f_exp(double *a, const double *b);
char f_ln(double *a, const double *b);
char f_atan(double *a, const double *b);
char f_asin(double *a, const double *b);
char f_acos(double *a, const double *b);
char f_round(double *a, const double *b);
char f_trunc(double *a, const double *b);
char f_abs(double *a, const double *b);
char f_tan(double *a, const double *b);
char f_random(double *a, const double *b);
void GetObjectBefore(char *b, char *s, int p);
void GetObjectBehind(char *b, char *s, int p);
int IsVar(const char *s, struct progtype *prog);
int VarNr(const char *s, struct progtype *prog);
int IsRegister(const char *s);
int IsFunction(const char *s, struct progtype *prog);
int AllocObjs(size_t len, char **a, char **b, char **c, char **d);
int FreeObjs(char *a, char *b, char *c, char *d);
int DoTranslate(char *wrong, char *a, struct progtype *prog);

char operators[20] = "+-*/^";
char numbers[20] = "0123456789.";
char registers[30] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char prio1[5] = "^";
char prio2[5] = "*/";
char prio3[5] = "+-";

double reg[26];
int regscount;
int ocount; 

/* number of functions defined the line below */
#define fncount 14
const char *functions[fncount] = { "sin", "cos", "sqr", "sqrt", "exp", "ln",
   "round", "trunc", "abs", "tan", "random", "atan", "asin", "acos" };

/* Define functions for mathematical operations */
char f_set(double *a, const double *b)
{
   *a = *b;
   return(0);
}         

char f_add(double *a, const double *b)
{
   *a = *a+(*b);
   return(0);
}               
      
char f_sub(double *a, const double *b)
{
   *a = *a-(*b);
   return(0);
}          

char f_mul(double *a, const double *b)
{
   *a = (*a)*(*b);
   return(0);
}            

char f_div(double *a, const double *b)
{         
   if (fabs(*b)<1E-30) return(1);   /* Division by zero */
   else *a = (*a)/(*b);
   return(0);
}               
   
char f_pow(double *a, const double *b)
{
   if (*a==0 && *b<=0) return(1);   /* would be division by zero or 0^0 */
   else
   {
      if (*a<0 && ceil(*b)!=*b) return(1);   /* would be root of neg. number */
      else *a = pow(*a, *b);
   }
   return(0);
}               

char f_sin(double *a, const double *b)
{
   *a = sin(*b);
   return(0);
}               

char f_cos(double *a, const double *b)
{
   *a = cos(*b);
   return(0);
}               

char f_sqr(double *a, const double *b)
{
   *a = (*b)*(*b);
   return(0);
}               

char f_sqrt(double *a, const double *b)
{           
   if (*b<0) return(1);
   *a = sqrt(*b);
   return(0);
}               

char f_exp(double *a, const double *b)
{
   *a = exp(*b);
   return(0);
}               

char f_ln(double *a, const double *b)
{
   if (*b<=0) return(1);
   else *a = log(*b);
   return(0);
}               

char f_atan(double *a, const double *b)
{
   *a = atan(*b);
   return(0);
}

char f_asin(double *a, const double *b)
{
   *a = asin(*b);
   return(0);
}

char f_acos(double *a, const double *b)
{
   *a = acos(*b);
   return(0);
}

char f_round(double *a, const double *b)
{           
   if (*b-floor(*b)>=0.5) *a = ceil(*b);
   else *a = floor(*b);
   return(0);
}               

char f_trunc(double *a, const double *b)
{
   *a = floor(*b);
   return(0);
}               

char f_abs(double *a, const double *b)
{
   *a = fabs(*b);
   return(0);
}               

char f_tan(double *a, const double *b)
{
   if (cos(*b)==0) return(1);
   else *a = tan(*b);
   return(0);
}               

char f_random(double *a, const double *b)
{
   *a = *b * rand()/RAND_MAX;
   return(0);
}               

void GetObjectBefore(char *b, char *s, int p)
{
   /* b ... string returned
      s ... string to look for object
      p ... position to start looking
   */

   int i = p-1;
/*   while (strchr(operators, *(s+i))==NULL && i>=0) i--; */
   while (i>=0 && strchr(operators, *(s+i))==NULL) --i;
   strcpy(b, s+i+1); *(b+p-i-1) = 0;
}

void GetObjectBehind(char *b, char *s, int p)
{
   /* b ... string returned
      s ... string to look for object
      p ... position to start looking
   */

   unsigned int i = p+1;
   while (strchr(operators, *(s+i))==NULL && i<=strlen(s)+1) i++;
   strcpy(b, s+p+1); *(b+i-p-1) = 0;
}

int IsVar(const char *s, struct progtype *prog)
{
   int i;

   i = 0;
   while ( i<prog->vardef && strcmp(s, prog->varnames[i])!=0 ) i++;
   if (strcmp(s, prog->varnames[i])!=0) return(0); else return(1);
}

int VarNr(const char *s, struct progtype *prog)
{
   int i;

   i = 0;
   while ( i<prog->vardef && strcmp(s, prog->varnames[i])!=0 ) i++;
   if ( strcmp(s, prog->varnames[i])==0 && i<prog->vardef ) return(i); else return(255);
}

int IsNumber(const char *s)
{                        
   char **endptr;
   char *p;
    
   if (s[0]==0) return(0);
   endptr = &p;
   strtod(s, endptr);
   if (**endptr!=0) return(0);
   return(1); 
}


int IsRegister(const char *s)
{
   unsigned int i;
   if (s[0]==0) return(0);
   if (strchr(registers, *s)!=NULL)
   {
      i = 1;
      while (*(s+i)==' '  && i<strlen(s)-1) i++;
      if (*(s+i)!=' ' && i!=1) return(0);
      return(1);
   }
   return(0);
}

int IsFunction(const char *s, struct progtype *prog)
/* return code >=0: number of integrated function */
/* >=0 and bit 7 set (>=128): number of declared function */
/* -1: error, none of the above */
{
   int i;
 
   for (i=0; ( i<prog->availfncount && strcmp(s, prog->avail_functions[i].name)!=0 ); i++); 
   if (i==prog->availfncount) 
   {
      for (i=0; ( i<fncount && strcmp(s, functions[i])!=0 ); i++);
      if (i==fncount) return(-1);
      else return(i);
   }
   return(i+128);
}                                                             

int DeclareFunction(const char *name, funcptr f, struct progtype *prog)
/* returns -1 if too many functions declared */
{
	int i;                                              
	i = IsFunction(name, prog);
	if ( i >= 128 ) 
	{
		prog->avail_functions[i-128].func = f;
		return 0;
	}
	if (prog->availfncount == 30) return(-1);
	prog->avail_functions[prog->availfncount].func = f;
	prog->avail_functions[prog->availfncount++].name = name;
	return(0);
}

/* Set value of variable / create new variable */
int SetVariable(const char *name, unsigned char *handle, double value, struct progtype *prog)
{
   int i;
   
   if (*handle<prog->vardef) prog->varvalues[*handle] = value; 
   else 
   {
      i = VarNr(name, prog);
      if (i==255)
      {
         if (prog->vardef==30) return(100);
         *handle = prog->vardef;
         strcpy(prog->varnames[(int)prog->vardef], name);
         prog->varvalues[(int)prog->vardef++] = value;
      }
      else
      {
         *handle = i;
         prog->varvalues[i] = value;
      }
   }
   return(-1);
}

int Init(struct progtype *prog)
{
   int i;

   prog->vardef = 0;        
   for (i=0; i<30; i++) prog->avail_functions[i].name = "";
   prog->availfncount = 0;
   prog->func[0] = NULL;
   return(0);
}
     
double calculate(char *notdef, struct progtype *prog)
{
   int i;

   *notdef = 0;
   for (i=0; prog->func[i]!=NULL && i<100 && *notdef==0; i++) 
      notdef += prog->func[i](prog->a[i], prog->b[i]);
   return(*(prog->a[i-1]));
}
                                                               
int AllocObjs(size_t len, char **a, char **b, char **c, char **d)
{
   *a = malloc(len+1);   /* +1 because windows seems to have problems with odd mallocs */
   *b = malloc(len+1);
   *c = malloc(len+1);
   *d = malloc(len+1);
   
   if ( (!*a) || (!*b) || (!*c) || (!*d) ) return(-1);
   return(0);
}  

int FreeObjs(char *a, char *b, char *c, char *d)
{
   free(d); free(c); free(b); free(a);
   return(0);
}
                                                               
int DoTranslate(char *wrong, char *a, struct progtype *prog)
{
   char *obj1, *obj2, *priori, *suba;
   unsigned int i, j;
   int k, sign;
   int bropen, brcount, error, fct;

   if (AllocObjs(strlen(a)+1, &obj1, &obj2, &priori, &suba)==-1) 
   /* "+1" because of memmove some lines later */
   {
      FreeObjs(obj1, obj2, priori, suba);
      sprintf(wrong, "No memory");
      return(100);
   }
   wrong[0] = 0;
   if (strchr(prio3, a[0])!=NULL)
   {
      memmove(a+1, a, strlen(a)+1);
      a[0] = '0';
   }
   bropen = -1; brcount = 0;
   for (j=0; j<strlen(a); j++)
   {
      if (a[j]=='(')
      {
         if (bropen==-1) bropen = j;
         brcount++;
      }
      if (a[j]==')') brcount--;
      if ((bropen!=-1) && (brcount==0))
      {
         strncpy(suba, a+bropen+1, j-(bropen+1));
         suba[j-(bropen+1)] = 0;
         error = DoTranslate(wrong, suba, prog);
         if (error!=0)
         {
            FreeObjs(obj1, obj2, priori, suba);
            return(error);
         }
         GetObjectBefore(obj1, a, bropen);
         strncpy(a+bropen, suba, j-(bropen+1));
         memset(a+j-1,' ',2);
         fct = IsFunction(obj1, prog);
         if (fct!=-1)
         {
            if ((fct & 128) != 0)
            {                                                                        
               /* Create command */
               prog->func[ocount] = prog->avail_functions[fct & ~128].func;
               prog->a[ocount] = &(reg[strchr(registers, suba[0])-&registers[0]]);
               prog->b[ocount] = &(reg[strchr(registers, suba[0])-&registers[0]]);
            }                                   
            else if (fct >= 0)
            {                      
               /* Declare new function */
               k = 0;
               switch (fct)
               {
                  case 0: k=DeclareFunction("sin", f_sin, prog); break;
                  case 1: k=DeclareFunction("cos", f_cos, prog); break;
                  case 2: k=DeclareFunction("sqr", f_sqr, prog); break;
                  case 3: k=DeclareFunction("sqrt", f_sqrt, prog); break;
                  case 4: k=DeclareFunction("exp", f_exp, prog); break;
                  case 5: k=DeclareFunction("ln", f_ln, prog); break;
                  case 6: k=DeclareFunction("round", f_round, prog); break;
                  case 7: k=DeclareFunction("trunc", f_trunc, prog); break;
                  case 8: k=DeclareFunction("abs", f_abs, prog); break;
                  case 9: k=DeclareFunction("tan", f_tan, prog); break;
                  case 10: k=DeclareFunction("random", f_random, prog); break;  
                  case 11: k=DeclareFunction("atan", f_atan, prog); break;
                  case 12: k=DeclareFunction("asin", f_asin, prog); break; 
                  case 13: k=DeclareFunction("acos", f_acos, prog); break; 
                  default: k=-255;
               }
               if (k==-1) 
               {
                  sprintf(wrong, "%s", "Too many functions declared (max. 30)");
                  return(-1);
               }                
               if (k==-255)
               {
                  sprintf(wrong, "%s", "Internal error #1");
                  return(-1);
               }                         
               /* Create command */    
               prog->func[ocount] = prog->avail_functions[prog->availfncount-1].func;
               prog->a[ocount] = &(reg[strchr(registers, suba[0])-&registers[0]]);
               /* &(reg[regscount-1]); */
               prog->b[ocount] = &(reg[strchr(registers, suba[0])-&registers[0]]);
               /* &(reg[regscount-1]); */
            }                
            ocount++;
            bropen -= strlen(obj1);
         }
         a[bropen] = suba[0];
         memset(a+bropen+1,' ', j-bropen);
         if ((bropen==1) && (j==strlen(a)))
         {
            sprintf(wrong, "Internal error #2");
            FreeObjs(obj1, obj2, priori, suba);
            return(244);
         }
         bropen = -1; brcount = 0;
      }
   }
   memset(priori,0,strlen(a));
   for (i=0; i<strlen(a); i++)
   {
      if (strchr(prio3, a[i])!=NULL) priori[i] = 3;
      else if (strchr(prio2, a[i])!=NULL) priori[i] = 2;
      else if (strchr(prio1, a[i])!=NULL) priori[i] = 1;
   }
   sign = 0;
   for (j=1; j<4; j++)
   {
      for (i=0; i<strlen(a); i++)
      {
         if (priori[i]==(char)j)
         {
            sign=1; /* string contains calculation signs */
            GetObjectBefore(obj1, a, (char)i);
            GetObjectBehind(obj2, a, (char)i);
            if ((IsNumber(obj1)==1) || (IsVar(obj1, prog)==1))
            {
               /* create sequence "load register" */
               if ((int)i-(int)strlen(obj1)<0)
               {
                  sprintf(wrong, "Internal error #3"); FreeObjs(obj1, obj2, priori, suba);
                  return(1);
               }
               else a[i-strlen(obj1)] = registers[regscount];
               prog->func[ocount] = f_set;
               prog->a[ocount] = &(reg[regscount]);
               if (IsNumber(obj1)==1) 
               {
                  prog->z[ocount] = atof(obj1);
                  prog->b[ocount] = &(prog->z[ocount]);
               }
               else prog->b[ocount] = &(prog->varvalues[VarNr(obj1, prog)]);
               prog->a[ocount+1] = &(reg[regscount]);
               ocount++; regscount++;
               if (ocount>75) 
               {
                  sprintf(wrong, "Formula too complex."); FreeObjs(obj1, obj2, priori, suba);
                  return(1);
               }
            }
            else if ((IsRegister(obj1)==0) && (IsVar(obj1, prog)==0))
            {
               sprintf(wrong, "Unknown object A %s ", obj1);
               FreeObjs(obj1, obj2, priori, suba);
               return(1);
            }
            /* create operation */                   
            switch (a[i])
            {
               case '+': prog->func[ocount] = f_add; break;
               case '-': prog->func[ocount] = f_sub; break;
               case '*': prog->func[ocount] = f_mul; break;
               case '/': prog->func[ocount] = f_div; break;
               case '^': prog->func[ocount] = f_pow; break;
            }
            if (IsRegister(obj1)==1) 
               prog->a[ocount] = &(reg[strchr(registers, obj1[0])-&registers[0]]);
            if (IsRegister(obj2)==1)
               prog->b[ocount] = &(reg[strchr(registers, obj2[0])-&registers[0]]);
            else if (IsNumber(obj2)==1)
            {
               prog->z[ocount] = atof(obj2);
               prog->b[ocount] = &(prog->z[ocount]);
            }
            else if (IsVar(obj2, prog)==1) 
               prog->b[ocount] = &(prog->varvalues[VarNr(obj2, prog)]);
            else
            {
               sprintf(wrong, "Unknown object B %s", obj2);
               FreeObjs(obj1, obj2, priori, suba);
               return(1);
            }
            if (strlen(obj2)+strlen(obj1)>strlen(a))
            {
               sprintf(wrong, "Internal error #4"); FreeObjs(obj1, obj2, priori, suba);
               return(1);
            }    
            else memset(a+i-strlen(obj1)+1, ' ', strlen(obj2)+strlen(obj1));
            ocount++;
            if (ocount>75) 
            {
               sprintf(wrong, "Formula too complex."); FreeObjs(obj1, obj2, priori, suba);
               return(1);
            }
         }
      }
   }
   if (sign==0)
   {
      if (IsRegister(a)==1)
      {
         wrong[0] = 0;
         FreeObjs(obj1, obj2, priori, suba);
         return(0);
      }
      if (IsNumber(a)==1)
      {
         prog->func[ocount] = f_set;
         prog->a[ocount] = &(reg[regscount]);
         prog->b[ocount] = &(prog->z[ocount]);
         prog->z[ocount] = atof(a);
         memset(a, ' ', strlen(a));
         a[0] = registers[regscount];
         ocount++; regscount++;
      }
      else if (IsVar(a, prog)==1)
      {                           
         prog->func[ocount] = f_set;
         prog->a[ocount] = &(reg[regscount]);
         prog->b[ocount] = &(prog->varvalues[VarNr(a, prog)]);
         memset(a, ' ', strlen(a));
         a[0] = registers[regscount];
         ocount++; regscount++;
      }
      else
      {
         sprintf(wrong, "Unknown object C %s",a);
         FreeObjs(obj1, obj2, priori, suba);
         return(1);
      }
      if (ocount>75) 
      {
         sprintf(wrong, "Formula too complex."); FreeObjs(obj1, obj2, priori, suba);
         return(1);
      }
   }
   wrong[0] = 0;
   FreeObjs(obj1, obj2, priori, suba);
   return(0);
}

int Translate(char *wrong, char *aa, struct progtype *prog)
{
   unsigned int i, j; 
   int brcount = 0, error;
   char a[256];

   wrong[0] = 0;
   /* strcpy(a,aa);*/
   j = 0;
   for (i=0; i<strlen(aa) && aa[i]!='#'; i++) 
      if (aa[i]!=' ') 
      { 
         /*if (aa[i]!='E')*/ a[j] = tolower((int)*(aa+i));
         /*else a[j] = aa[i];*/
         j++;
      }
   a[j] = 0;
   for (i=0; i<strlen(a); i++)
   {
      if (a[i]=='(') brcount++;
      if (a[i]==')') brcount--;
      /* error with brackets */
      if (brcount<0)
      {
         sprintf(wrong, "closed before open bracket error");
         return(255);
      }
   }
   if (brcount!=0)
   {
      sprintf(wrong, "open brackets error");
      return(255);
   }
   regscount = 0; ocount = 0;
   error = DoTranslate(wrong, a, prog);
   if (wrong[0]==0) sprintf(wrong, "Parsing OK");
   prog->func[ocount] = NULL;
   return(error);
}
