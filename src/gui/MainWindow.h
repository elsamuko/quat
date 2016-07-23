#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>

#ifndef NO_NAMESPACE
using namespace std;
#endif

#include "structs.h"
#include "CReplacements.h"

class Fl_Window;
class Fl_Widget;
class Fl_Box;
class ScrollWid;
class Fl_Menu_Bar;
class PixWid;
class Fl_Help_Dialog;

class MainWindow {
    public:
        MainWindow( int argc, char** argv, int x = 308, int y = 296, const char* label = PROGNAME );
        ~MainWindow();
        bool shown() const;

        /* These are the functions that are called from the C code during calculation */
        static int FLTK_ReturnVideoInfo( struct vidinfo_struct* vidinfo );
        static int FLTK_SetColors( struct disppal_struct* );
        static int FLTK_Initialize( int x, int y, char* );
        static int FLTK_Done();
        static int FLTK_update_bitmap( long x1, long x2, long xres, int y, unsigned char* Buf, int which );
        static int FLTK_getline( unsigned char* line, int y, long xres, int whichbuf );
        static int FLTK_putline( long x1, long x2, long xres, int y, unsigned char* Buf, int whichbuf );
        static int FLTK_check_event();
        static int FLTK_Change_Name( const char* s );
        static void FLTK_Debug( char* s );
        static void FLTK_eol( int line );
        static void FLTK_eol_4( int line );

        void Image_Open();
        int Image_Close();
        void Image_Save();
        void Image_SaveAs();
        void Image_AdjustWindow();
        void Help_About();
        void Image_Exit();
        void Calculation_StartImage();
        void Calculation_StartZBuf();
        void Parameters_Edit();
        void Parameters_Reset();
        bool Parameters_ReadINI( frac_cpp&, view_cpp&,
                                 realpal_cpp&, colscheme_cpp&, cutbuf_cpp&, const char* fn = NULL );
        bool Parameters_ReadPNG( frac_cpp&, view_cpp&,
                                 realpal_cpp&, colscheme_cpp&, cutbuf_cpp&, bool );
        void Parameters_SaveAs( frac_cpp&, view_cpp&,
                                realpal_cpp&, colscheme_cpp&, cutbuf_cpp& );
        void ZBuffer_Open();
        int ZBuffer_Close();
        //	void ZBuffer_Save();
        void ZBuffer_SaveAs();
        void Help_Manual();
        frac_cpp frac;
        view_cpp view;
        realpal_cpp realpal;
        colscheme_cpp colscheme;
        cutbuf_cpp cutbuf;
        bool auto_resize;
        //	static const unsigned long TYPE;
        //	unsigned long type() { return _type; }
    private:
        void MakeTitle();
        void DoImgOpen( const char* givenfile, int zflag );
        void DoImgClose();
        void DoZBufClose();
        int DoInitMem( int xres, int yres, string& Error, int zflag );
        void DoStartCalc( char zflag );
        static void Image_Open_cb( Fl_Widget*, void* );
        static void Image_Close_cb( Fl_Widget*, void* );
        static void Image_Save_cb( Fl_Widget*, void* );
        static void Image_SaveAs_cb( Fl_Widget*, void* );
        static void Image_AdjustWindow_cb( Fl_Widget*, void* );
        static void Help_About_cb( Fl_Widget*, void* );
        static void Image_Exit_cb( Fl_Widget*, void* );
        static void Calculation_StartImage_cb( Fl_Widget*, void* );
        static void Calculation_StartZBuf_cb( Fl_Widget*, void* );
        static void Calculation_Stop_cb( Fl_Widget*, void* );
        static void Parameters_Edit_cb( Fl_Widget*, void* );
        static void Parameters_Reset_cb( Fl_Widget*, void* );
        static void Parameters_ReadINI_cb( Fl_Widget*, void* );
        static void Parameters_ReadPNG_cb( Fl_Widget*, void* );
        static void Parameters_SaveAs_cb( Fl_Widget*, void* );
        static void ZBuffer_Open_cb( Fl_Widget*, void* );
        static void ZBuffer_Close_cb( Fl_Widget*, void* );
        //	static void ZBuffer_Save_cb(Fl_Widget*, void*);
        static void ZBuffer_SaveAs_cb( Fl_Widget*, void* );
        static void Help_Manual_cb( Fl_Widget*, void* );
        int minsizeX, minsizeY;
        int imgxstart, imgystart, zbufxstart, zbufystart;
        bool stop;
        void menuItemEnabled( int, bool );
        void update();
        bool ImgInMem, ZBufInMem, ImgChanged, ZBufChanged,
             ImgReady, ZBufReady, InCalc;
        Fl_Window* MainWin;
        ostringstream status_text;
        Fl_Box* status;
        ScrollWid* scroll;
        Fl_Widget* pix;
        Fl_Menu_Bar* menubar;
        Fl_Help_Dialog* help;
        unsigned char* ZBuf;
        pathname act_file;
        pathname ini_path, png_path;
        char* _status_text_char;
        //	const unsigned long _type;
};

#endif
