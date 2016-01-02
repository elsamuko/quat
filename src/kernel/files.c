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

#include <stdio.h>          
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>  /* time, ctime */
#include "quat.h"  /* includes common.h */
#include "files.h"   

int isint(int *r, char *s);
int isdouble(double *d, char *s);
int GetNextObj(char *line, string *obj, int beg);
int GetKeywordIndex(char *keyword, struct keyword_struct kwords[]);
int ParseLine(char *s, string *keyword, string argument[], int *argcount);
int ReadNext(z_stream *d, unsigned char *s);
int ReadNextDouble(z_stream *s, double *d, char *Error);
int ReadNextLong(z_stream *s, long *i, char *Error);
int ReadNextInt(z_stream *s, int *i, char *Error);

extern long calc_time;
extern char slash;
        
int isint(int *r, char *s)
{             
   char **endptr;
   char *p;
   long int l;
            
   if (s[0]==0) return(0);
   endptr = &p;
   l = strtol(s, endptr, 10);
   if (**endptr!=0) return(0);
   *r = (int)l;
   return(-1);
}             

int isdouble(double *d, char *s)
{                        
   char **endptr;
   char *p;
    
   if (s[0]==0) return(0);
   endptr = &p;
   *d = strtod(s, endptr);
   if (**endptr!=0) return(0);
   return(-1); 
}

int GetNextObj(char *line, string *obj, int beg)
{         
   unsigned int i; int objbeg, objend;
   
   for (i=beg; isspace((int)line[i]) && i<strlen(line); i++);     
   if (i==strlen(line)) return(-1); /* No object follows */
   objbeg = i;
   for (i=objbeg; !isspace((int)line[i]) && i<strlen(line); i++);      
   objend = i;
   strncpy(*obj, line+objbeg, (size_t)(objend-objbeg));
   (*obj)[objend-objbeg] = 0;
        
   return(objend);
}

int GetKeywordIndex(char *keyword, struct keyword_struct kwords[])
{
   int i;

   for (i=0; kwords[i].name[0]!=0 && strcmp(kwords[i].name, keyword)!=0; i++);     
   if (kwords[i].name[0]==0) return(-1);
   
   return(i);
}     

int ParseLine(char *s, string *keyword, string argument[], int *argcount)
{
   int i;
   char *s2;
   
   if (strlen(s)>0 && s[strlen(s)-1]=='\n') s[strlen(s)-1] = 0;   
   (*keyword)[0] = 0; 
   for (i=0; i<8; i++) argument[i][0] = 0;
   for (i=0; i<(int)strlen(s); i++) s[i] = tolower((int)s[i]);
   if (strchr(s, '#')!=0) { s2 = strchr(s, '#'); *s2 = 0; }
   *argcount = 0;
   i = GetNextObj(s, keyword, 0);
   if (i==-1) return(-1);
   while ( i!=-1 && *argcount<8 )      
   {
      i = GetNextObj(s, &argument[*argcount], i);         
      if (i!=-1) (*argcount)++;
   }   
   if (i!=-1) return(-2);  /* Too many arguments */
   return(0);
}

int ParseFile(const char *nam, const char *_altpath, struct keyword_struct kwords[], char *ColBuf, int ColBufLen, int *ColBufCount,
   double *CutBuf, int CutBufLen, int *CutBufCount, char *rc, char *Error_Msg)
{
   FILE *ini;
   char fname[256], name[256], altpath[256];
   string s, keyword, argument[8], includeb, ErrorMSG;
   int argtype[8];
   int argint[8];
   double argdouble[8];
   int line, i, j, n, argcount, kwidx;
   char nstr[3];
   int ferror_val = 0;

   strcpy(name, nam); ConvertPath(name);
   strcpy(altpath, _altpath); ConvertPath(altpath);

   ini = fopen(name, "r");
   if (ini==NULL)
   {
      strcpy(fname, altpath); strcat(fname, name);
      ini = fopen(fname, "r");
      if (ini==NULL)
      {
         sprintf(Error_Msg, "Cannot open file '%s'.", name);
         return(-1);
      }
   }
   line = 0;
   while (!feof(ini) && !(ferror_val = ferror(ini)))
   {
      do { s[0]=0; fgets(s, 254, ini); line++; } while (s[0]=='#');
      if (strlen(s)>250)
      {
         sprintf(Error_Msg, "Line %i: Line too long (max. 250 characters).", line);
         fclose(ini);
         return(-1); 
      }
      kwords[18].param[0].mem = &includeb[0];   /* Set memory for include keyword */
      kwords[18].param[0].max = 250;
      i = ParseLine(s, &keyword, argument, &argcount);
      if (i==-2)
      {
         sprintf(Error_Msg, "Line %i: Too many arguments.",line);
         fclose(ini);
         return(-1);
      }
      /* Blank line ? */
      if (i!=-1)
      {
         /* check keyword */
         if ((kwidx = GetKeywordIndex(keyword, kwords))==-1)
         {
            sprintf(Error_Msg, "Line %i: Unknown keyword '%s'.", line, keyword);
            fclose(ini);
            return(-1);
         }
         /* check arguments */
         if (argcount!=kwords[kwidx].paramcount)
         {
            sprintf(Error_Msg, "Line %i: Invalid number of arguments, found %i instead of %i.",line,argcount,kwords[kwidx].paramcount);
            fclose(ini);
            return(-1);
         }
         for (i=0; i<argcount; i++)
         {
            argtype[i] = 0;
            if (isint(&argint[i], &argument[i][0])==-1) argtype[i] +=1; 
            if (isdouble(&argdouble[i], &argument[i][0])==-1) argtype[i] +=2; 
            /* check type and range of argument */
            if ( kwords[kwidx].param[i].type==T_INTEGER && (argtype[i]&1)==0 )  
            {
               sprintf(Error_Msg, "Line %i, argument %i: Invalid type, should be integer.",line,i+1);
               fclose(ini);
               return(-1);
            }  
            if ( kwords[kwidx].param[i].type==T_DOUBLE && (argtype[i]&2)==0 )    
            {
               sprintf(Error_Msg, "Line %i, argument %i: Invalid type, should be double.",line,i+1);
               fclose(ini);
               return(-1);
            }
            if (kwords[kwidx].param[i].type==T_INTEGER)
            {
               if (((double)argint[i]<kwords[kwidx].param[i].min)||((double)argint[i]>kwords[kwidx].param[i].max))
               {
                 sprintf(Error_Msg, "Line %i, argument %i: Invalid range, should be between %i and %i.",
                    line,i+1,(int)kwords[kwidx].param[i].min,(int)kwords[kwidx].param[i].max);
                 fclose(ini);
                 return(-1);
               }
               if (kwords[kwidx].which_buf == NO_BUF)
               {
                  int *tmp;
                  tmp = (int *)kwords[kwidx].param[i].mem;
                  *tmp = argint[i];
               }
            }
            if (kwords[kwidx].param[i].type==T_DOUBLE)
            {
               if ( argdouble[i]<kwords[kwidx].param[i].min || argdouble[i]>kwords[kwidx].param[i].max )
               {
                  sprintf(Error_Msg, "Line %i, argument %i: Invalid range, should be between %.0f and %.0f.",
                     line,i+1,kwords[kwidx].param[i].min,kwords[kwidx].param[i].max);
                  fclose(ini);
                  return(-1);
               }
               if (kwords[kwidx].which_buf == NO_BUF)
               {
                  double *tmp;
                  tmp = (double *)kwords[kwidx].param[i].mem;
                  *tmp = argdouble[i];
               }
            }
            if (kwords[kwidx].param[i].type==T_STRING)
            {
               if (strlen(argument[i])>kwords[kwidx].param[i].max)
               {
                  sprintf(Error_Msg, "Line %i, argument %i: String too long, should be less than %i characters.", 
                     line,i+1,(int)kwords[kwidx].param[i].max);
                  fclose(ini);
                  return(-1);
               }
               if (kwords[kwidx].which_buf == NO_BUF) strncpy(kwords[kwidx].param[i].mem,
                  &argument[i][0], (size_t)kwords[kwidx].param[i].max);   
            }
         }
         /* Parse format string */
         /* no check done whether format_string correct !!! */
         /* %n   ... insert argument n into buf */             
         /* %bnn ... insert following nn bytes into buf, whatever they may be (even \0) */
         i = 0;
         if (kwords[kwidx].which_buf != NO_BUF) while (kwords[kwidx].format_string[i]!=0)
         {
            if (kwords[kwidx].format_string[i]=='%')
            {
               if (kwords[kwidx].format_string[i+1]=='b')   /* it's a %bnn */
               {
                  i++;
                  nstr[0] = kwords[kwidx].format_string[i+1];
                  nstr[1] = kwords[kwidx].format_string[i+2];
                  nstr[2] = 0;
                  n = 0;
                  isint(&n, nstr);
                  if (kwords[kwidx].which_buf == CUT_BUF)
                  {
                     if (*CutBufCount>=CutBufLen) 
                     {
                        sprintf(Error_Msg, "Too many intersection objects.");
                        fclose(ini);
                        return(-1);
                     }
                     memcpy(&CutBuf[*CutBufCount], &kwords[kwidx].format_string[i+3], (size_t)n);
                     (*CutBufCount)++;
                  }
                  i += (n+2);  
               }
               else /* must be a %n */
               {   
                  nstr[0] = kwords[kwidx].format_string[i+1];
                  nstr[1] = 0;
                  n = 0;
                  isint(&n, nstr);   
                  n--;  /* internal arg 1 = 0 */
                  if (kwords[kwidx].param[n].type==T_INTEGER)
                  {                                  
                     if (kwords[kwidx].which_buf == COL_BUF)
                     {
                        if (*ColBufCount+(int)sizeof(int)>=ColBufLen)  /* Buffer overflow */
                        { 
                           sprintf(Error_Msg, "Too many colors (>30).");
                           fclose(ini);
                           return(-1);
                        }
                        memcpy(&ColBuf[*ColBufCount], &argint[n], sizeof(int));  
                        *ColBufCount += sizeof(int);
                     }
                     if (kwords[kwidx].which_buf == CUT_BUF)
                     {
                        if (*CutBufCount>=CutBufLen) /* Buffer overflow */
                        {
                           sprintf(Error_Msg, "Too many intersection objects.");
                           fclose(ini);
                           return(-1);
                        }
                        CutBuf[*CutBufCount] = (double)argint[n];
                        (*CutBufCount)++;
                     }
                  }                          
                  if (kwords[kwidx].param[n].type==T_DOUBLE)
                  {                                                                
                     if (kwords[kwidx].which_buf == COL_BUF)
                     {
                        if (*ColBufCount+(int)sizeof(double)>=ColBufLen)  /* Buffer overflow */
                        {           
                           sprintf(Error_Msg, "Too many colors (>30).");
                           fclose(ini);
                           return(-1);
                        }
                        memcpy(&ColBuf[*ColBufCount], &argdouble[n], sizeof(double));   
                        *ColBufCount += sizeof(double);
                     }
                     if (kwords[kwidx].which_buf == CUT_BUF)
                     {
                        if (*CutBufCount>=CutBufLen) /* Buffer overflow */
                        {           
                           sprintf(Error_Msg, "Too many intersection objects.");
                           fclose(ini);
                           return(-1);
                        }
                        CutBuf[*CutBufCount] = argdouble[n];
                        (*CutBufCount)++;
                     }
                  }
                  i++;
               }
            }
            i++;
         }
         
         if (kwidx == 18)  /* "include" keyword */
         {
            ConvertPath(includeb);
            if (*rc==5)
            {         
               sprintf(Error_Msg, "Recursion of files included to deep.");
               fclose(ini);
               return(-1);
            }
            else (*rc)++;
            /* Do a recursion */
            if ((j = ParseFile(includeb, altpath, kwords, ColBuf, ColBufLen, ColBufCount,
               CutBuf, CutBufLen, CutBufCount, rc, ErrorMSG)) != 0)
            {                  
               fclose(ini);
               if (strlen(includeb)+strlen(ErrorMSG)+18<180)
               {
                  sprintf(Error_Msg, "included file '%s': %s", includeb, ErrorMSG);
                  return(-1);
               }
               else
               {
                  sprintf(Error_Msg, "[...] %s", ErrorMSG);
                  return(-2);  /* -2: Don´t add anything to Error_Msg (avoid overflow) */
               }
            }
         }
      }
   }
   fclose(ini);
   if (ferror_val != 0) {
   	sprintf(Error_Msg, "Error while reading file '%s'.", name);
	return -1;
   }
   return(0);
}

int PNGInitialization(name, mode, png, png_info, png_internal, xstart, ystart, calctime, f,
   v, pal, realpal, colorscheme, cutbuf, zflag)
const char *name;
int mode;
FILE **png;
struct png_info_struct *png_info;
struct png_internal_struct *png_internal;
int xstart, ystart;
long calctime;
struct frac_struct *f;
struct view_struct *v;
struct disppal_struct *pal;
struct realpal_struct *realpal;
char *colorscheme;
double *cutbuf;
int zflag;
/* rewrites file "name" */
{
   int i, j, pos, cutpos;
   unsigned char pal2[256][3];
   char pngname[256], *s;
   unsigned long l;  
   z_stream c_stream;
   char *Buf, string[10240];
   unsigned char *uBuf;
   time_t dtime;
   unsigned char LongBuf[4];
   struct frac_struct frac;
   struct view_struct view;
                                    
   frac = *f; view = *v;
   FormatExternToIntern(&frac, &view);

   strcpy(pngname, name);
   ConvertPath(pngname);
   *png = fopen(pngname, "w+b");
   if (!*png) return(-1);
   
   png_info->width = view.xres;
   if (zflag==1) png_info->width *= view.antialiasing;
   if (IsStereo(&view)) png_info->width *= 2;
   png_info->height = view.yres;                      
   if (zflag==1) png_info->height *= view.antialiasing;
   if (mode==0)   /* indexed color */
   { 
      png_info->bit_depth = 8; 
      png_info->color_type = 3;
   }
   if (mode==1)   /* true color */
   {
      png_info->bit_depth = 8;
      png_info->color_type = 2;
   }
   png_info->compression = 0;
   png_info->interlace = 0;
   png_info->filter = 0;
   
   if (InitWritePNG(*png, png_info, png_internal)) return(-1);
   
   /* Write a gAMA chunk */
   strncpy(png_internal->chunk_type, "gAMA", 4);
   png_internal->length = 4;
   l = (long)(0.45455*100000);
   CorrectLong1(&l, LongBuf);
   if (WriteChunk(png_internal, LongBuf)) return(-1);
   
   /* Write a PLTE chunk */
   if (mode==0)
   {
      for (i=0; i<pal->maxcol; i++) 
      {
         pal2[i][0] = pal->cols[i].r << 2;
         pal2[i][1] = pal->cols[i].g << 2;
         pal2[i][2] = pal->cols[i].b << 2;
      }
      strncpy(png_internal->chunk_type, "PLTE", 4);
      png_internal->length = pal->maxcol*3;
      if (WriteChunk(png_internal, pal2[0])) return(-1);
   }                  
            
   if ((Buf=malloc(10240))==NULL) return(-1);
   if ((uBuf=malloc(10240))==NULL) {
      free(Buf);
      return(-1);
   }
   strncpy(png_internal->chunk_type, "tEXt", 4);
   strcpy(Buf, "Title");
   strcpy(&Buf[6], "Quaternion fractal image");   
   png_internal->length = 30;
   if (WriteChunk(png_internal, (unsigned char*)Buf)) {
      free(uBuf); free(Buf); return(-1);
   }
   
   strncpy(png_internal->chunk_type, "tEXt", 4);
   strcpy(Buf, "Software");
   sprintf(&Buf[9], "%s %s", PROGNAME, PROGVERSION);   
   png_internal->length = strlen(Buf)+strlen(&Buf[9])+1; 
   if (WriteChunk(png_internal, (unsigned char*)Buf)) {
      free(uBuf); free(Buf); return(-1);
   }
   
   strncpy(png_internal->chunk_type, "tEXt", 4);
   strcpy(Buf, "Creation Time");
   time(&dtime);
   s = ctime(&dtime);
   s[strlen(s)-1] = 0;
   sprintf(&Buf[14], "%s", s);    
   png_internal->length = 14+strlen(&Buf[14]); 
   if (WriteChunk(png_internal, (unsigned char*)Buf)) {
      free(uBuf); free(Buf); return(-1);
   }
   
   /* Write quAt chunk */
   strncpy(png_internal->chunk_type, "quAt", 4);
   sprintf(Buf, "%s %s", PROGNAME, PROGVERSION);
   memcpy(uBuf, Buf, 10240);
   uBuf[strlen(Buf)+1] = (unsigned char)(xstart>>8 & 0xffL);      /* MSB of actx */    
   uBuf[strlen(Buf)+2] = (unsigned char)(xstart & 0xffL);         /* LSB of actx */
   uBuf[strlen(Buf)+3] = (unsigned char)(ystart>>8 & 0xffL);      /* MSB of acty */
   uBuf[strlen(Buf)+4] = (unsigned char)(ystart & 0xffL);         /* LSB of acty */ 
   uBuf[strlen(Buf)+5] = (unsigned char)(calctime>>24 & 0xffL); 
   uBuf[strlen(Buf)+6] = (unsigned char)(calctime>>16 & 0xffL);
   uBuf[strlen(Buf)+7] = (unsigned char)(calctime>>8 & 0xffL);
   uBuf[strlen(Buf)+8] = (unsigned char)(calctime &0xffL);
   i = strlen(Buf)+8;
   
   /* convert frac_struct to ascii */      
   pos = sprintf(string, "%+20.20eN%+20.20eN%+20.20eN%+20.20eN", 
      frac.c[0], frac.c[1], frac.c[2], frac.c[3]);
   pos += sprintf(&string[pos], "%+20.20eN%iN%+20.20eN%iN",
      frac.bailout, frac.maxiter, frac.lvalue, frac.formula);
	for (j=0; j<4; ++j)
	   pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN%+20.20eN",
	   	frac.p[j][0], frac.p[j][1], frac.p[j][2], frac.p[j][3]);
   /* convert view_struct to ascii */ 
   pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN",
      view.s[0], view.s[1], view.s[2]);
   pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN",
      view.up[0], view.up[1], view.up[2]);
   pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN",
      view.light[0], view.light[1], view.light[2]);
   pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN",
      view.Mov[0], view.Mov[1], view.LXR);
   pos += sprintf(&string[pos], "%iN%iN%iN",
      view.xres, view.yres, view.zres);
   pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN%iN",
      view.phongmax, view.phongsharp, view.ambient, view.antialiasing);
   pos += sprintf(&string[pos], "%+20.20eN", view.interocular);
   c_stream.zalloc = (alloc_func)0;
   c_stream.zfree = (free_func)0;
   deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
   c_stream.next_out = &uBuf[i+1];
   c_stream.avail_out = 10240-i-1;
   c_stream.next_in = (unsigned char*)string;
   c_stream.avail_in = pos;
   deflate(&c_stream, Z_NO_FLUSH);
   /* Convert realpal_struct to ascii */
   pos = sprintf(string, "%iN", realpal->colnum);
   c_stream.next_in = (unsigned char*)string;
   c_stream.avail_in = pos;
   deflate(&c_stream, Z_NO_FLUSH);
   for (j=0; j<realpal->colnum; j++)
   {                                   
      pos = sprintf(string, "%+20.20eN", realpal->cols[j].weight);
      pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN",
         realpal->cols[j].col1[0], realpal->cols[j].col1[1], realpal->cols[j].col1[2]);
      pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN",
         realpal->cols[j].col2[0], realpal->cols[j].col2[1], realpal->cols[j].col2[2]);
      c_stream.next_in = (unsigned char*)string;
      c_stream.avail_in = pos;
      deflate(&c_stream, Z_NO_FLUSH);
   }
   c_stream.next_in = (unsigned char *)colorscheme;
   c_stream.avail_in = 250;
   deflate(&c_stream, Z_NO_FLUSH);
   /* Write cutbuf to chunk */
   cutpos = 0; 
   while (*(char *)&cutbuf[cutpos]!=CUT_TERMINATOR)
   {
      switch (*(char *)&cutbuf[cutpos])
      {
         case CUT_PLANE:
            pos = sprintf(string, "%iN", CUT_PLANE);
            pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN",
               cutbuf[cutpos+1], cutbuf[cutpos+2], cutbuf[cutpos+3]);
            pos += sprintf(&string[pos], "%+20.20eN%+20.20eN%+20.20eN",
               cutbuf[cutpos+4], cutbuf[cutpos+5], cutbuf[cutpos+6]);
            cutpos += 7;
            c_stream.next_in = (unsigned char*)string;
            c_stream.avail_in = pos;
            deflate(&c_stream, Z_NO_FLUSH);
            break;
      }
   }
   string[0] = CUT_TERMINATOR;
   c_stream.next_in = (unsigned char*)string;
   c_stream.avail_in = 1;
   deflate(&c_stream, Z_FINISH);
   png_internal->length = c_stream.total_out+i;
   deflateEnd(&c_stream);
   if (WriteChunk(png_internal, uBuf)) {
      free(uBuf); free(Buf); return(-1);
   }

   free(uBuf);
   free(Buf);
   return(0);
}                       

int ReadNext(z_stream *d, unsigned char *s)
{                         
   int i=0, err;
   
   do
   {
      d->avail_out = 1;
      d->next_out = &s[i];     
      err = inflate(d, Z_PARTIAL_FLUSH);
   } while ( err==0 && s[i++]!='N' ); 
   return(err);
}           

int ReadNextDouble(z_stream *s, double *d, char *Error)
{
   int err;
   char string[100];           
   
   err = ReadNext(s, (unsigned char*)string);
   if (err==Z_DATA_ERROR || err==Z_STREAM_ERROR || err==Z_MEM_ERROR)
   {
      sprintf(Error, "Corrupted PNG file or no memory.\n");
      return(-1);
   }
   *d = atof(string);
   
   return(0);
}

int ReadNextInt(z_stream *s, int *i, char *Error)
{            
   int err;
   char string[100];
   
   err = ReadNext(s, (unsigned char*)string);
   if (err==Z_DATA_ERROR || err==Z_STREAM_ERROR || err==Z_MEM_ERROR)
   {
      sprintf(Error, "Corrupted PNG file or no memory.\n");
      return(-1);
   }
   *i = (int)atoi(string);
   
   return(0);
}

int ReadNextLong(z_stream *s, long *i, char *Error)
{            
   int err;
   char string[100];
   
   err = ReadNext(s, (unsigned char*)string);
   if (err==Z_DATA_ERROR || err==Z_STREAM_ERROR || err==Z_MEM_ERROR)
   {
      sprintf(Error, "Corrupted PNG file or no memory.\n");
      return(-1);
   }
   *i = (long)atol(string);
   
   return(0);
}

int ReadParameters(char *Error, int *xstart, int *ystart, struct png_internal_struct *internal,
    struct frac_struct *f, struct view_struct *v, struct realpal_struct *realpal, 
    char *colorscheme, double *cutbuf)
/* RC :  -3 No quat chunk; -4 No memory; -5 No Quat-PNG; -128 file ver higher than progver */ 
{
   int i, j, cutpos, cuttype;
   unsigned char *Buf;
   char s[40];
   z_stream d;
   int err;
   float ver, thisver;            
   struct frac_struct frac;
   struct view_struct view;
      
   frac = *f; view = *v;   /* Init to defaults. Important if filever<thisver */
   /* search until quAt chunk found */
   while ( strncmp(internal->chunk_type, "quAt", 4)!=0
      && strncmp(internal->chunk_type, "IEND", 4)!=0) GetNextChunk(internal);
   /* No quAt chunk? */
   if (strncmp(internal->chunk_type, "IEND", 4)==0) 
   {
      sprintf(Error, "PNG file has no QUAT chunk.\n");
      return(-3);
   }
   sprintf(s, "%s %s", PROGNAME, PROGVERSION);
   Buf = malloc((size_t)internal->length);
   /* No memory? Bad! */
   if (Buf==NULL) 
   {
      sprintf(Error, "No memory.\n");
      return(-4);
   }
   ReadChunkData(internal, Buf);
   /* Check for QUAT signature */
   if (strncmp((char *)Buf, s, 4)!=0)
   {
      free(Buf);
      sprintf(Error, "No QUAT signature in QUAT chunk.\n");
      return(-5);
   }
   else sprintf(Error, "%c%c%c%c",Buf[5],Buf[6],Buf[7],Buf[8]); 
   /* Determine version of QUAT which wrote file */
   ver = (float)0; thisver = (float)0;
   thisver = (float)atof(PROGVERSION);
   ver = (float)atof((char*)&Buf[5]);
   if (ver==0)
   {
      free(Buf); 
      sprintf(Error, "No QUAT signature in QUAT chunk.\n");
      return(-5);
   }
   *xstart = ((Buf[strlen(s)+1] << 8) & 0xff00) | Buf[strlen(s)+2];  
   *ystart = ((Buf[strlen(s)+3] << 8) & 0xff00) | Buf[strlen(s)+4];  
   d.zalloc = (alloc_func)0;
   d.zfree = (free_func)0;
   err = inflateInit(&d);
   if (ver>=0.92)
   {
      calc_time = ((Buf[strlen(s)+5] << 24) & 0xff000000L) | ((Buf[strlen(s)+6] << 16) & 0xff0000L)
         | ((Buf[strlen(s)+7] << 8) & 0xff00L) | Buf[strlen(s)+8];
      d.next_in = &(Buf[strlen(s)+9]);
      d.avail_in = (int)internal->length-strlen(s) - 9;
   }
   else
   {
      d.next_in = &(Buf[strlen(s)+5]);
      d.avail_in = (int)internal->length-strlen(s) - 5;
   }
   
 
   for (i=0; i<4; i++) if (ReadNextDouble(&d, &frac.c[i], Error)) return(-6); 
   if (ReadNextDouble(&d, &frac.bailout, Error)) return(-6);
   if (ReadNextInt(&d, &frac.maxiter, Error)) return(-6);
   if (ReadNextDouble(&d, &frac.lvalue, Error)) return(-6);
   if (ReadNextInt(&d, &frac.formula, Error)) return(-6); 
   
	if (ver>=1.1) {
		for (j=0; j<4; ++j)
		  for (i=0; i<4; ++i) 
		    if (ReadNextDouble(&d, &frac.p[j][i], Error)) return(-6);
	}

   for (i=0; i<3; i++) if (ReadNextDouble(&d, &view.s[i], Error)) return(-6); 
   for (i=0; i<3; i++) if (ReadNextDouble(&d, &view.up[i], Error)) return(-6);  
   for (i=0; i<3; i++) if (ReadNextDouble(&d, &view.light[i], Error)) return(-6); 
   if (ReadNextDouble(&d, &view.Mov[0], Error)) return(-6); 
   if (ReadNextDouble(&d, &view.Mov[1], Error)) return(-6); 
   if (ReadNextDouble(&d, &view.LXR, Error)) return(-6); 
   if (ReadNextInt(&d, &view.xres, Error)) return(-6);
   if (ReadNextInt(&d, &view.yres, Error)) return(-6);
   if (ReadNextInt(&d, &view.zres, Error)) return(-6);
   if (ReadNextDouble(&d, &view.phongmax, Error)) return(-6); 
   if (ReadNextDouble(&d, &view.phongsharp, Error)) return(-6); 
   if (ReadNextDouble(&d, &view.ambient, Error)) return(-6); 
   if (ver>=0.91) if (ReadNextInt(&d, &view.antialiasing, Error)) return(-6); 
   if (ver>=0.92) if (ReadNextDouble(&d, &view.interocular, Error)) return(-6);
   if (ReadNextInt(&d, &realpal->colnum, Error)) return(-6);
   for (i=0; i<realpal->colnum; i++)
   {
      if (ReadNextDouble(&d, &realpal->cols[i].weight, Error)) return(-6);
      for (j=0; j<3; j++) if (ReadNextDouble(&d, &realpal->cols[i].col1[j], Error)) return(-6);
      for (j=0; j<3; j++) if (ReadNextDouble(&d, &realpal->cols[i].col2[j], Error)) return(-6);
   }
   
   d.next_out = (unsigned char*)colorscheme;
   d.avail_out = 250;
   err = inflate(&d, Z_PARTIAL_FLUSH);
   if (err==Z_DATA_ERROR || err==Z_STREAM_ERROR || err==Z_MEM_ERROR)
   {
      sprintf(Error, "Corrupted PNG file or no memory.");
      return(-6);
   }
   cutpos = 0;
   do
   {
      if (ReadNextInt(&d, &cuttype, Error)) return(-6);
      *(char *)&cutbuf[cutpos++] = (char)cuttype;
      switch (cuttype)
      {
         case CUT_PLANE:
            for (i=0; i<6; i++) if (ReadNextDouble(&d, &cutbuf[cutpos+i], Error)) return(-6);
            cutpos += 6;
            break;
      }
   } while (cuttype != CUT_TERMINATOR);
   err = inflateEnd(&d);

   free(Buf);
              
   *f = frac; *v = view;
   if (FormatInternToExtern(f,v)!=0) 
   {
      sprintf(Error, "Strange: Error in view struct!");
      return(-1);
   }
   if (thisver>=ver) return(0);
   return(-128);
}

int UpdateQUATChunk(struct png_internal_struct *internal, int actx, int acty)
{     
   char s[41];
   unsigned char *Buf;
   long QuatPos;

   /* Back to beginning */
   fflush(internal->png);
   PosOverIHDR(internal);

   /* search until quAt chunk found */
   while ( strncmp(internal->chunk_type, "quAt", 4)!=0 
      && strncmp(internal->chunk_type, "IEND", 4)!=0 ) GetNextChunk(internal);
   /* No quAt chunk? */
   if (strncmp(internal->chunk_type, "IEND", 4)==0) return(-3);
   sprintf(s, "%s %s", PROGNAME, PROGVERSION);
   QuatPos = ftell(internal->png)-8;
   Buf = malloc((size_t)internal->length);
   /* No memory? Bad! */
   if (!Buf) return(-4);
   ReadChunkData(internal, Buf);
   if (strncmp((char*)Buf, s, strlen(s))!=0)
   {
      free(Buf);
      return(-5);
   }    
   Buf[strlen(s)+1] = (unsigned char)(actx>>8 & 0xffL);      /* MSB of actx */    
   Buf[strlen(s)+2] = (unsigned char)(actx & 0xffL);         /* LSB of actx */
   Buf[strlen(s)+3] = (unsigned char)(acty>>8 & 0xffL);      /* MSB of acty */
   Buf[strlen(s)+4] = (unsigned char)(acty & 0xffL);         /* LSB of acty */ 
   Buf[strlen(s)+5] = (unsigned char)(calc_time>>24 & 0xffL); 
   Buf[strlen(s)+6] = (unsigned char)(calc_time>>16 & 0xffL);
   Buf[strlen(s)+7] = (unsigned char)(calc_time>>8 & 0xffL);
   Buf[strlen(s)+8] = (unsigned char)(calc_time &0xffL);
   fflush(internal->png);
   fseek(internal->png, QuatPos, SEEK_SET);
   if (WriteChunk(internal, Buf)) return(-1);
   fflush(internal->png);
   free(Buf);
   
   return(0); 
}

int PNGEnd(struct png_internal_struct *png_internal, unsigned char *buf, int actx, int acty)
{            
   unsigned char dummy; 
   int i;
   
   for (i=acty; i<(int)png_internal->height; i++) WritePNGLine(png_internal, buf); 

   EndIDAT(png_internal);                   
   png_internal->length = 0;
   strncpy(png_internal->chunk_type, "IEND", 4);
   if (WriteChunk(png_internal, &dummy)) return(-1);
   UpdateQUATChunk(png_internal, actx, acty);
   
   return(0);   
}

int GetNextName(char *nowname, char *namewop)
{
   char onlyname[256], *onlyextp, *pathendp, onlyext[256], name[260];
   int n = 0;
   size_t pathlen; 
   FILE *f;
   
   ConvertPath(nowname);
   /* Delete everything after first point */
   pathendp = strrchr(nowname, slash);
   if (pathendp!=NULL) pathlen = (int)(pathendp - nowname)+1;
   else 
   {
      pathlen = 0;
      pathendp = nowname - 1;
   }
   strncpy(onlyname, pathendp+1, 256);
   if ((f=fopen(nowname, "r"))==NULL)
   {
      if (namewop!=NULL) strncpy(namewop, onlyname, 80);
      return(0);
   }      
   fclose(f);
   onlyextp = strrchr(onlyname, '.');
   strcpy(onlyext, ".png");
   if (onlyextp!=NULL) 
   {
      strcpy(onlyext, onlyextp);
      *onlyextp = 0;
   }  
   else onlyextp = onlyext;
   while (strlen(onlyname)<8) strcat(onlyname, "_"); 
   onlyname[strlen(onlyname)-2] = 0;
   
   strncpy(name, nowname, pathlen); name[pathlen] = 0;     /* copy path to name */
   sprintf(name, "%s%s%02i%s", name, onlyname, n, onlyext);/* append name, number, ext to name */
   for (n=1; n<=99 && (f=fopen(name, "r"))!=NULL; n++)
   {                                       
      fclose(f);
      strncpy(name, nowname, pathlen);  name[pathlen] = 0;
      sprintf(name, "%s%s%02i%s", name, onlyname, n, onlyext);
   }
                     
   /* No free name found */      
   if (n>99) return(-1);  
   if (strlen(name)>256) return(-1);

   strncpy(nowname, name, 256);
   if (namewop!=NULL) sprintf(namewop, "%s%02i%s", onlyname, n-1, onlyext);
   return(0);
}
              
