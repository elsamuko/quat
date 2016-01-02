#include "common.h"
/*#include "png.h"*/

typedef int (*VER_ReturnVideoInfo) (struct vidinfo_struct *vidinfo);
typedef int (*VER_SetColors) (struct disppal_struct *disppal);
typedef int (*VER_Initialize) (int x, int y, char *Error);
typedef int (*VER_Done)(void);
typedef int (*VER_update_bitmap) (long x1, long x2, long xres, int y, unsigned char *Buf, int which);
typedef int (*VER_getline) (unsigned char *line, int y, long xres, int whichbuf);
typedef int (*VER_putline) (long x1, long x2, long xres, int y, unsigned char *Buf, int whichbuf);
typedef int (*VER_check_event)(void);
typedef int (*VER_Change_Name) (char *s);
typedef void (*VER_eol) (int line);
typedef void (*VER_Debug) (char *s);
typedef void (*VER_update_parameters) (struct frac_struct *f, struct view_struct *v,
   struct realpal_struct *r, char *colscheme, double *cut);

#ifdef __cplusplus
extern "C" {
#endif 

/* DUMMY function prototypes */
int DUMMY_ReturnVideoInfo(struct vidinfo_struct *vidinfo);
int DUMMY_SetColors(struct disppal_struct *disppal);
int DUMMY_Initialize(int x, int y, char *Error);
int DUMMY_Done();
int DUMMY_update_bitmap(long x1, long x2, long xres, int y, unsigned char *Buf, int which);
int DUMMY_getline(unsigned char *line, int y, long xres, int whichbuf);
int DUMMY_putline(long x1, long x2, long xres, int y, unsigned char *Buf, int whichbuf);
int DUMMY_check_event(void);
int DUMMY_Change_Name(char *s);
void DUMMY_Debug(char *s);
void DUMMY_eol(int line);
void DUMMY_update_parameters(struct frac_struct *f, struct view_struct *v,
   struct realpal_struct *r, char *colscheme, double *cut);

void SetSlash(char s);
void ConvertPath(char *Path);
int ParseINI(const char *file, char *Error, struct frac_struct *frac, struct view_struct *view, 
   struct realpal_struct *realpal, char *colscheme, double *cutbuf);
int SetDefaults(struct frac_struct *frac, struct view_struct *view, struct realpal_struct *realpal,
   char *colscheme, double *cutbuf);
int ParseAndCalculate(const char *file, char *Error, char zflag);
int TranslateColorFormula(char *colscheme, char *ErrorMSG);
int CleanNumberString(char *s);
int GetParameters(const char *file, char *Error);
int ReadParametersPNG(const char *file, char *Error, struct frac_struct *frac, struct view_struct *view, 
   struct realpal_struct *realpal, char *colscheme, double *cutbuf);
int FormatExternToIntern(struct frac_struct *frac, struct view_struct *view);
int FormatInternToExtern(struct frac_struct *frac, struct view_struct *view);
int ReadParametersAndImage(char *Error, const char *pngfile, 
   unsigned char *ready, int *xstart, int *ystart, struct frac_struct *frac, struct view_struct *view,
   struct realpal_struct *realpal, char *colscheme, double *cutbuf, int zflag);
int SavePNG(char *Error, const char *pngfile, int xstart, int ystart,
   struct disppal_struct *disppal, struct frac_struct *f, struct view_struct *v, 
   struct realpal_struct *realpal, char *colscheme, double *cutbuf, int zflag);
/* Saves the whole QUAT-PNG (retrieved by XXX_getline) */
int BuildName(char *name, char *namewop, const char *ext, const char *file, char *Error);
int CreateImage(char *Error, int *xstart, int *ystart, struct frac_struct *frac, 
   struct view_struct *view, struct realpal_struct *realpal, char *colscheme, double *cutbuf, 
   int noev, int zflag);
/* Creates image from given parameters. Wants external format of frac & view */
int CreateZBuf(char *Error, int *xstart, int *ystart, struct frac_struct *frac, 
   struct view_struct *view, double *cutbuf, int noev);
/* Creates ZBuffer from given parameters. Wants external format of frac & view */
int WriteINI(char *Error, const char *file, int mode, struct frac_struct *frac, struct view_struct *view,
   struct realpal_struct *realpal, char *colscheme, double *cutbuf);
int ImgFromZBuf(const char *file, const char *file2, char *Error); 

#ifdef __cplusplus
}
#endif
