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

#include <stdio.h>      
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include "quat.h"
#include "textver.h"
#ifdef __unix__
#	include <termios.h>
#	include <sys/time.h>
	static struct termios stored_settings;
#elif GCC_DOS
#   include <conio.h>
#endif

extern unsigned char *TEXTVER_zbuf;
extern VER_ReturnVideoInfo ReturnVideoInfo; 
extern VER_check_event check_event; 
extern VER_Debug Debug; 
extern VER_eol eol; 
extern VER_getline QU_getline;  
extern VER_putline putline; 

#ifdef __unix__
/* Disable line buffer */
void set_keypress(void)
{
	struct termios new_settings;
	
	tcgetattr(0,&stored_settings);
	
	new_settings = stored_settings;
	
	/* Disable canonical mode, and set buffer size to 1 byte */
	new_settings.c_lflag &= (~ICANON);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1;
	
	tcsetattr(0,TCSANOW,&new_settings);
	return;
	
}

void reset_keypress(void)
{
	tcsetattr(0,TCSANOW,&stored_settings);
	return;
}

int isready(int fd)
{
	int rc;
	fd_set fds;
	struct timeval tv;
	
	FD_ZERO(&fds);
	FD_SET(fd,&fds);
	tv.tv_sec = tv.tv_usec = 0;
	
	rc = select(fd+1, &fds, NULL, NULL, &tv);
	if (rc < 0) return -1;
	
	return FD_ISSET(fd,&fds) ? 1 : 0;
}

int keypressed(void)
{
	return isready(0);
}

#endif  /* __unix__ */

/* TEXT version special functions */
/*int InitializeZBuf(int x, int y, char *Error)
{
   TEXTVER_zbuf = malloc((size_t)x*(size_t)y*3);
   if (!TEXTVER_zbuf)
   {
      sprintf(Error, "Not enough memory for ZBuffer!");
      return(-1);
   }
   return(0);
} 

int DoneZBuf()
{
   free(TEXTVER_zbuf);
   return(0);
}

*/

int TEXTVER_ReturnVideoInfo(struct vidinfo_struct *vidinfo)
{
   vidinfo->maxcol = -1;   /* Simulate True Color capability */
   return(0);
}

/*int TEXTVER_update_bitmap(unsigned int x1, unsigned int x2, int xres, int j, unsigned char *Buf, int which)
{              
   if (which==0) return(0);
   memcpy(TEXTVER_zbuf+((size_t)xres*(size_t)j+(size_t)x1)*3, Buf+x1*3, (x2-x1+1)*3);
   return(0);
}
*/
int TEXTVER_getline(unsigned char *line, int y, long xres, int whichbuf)
{
   if (whichbuf==0) return(0);
   else memcpy(line, TEXTVER_zbuf+((size_t)xres*(size_t)y)*3, xres*3);
   return(0);
}

int TEXTVER_putline(long x1, long x2, long xres, int y, unsigned char *Buf, int whichbuf)
{
   if (whichbuf==0) return(0);
   else memcpy(TEXTVER_zbuf+(xres*y+x1)*3, Buf+x1*3, (x2-x1+1)*3);
   return(0);
}

int TEXTVER_check_event()
{
#ifdef __unix__
	if (keypressed()) 
	{
		return -128;
	}
#elif GCC_DOS
   if (kbhit()) 
   {
      getch();
      return(-128);
   }
#endif 
   return(0);
}

void TEXTVER_Debug(char *s)
{
}

void TEXTVER_eol(int line)
{
   printf("\rCalculated line %i ",line); fflush(stdout);
   return;
}

void endprg(void)
{
#ifdef __unix__
	reset_keypress();
#endif
}

int main(int argc, char *argv[]) 
{            
   /* char getprm = 0, isfile = 0;*/ 
   int i; 
   char command = 0, argument = -1; 
   char file[256], file2[256], Error[256], *s; 
    
#ifdef __unix__
	set_keypress();
#endif
   file[0] = 0; file2[0] = 0; 
   ReturnVideoInfo = TEXTVER_ReturnVideoInfo; 
   check_event = TEXTVER_check_event; 
   Debug = TEXTVER_Debug; 
   eol = TEXTVER_eol;       
   QU_getline = TEXTVER_getline; 
   putline = TEXTVER_putline;    
   
   if (argc>1) 
   { 
      if (strcmp(argv[1], "-z")==0) command = 1;        /* calc a ZBuffer */ 
      else if (strcmp(argv[1], "-i")==0) command = 2;   /* calc an image */ 
      else if (strcmp(argv[1], "-p")==0) command = 3;   /* write parameters */     
      else if (strcmp(argv[1], "-c")==0) command = 4;   /* continue calculation */ 
      else if (strcmp(argv[1], "-h")==0) command = 5;   /* help */ 
   } 
   if (argc>2)  
   { 
      strcpy(file, argv[2]); 
      if (strrchr(file, '.'))  
      { 
         for (i=0; i<strlen(file); i++) file[i] = tolower((int)file[i]); 
         if (strcmp(strrchr(file, '.'), ".ini")==0) argument = 1;       /* ini file */ 
         else if (strcmp(strrchr(file, '.'), ".png")==0) argument = 2;  /* png file */  
         else if (strcmp(strrchr(file, '.'), ".zpn")==0) argument = 3;  /* zpn file */ 
         else if (file[0]==0) argument = 0; 
         strcpy(file, argv[2]); 
      } 
   } 
   if (argc>3) strcpy(file2, argv[3]); 
   switch (command) 
   { 
      case 1: 
	 if (argument==0) 
	 { 
	    printf("You didn´t specify an ini-file.\nUse -h for help.\n"); 
	    endprg(); return(-1);
	 }            
	 if (argument!=1) 
	 { 
	    printf("The file you specified has not the suffix '.ini', but it will be assumed to\n"); 
	    printf("be an ini file.\n"); 
	 }             
	 ParseAndCalculate(file, Error, 1); 
	 break; 
      case 2: 
	 if (argument==1) 
	 { 
	    ParseAndCalculate(file, Error, 0); 
	 }                                     
	 else if (argument==3) 
	 { 
	    ImgFromZBuf(file, file2, Error); 
	 }     
	 else 
	 { 
	    printf("You didn´t specify an ini- or zpn-file.\nUse -h for help.\n"); 
	    endprg(); return(-1); 
	 }                    
	 break;   
      case 3: 
	 if (argument==0)  
	 {  
	    printf("No file given. Use -h for help.\n"); 
	    endprg(); return(-1); 
	 }      
	 GetParameters(file, Error); 
	 break; 
      case 4: 
	 if (argument!=2 && argument!=3) 
	 { 
	    printf("Given file is neither png nor zpn.\n"); 
	    endprg(); return(-1); 
	 } 
	 if (argument==2) ParseAndCalculate(file, Error, 0); 
	 if (argument==3) ParseAndCalculate(file, Error, 1); 
	  
	 break; 
      case 5: 
	 strncpy(file, argv[0], 256); 
	 s = strrchr(file, '/'); 
	 if (s==NULL) s = strrchr(file, '\\'); 
	 if (s==NULL) s = (char *)&file[0]; else s++; 
	 printf("\n%s %s%s%s\n%s", PROGNAME, PROGVERSION, PROGSUBVERSION, PROGSTATE, 
		"A 3d fractal generation program\n"); 
	 printf("Copyright (C) 1997-2002 Dirk Meyer\n");
	 printf("email: dirk.meyer@studserv.uni-stuttgart.de\n"); 
	 printf("WWW: http://www.physcip.uni-stuttgart.de/phy11733/index_e.html\n");    
	 printf("Mailing-List: http://groups.yahoo.com/group/quat/\n");
	 printf("\nThis program is distributed under the terms of the\n"); 
	 printf("GNU General Public License Version 2 (See file 'COPYING' for details)\n\n"); 
	 printf("-z <ini-file>           begin calculation of a ZBuffer\n"); 
	 printf("-i [<ini-file> | <zpn-file> [& <ini-file>]]\n"); 
	 printf("                        begin calculation of an image [using an ini-file | a\n"); 
	 printf("                        ZBuffer [& replacing parameters from ini-file]]\n"); 
	 printf("-p <png- or zpn-file>   read fractal parameters from image or ZBuffer and\n"); 
	 printf("                        write them to raw data (ini)\n"); 
	 printf("-c <png- or zpn-file>   continue calculation of image or ZBuffer\n"); 
	 printf("-h                      Show help text\n"); 
	 endprg(); return(0); 
      default:  
	 printf("Didn't understand the given parameters. For help use -h\n"); 
	 endprg(); return(-1);                  
   } 
   if (Error[0]!=0) printf("%s\n", Error);  
   endprg();
   return(0); 
} 

