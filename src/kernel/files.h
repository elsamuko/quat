#define T_INTEGER 0
#define T_DOUBLE 1
#define T_STRING 2
#define STDDOUBLE(z,i) { z[i].type = T_DOUBLE; z[i].min = -1000000; z[i].max = 1000000; }
#define STDINT(z,i) { z[i].type = T_INTEGER; z[i].min = 0; z[i].max = 65535; }
#define VVPTR (void *)&

#include "png.h"

#define NO_BUF 0
#define COL_BUF 1
#define CUT_BUF 2

typedef char string[256];

struct parameter_struct
{                  
   int type;   /* yet only T_INTEGER, T_DOUBLE or T_STRING */
   double min;
   double max;
   void *mem;             /* used for "fixed" parameters that cannot repeat (e.g."lvalue") */
};

struct keyword_struct
{                
   const char *name;
   int paramcount;
   struct parameter_struct *param;    /* Array of all 'paramcount' parameters */
   const char *format_string;        /* used for repeatable parameters (e.g."color") */
   char which_buf;             /* (s.above) describes to which buffer the data is written */  
                               /* can be NO_BUF, COL_BUF or CUT_BUF */
};

#ifdef __cplusplus
extern "C" {
#endif
int ParseFile(const char *name, const char *altpath, struct keyword_struct kwords[], char *ColBuf, int ColBufLen, int *ColBufCount,
   double *CutBuf, int CutBufLen, int *CutBufCount, char *rc, char *Error_Msg);
   /* name: name of INI-file */          
   /* altpath: alternative path to search for files */
   /* kwords: array of keyword_struct, which describes keywords and their parameters */
   /* Col|CutBuf: Buffers which data from files is written to according to the */
   /*             field "format_string" in keyword_struct */
   /* Col|CutBufLen: maximal number of bytes in buffer */
   /* *Col|*CutBufCount: number of bytes written to buffer. Has to be set to zero on call */
   /* rc: recursion counter. Has to be zero when called. */
   /* Error_MSG: string, Error-message is returned here */
   /* returns: */
   /* 0 , no errors */
   /* -1 , if parsing error (Error_Msg says which) */

int PNGInitialization(const char *name, int mode, FILE **png, struct png_info_struct *png_info, struct png_internal_struct *png_internal, 
   int xstart, int ystart, long calctime, struct frac_struct *f, struct view_struct *v, struct disppal_struct *pal, struct realpal_struct *realpal,  
   char *colorscheme, double *cutbuf, int zflag);
/* Wants external format, writes internal to PNG */
   
int ReadParameters(char *Error, int *xstart, int *ystart, struct png_internal_struct *internal,
    struct frac_struct *f, struct view_struct *v, struct realpal_struct *realpal, 
    char *colorscheme, double *cutbuf);           
/* returns external format, reads internal from PNG */

int UpdateQUATChunk(struct png_internal_struct *internal, int actx, int acty);

int PNGEnd(struct png_internal_struct *png_internal, unsigned char *buf, int actx, int acty);

int GetNextName(char *nowname, char *namewop);
/* returns next name in numbering, namewop ... name without path */
#ifdef __cplusplus
}
#endif

