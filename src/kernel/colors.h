#ifdef __cplusplus
extern "C" {
#endif
int CreateDispPal(struct disppal_struct *disppal, struct realpal_struct *realpal, int maxcol, double phongmax, int rdepth, int gdepth, int bdepth);
int PixelvaluePalMode(int x1, int x2, int colmax, int brightmax, unsigned char *line, float *CBuf, float *BBuf);
int PixelvalueTrueMode(int x1, int x2, int rmax, int gmax, int bmax, 
   struct realpal_struct *realpal, unsigned char *line, float *CBuf, float *BBuf);
int CalcWeightsum(struct realpal_struct *realpal);
int FindNearestColor(struct disppal_struct *disppal, unsigned char r, unsigned char g, unsigned char b);
#ifdef __cplusplus
}
#endif
