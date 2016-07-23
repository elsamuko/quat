/* Quat - A 3D fractal generation program */
/* Copyright (C) 1997-2002 Dirk Meyer */
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

#include <ctime>	// time_t
#include <cstdlib>	// atof
#include <new>
#ifndef WIN32
#	include <sys/time.h>
#endif

#include <sstream>
#include <iostream>
#include <tuple>
#include <string>
#if __APPLE__
#include <mach-o/dyld.h>
#elif __linux__
#include <unistd.h>
#elif _WIN32
#include <Windows.h>
#include <cstdint>
#endif

#ifndef NO_NAMESPACE
using namespace std;
#endif

#include "MainWindow.h"
#include "colors.h"
#include "quat.h"
#include "ScrollWid.h"
#include "PixWid.h"
#include "about.h"
#include "ParameterEditor.h"
#include "WriteIni.h"
#include "title.xpm"
#include "ImageWid.h"

#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_draw.H>
#include <FL/fl_message.H>
//#include <FL/fl_file_chooser.H>
#include <FL/FL_Native_File_Chooser.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/x.H>

extern "C" VER_ReturnVideoInfo ReturnVideoInfo;
extern "C" VER_SetColors SetColors;
extern "C" VER_Initialize Initialize;
extern "C" VER_Done Done;
extern "C" VER_update_bitmap update_bitmap;
extern "C" VER_getline QU_getline;
extern "C" VER_putline putline;
extern "C" VER_check_event check_event;
extern "C" VER_Change_Name Change_Name;
extern "C" VER_Debug Debug;
extern "C" VER_eol eol;

// Those ugly global variables are neccessary for communication with C source
MainWindow* MainWinPtr = NULL;
char Error[1024];
extern "C" time_t calc_time;
time_t old_time = 0;

void MainWindow::Image_Open_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Image_Open();
}

void MainWindow::Image_Close_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Image_Close();
}

void MainWindow::Image_Save_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Image_Save();
}

void MainWindow::Image_SaveAs_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Image_SaveAs();
}

void MainWindow::Help_About_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Help_About();
}

void MainWindow::Image_AdjustWindow_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Image_AdjustWindow();
}

void MainWindow::Image_Exit_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Image_Exit();
}

void MainWindow::Calculation_StartImage_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Calculation_StartImage();
}

void MainWindow::Calculation_StartZBuf_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Calculation_StartZBuf();
}

void MainWindow::Calculation_Stop_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->stop = true;
}

void MainWindow::Parameters_Edit_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Parameters_Edit();
}

void MainWindow::Parameters_Reset_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Parameters_Reset();
}

void MainWindow::Parameters_ReadINI_cb( Fl_Widget*, void* v ) {
    MainWindow* p = reinterpret_cast<MainWindow*>( v );
    p->Parameters_ReadINI( p->frac, p->view, p->realpal,
                           p->colscheme, p->cutbuf );
}

void MainWindow::Parameters_ReadPNG_cb( Fl_Widget*, void* v ) {
    MainWindow* p = reinterpret_cast<MainWindow*>( v );
    p->Parameters_ReadPNG( p->frac, p->view, p->realpal,
                           p->colscheme, p->cutbuf, p->ZBufInMem );
}

void MainWindow::Parameters_SaveAs_cb( Fl_Widget*, void* v ) {
    MainWindow* p = reinterpret_cast<MainWindow*>( v );
    p->Parameters_SaveAs( p->frac, p->view, p->realpal,
                          p->colscheme, p->cutbuf );
}

void MainWindow::ZBuffer_Open_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->ZBuffer_Open();
}

void MainWindow::ZBuffer_Close_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->ZBuffer_Close();
}

void MainWindow::ZBuffer_SaveAs_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->ZBuffer_SaveAs();
}

void MainWindow::Help_Manual_cb( Fl_Widget*, void* v ) {
    reinterpret_cast<MainWindow*>( v )->Help_Manual();
}

int switch_callback( int argc, char* argv[], int& i ) {
    // Ugly method to get parameters without global variables...
    static int paramidx[] = { -1 };
    static char* param[] = { NULL };

    if( argc < 0 ) {
        int idx = -argc - 1;
        assert( idx == 0 );

        if( paramidx[idx] == -1 ) {
            return -1;
        }

        argv[0] = param[idx];
        return 0;
    }

    // regular callback
    string s( argv[i] );

    // Seems not to be a file, but an option?
    if( s[0] == '-' ) {
        return 0;
    }

    // File already given?
    if( paramidx[0] != -1 ) {
        return 0;
    }

    paramidx[0] = i;
    param[0] = argv[i++];

    return 1;
}

std::tuple<std::string, std::string> applicationPath() {

    std::string dir;  // e.g. /Applications/Quat.app/Contents/MacOS
    std::string base; // e.g. Quat

    std::string fullPath( 1024, '\0' );
    uint32_t size = fullPath.size();

#if __APPLE__
    bool ok = _NSGetExecutablePath( ( char* )fullPath.c_str(), &size ) == 0;
#elif __linux__
    size = readlink( "/proc/self/exe", ( char* )fullPath.c_str(), size );
    bool ok = size > 0;
#elif _WIN32
    size = GetModuleFileNameA( 0, ( char* )fullPath.c_str(), size );
    bool ok = size > 0;
#endif
    if( ok ) {
        fullPath.resize( size );
    } else {
        fullPath.clear();
    }

    if( !fullPath.empty() ) {

#if _WIN32
        size_t pos = fullPath.find_last_of( '\\' );
#else
        size_t pos = fullPath.find_last_of( '/' );
#endif

        if( pos != std::string::npos ) {
            dir = fullPath.substr( 0, pos );
            base = fullPath.substr( pos );
        }
    }

    return std::make_tuple( dir, base );
}

MainWindow::MainWindow( int argc, char** argv, int w, int h, const char* label )
    : auto_resize( false ), minsizeX( 200 ), minsizeY( 150 ), imgxstart( 0 ), imgystart( 0 ),
      zbufxstart( 0 ), zbufystart( 0 ), stop( false ),
      ImgInMem( false ), ZBufInMem( false ), ImgChanged( false ), ZBufChanged( false ),
      ImgReady( false ), ZBufReady( false ), InCalc( false ),
      pix( 0 ), help( new Fl_Help_Dialog ), ZBuf( 0 ),
      act_file( "Noname.png" ), ini_path( std::get<0>( applicationPath() ) + INIPATH ), png_path( "./" ),
      _status_text_char( 0 ) {
    assert( MainWinPtr == 0 );
    MainWinPtr = this;
#ifdef WIN32
    SetSlash( '\\' );
#endif
    status_text << "Dummy.";
    status_text.seekp( 0 );
    Fl_Menu_Item tmp[] = {
        {"&Image", FL_ALT + 'f', 0, 0, FL_SUBMENU, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"&Open...", 0, Image_Open_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"&Close", 0, Image_Close_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"&Save", 0, Image_Save_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"Save &As...", 0, Image_SaveAs_cb, this, FL_MENU_DIVIDER, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {
            "Adjust &Window...", 0, Image_AdjustWindow_cb, this, FL_MENU_DIVIDER, FL_NORMAL_LABEL,
            FL_HELVETICA, 12, FL_FOREGROUND_COLOR
        },
        {"E&xit...", 0, Image_Exit_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {"&Calculation", FL_ALT + 'c', 0, 0, FL_SUBMENU, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {
            "Start / Resume an &image", 0, Calculation_StartImage_cb, this, 0, FL_NORMAL_LABEL,
            FL_HELVETICA, 12, FL_FOREGROUND_COLOR
        },
        {
            "Start / Resume a &ZBuffer", 0, Calculation_StartZBuf_cb, this, 0, FL_NORMAL_LABEL,
            FL_HELVETICA, 12, FL_FOREGROUND_COLOR
        },
        {"Stop", 0, Calculation_Stop_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {"&Parameters", FL_ALT + 'p', 0, 0, FL_SUBMENU, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"&Edit...", 0, Parameters_Edit_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"&Reset", 0, Parameters_Reset_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"Read from &INI...", 0, Parameters_ReadINI_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"Read from &PNG...", 0, Parameters_ReadPNG_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"Save &As...", 0, Parameters_SaveAs_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {"&ZBuffer", FL_ALT + 'f', 0, 0, FL_SUBMENU, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"&Open...", 0, ZBuffer_Open_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"&Close", 0, ZBuffer_Close_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"Save &As...", 0, ZBuffer_SaveAs_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {"&Help", FL_ALT + 'h', 0, 0, FL_SUBMENU, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"&Manual", 0, Help_Manual_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {"A&bout...", 0, Help_About_cb, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    SetDefaults( &frac, &view, &realpal, colscheme, cutbuf );
    MainWin = new Fl_Double_Window( w, h, label );
    MainWin->callback( MainWindow::Image_Exit_cb, this );
    MainWin->size_range( minsizeX, minsizeY );
    MainWin->box( FL_NO_BOX );
    status = new Fl_Box( 0, MainWin->h() - 20, MainWin->w(), 20, "Status" );
    status->box( FL_UP_BOX );
    status->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
    status->labelsize( 12 );
    scroll = new ScrollWid( 0, 25, /*320*/MainWin->w(), MainWin->h() - 25 - status->h() );
    MakeTitle();
    scroll->end();
    menubar = new Fl_Menu_Bar( 0, 0, MainWin->w(), 25 );
    menubar->copy( tmp );
    MainWin->end();
    MainWin->resizable( scroll );

    /*	for (int i=1; i<argc; i++) if (argv[i][0] != '-') {
            pathname tmp(argv[i]);
            tmp.uppercase();
            if (tmp.ext() == ".PNG") DoImgOpen(tmp.c_str(), 0);
            else if (tmp.ext() == ".ZPN") DoImgOpen(tmp.c_str(), 1);
            else if (tmp.ext() == ".INI" || tmp.ext() == ".OBJ" || tmp.ext() == ".COL")
                Parameters_ReadINI(frac, view, realpal, colscheme, cutbuf, tmp.c_str());
            else DoImgOpen(tmp.c_str(), 0);

    //		char *ext_tmp = strrchr(argv[i], '.');
    //		if (!ext_tmp) DoImgOpen(argv[i], 0);
    //		else {
    //			char *ext = new char[strlen(ext_tmp)+1];
    //			strcpy (ext, ext_tmp);
    //			for (unsigned int j = 0; j<strlen(ext); j++) ext[j] = toupper(ext[j]);
    //			if (strcmp(ext, ".PNG")==0) DoImgOpen(argv[i], 0);
    //			else if (strcmp(ext, ".ZPN")==0) DoImgOpen(argv[i], 1);
    //			else if (strcmp(ext, ".INI")==0 || strcmp(ext, ".OBJ")==0
    //				|| strcmp(ext, ".COL")==0)
    //				Parameters_ReadINI(frac, view, realpal, colscheme, cutbuf, argv[i]);
    //			else DoImgOpen(argv[i], 0);
    //		}

            for (int j=i; j<argc-1; j++) argv[j] = argv[j+1];
            argc--;
            break;
        }
    */

    int sw_i = 0;

    if( Fl::args( argc, argv, sw_i, switch_callback ) < argc ) {
        cout << "Quat options:" << endl <<
             "  filename.[png|zpn|ini|col] Open file." << endl <<
             "FLTK options:" << endl << Fl::help << endl;
    }

    //	char *param[1];
    //	if (switch_callback(-1, param, sw_i) == 0) {
    //		pathname tmp(param[0]);
    //		pathname tmp_upper(tmp);
    //		tmp_upper.uppercase();
    //		if (tmp_upper.ext() == ".PNG") DoImgOpen(tmp.c_str(), 0);
    //		else if (tmp_upper.ext() == ".ZPN") DoImgOpen(tmp.c_str(), 1);
    //		else if (tmp_upper.ext() == ".INI" || tmp.ext() == ".OBJ" || tmp.ext() == ".COL")
    //			Parameters_ReadINI(frac, view, realpal, colscheme, cutbuf, tmp.c_str());
    //		else DoImgOpen(tmp.c_str(), 0);
    //	}

    act_file.auto_name();
    static string title;
    title = PROGNAME;
    title += " - ";
    title += act_file.file();
    MainWin->label( title.c_str() );
    update();
    Image_AdjustWindow();
#ifdef WIN32
    MainWin->icon( ( char* )LoadIcon( fl_display, MAKEINTRESOURCE( IDI_ICON1 ) ) );
#elif __unix__
    Fl_PNG_Image* icon = new Fl_PNG_Image( "logo.png" );
    MainWin->icon( icon );
#endif

    pathname helpfile;
#ifdef DOCDIR
    helpfile = std::get<0>( applicationPath() ) + std::string( DOCDIR ) + "/quat-us.html";
#else
    helpfile = "/quat-us.html";
#endif
    pathname prgpath( argv[0] );
    prgpath = prgpath.path();

    if( !helpfile.exists() ) {
        const char* search[] = { "../doc/quat-us.html",
                                 "doc/quat-us.html", "quat-us.html",
                                 "../share/doc/quat/quat-us.html",
                                 "../doc/quat/quat-us.html",
                                 "../share/quat/quat-us.html"
                               };
        int no = sizeof( search ) / sizeof( const char* );
        int idx = 0;

        do {
            helpfile = prgpath + search[idx];
            ++idx;
        } while( !helpfile.exists() && idx < no );
    }

#ifdef DOCDIR

    // Simply to let Fl_Help_Dialog output an error message
    // with the *most important* path.
    if( !helpfile.exists() ) {
        helpfile = DOCDIR "/quat-us.html";
    }

#endif

    help->load( helpfile.c_str() );
    MainWin->show( argc, argv );

    char* param[1];

    if( switch_callback( -1, param, sw_i ) == 0 ) {
        pathname tmp( param[0] );
        pathname tmp_upper( tmp );
        tmp_upper.uppercase();

        if( tmp_upper.ext() == ".PNG" ) {
            DoImgOpen( tmp.c_str(), 0 );
        } else if( tmp_upper.ext() == ".ZPN" ) {
            DoImgOpen( tmp.c_str(), 1 );
        } else if( tmp_upper.ext() == ".INI" || tmp.ext() == ".OBJ" || tmp.ext() == ".COL" ) {
            Parameters_ReadINI( frac, view, realpal, colscheme, cutbuf, tmp.c_str() );
        } else {
            DoImgOpen( tmp.c_str(), 0 );
        }

        //		act_file.auto_name();
        //		static string title;
        //		title = PROGNAME; title += " - "; title += act_file.file();
        //		MainWin->label(title.c_str());
        update();
        //		Image_AdjustWindow();
    }
}

MainWindow::~MainWindow() {
    delete MainWin;
    delete help;
    MainWinPtr = 0;
    //	delete[] ZBuf;
    operator delete[]( ZBuf, nothrow );
    delete[] _status_text_char;
}

void MainWindow::MakeTitle() {
    int wp = 0, hp = 0;
    int suc = fl_measure_pixmap( title, wp, hp );

    if( suc ) {
        pix = new PixWid( 0, 25, wp, hp );
        reinterpret_cast<PixWid*>( pix )->setPixmap( ( char* const* ) title );
    } else {
        pix = new Fl_Button( 0, 0, 300, 200, "There was a problem\nanalyzing the XPM image." );
    }

    scroll->widget( pix );
}

bool MainWindow::shown() const {
    return MainWin->shown();
}

void MainWindow::DoImgOpen( const char* givenfile, int zflag ) {
    Error[0] = 0;
    pathname filename;

    status_text.seekp( 0 );

    if( !givenfile ) {
		Fl_Native_File_Chooser fnfc;
		fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
		fnfc.directory( png_path.c_str() );
        
		if( zflag == 0 ) {
			fnfc.title("Open Image");
			fnfc.filter("Images\t*.{png,PNG,Png}");
        } else {
			fnfc.title("Open ZBuffer");
			fnfc.filter("ZBuffers\t*.{zpn,ZPN,Zpn}");			
        }

		switch ( fnfc.show() ) {
			case -1: fl_alert("%s", fnfc.errmsg()); update(); return;
			case  1: update(); return;	// Cancel
			default: filename = fnfc.filename();
		}

        png_path = filename.path();
    } else {
        filename = givenfile;
    }

    if( filename.size() == 0 ) {
        return;
    }

    ReturnVideoInfo = ( VER_ReturnVideoInfo )FLTK_ReturnVideoInfo;
    SetColors = ( VER_SetColors )FLTK_SetColors;
    Initialize = ( VER_Initialize )FLTK_Initialize;
    Done = ( VER_Done )FLTK_Done;
    update_bitmap = ( VER_update_bitmap )FLTK_update_bitmap;
    QU_getline = ( VER_getline )FLTK_getline;
    putline = ( VER_putline )FLTK_putline;
    check_event = ( VER_check_event )FLTK_check_event;
    Change_Name = ( VER_Change_Name )FLTK_Change_Name;
    Debug = ( VER_Debug )FLTK_Debug;
    eol = ( VER_eol )FLTK_eol;
    frac_cpp _f;
    view_cpp _v;
    realpal_cpp _r;
    colscheme_cpp _col;
    cutbuf_cpp _cut;

    if( ReadParametersPNG( filename.c_str_conv(), Error, &_f, &_v, &_r, _col, _cut ) ) {
        fl_alert( "%s", Error );

        if( zflag == 0 ) {
            ImgChanged = false;
            DoImgClose();
        } else {
            ZBufChanged = false;
            DoZBufClose();
        }

        return;
    }

    frac = _f;
    view = _v;
    realpal = _r;
    colscheme = _col;
    cutbuf = _cut;

    int xres = view.xres;

    if( zflag != 0 ) {
        xres *= view.antialiasing;
    }

    if( view.interocular != 0 ) {
        xres *= 2;
    }

    int yres = view.yres;

    if( zflag != 0 ) {
        yres *= view.antialiasing;
    }

    string error;

    if( DoInitMem( xres, yres, error, zflag ) ) {
        fl_alert( "%s", error.c_str() );
        return;
    }

    int xs, ys;
    unsigned char ready;

    if( ReadParametersAndImage( Error, filename.c_str_conv(), &ready, &xs, &ys,
                                &_f, &_v, &_r, _col, _cut, zflag ) ) {
        fl_alert( "%s", Error );

        if( zflag == 0 ) {
            ImgChanged = 0;
            DoImgClose();
        } else {
            ZBufChanged = 0;
            DoZBufClose();
        }

        return;
    }

    if( zflag == 0 ) {
        static string title;
        act_file = filename.c_str();
        ImgReady = ( ready != 0 );
        ImgChanged = false;
        ImgInMem = true;
        imgxstart = xs;
        imgystart = ys;
        title = PROGNAME;
        title += " - ";
        title += act_file.file();
        MainWin->label( title.c_str() );
    } else {
        ZBufReady = ( ready != 0 );
        ZBufChanged = false;
        ZBufInMem = true;
        zbufxstart = xs;
        zbufystart = ys;
    }

    MainWin->redraw();
    pathname tmp( filename.c_str() );

    if( ( zflag == 0 && ImgReady ) || ( zflag != 0 && ZBufReady ) ) {
        status_text.seekp( 0 );
        status_text << tmp.file() << ": Finished. Created by Quat "
                    << Error << ".";
    } else {
        status_text.seekp( 0 );
        status_text << tmp.file() << ": In line " << ys
                    << ", created by Quat " << Error << ".";
    }

    if( atof( Error ) > atof( PROGVERSION ) )
        fl_message( "Warning: File created by a higher version of Quat!\n"
                    "Unpredictable if it will work..." );

    return;
}

void MainWindow::DoZBufClose() {
}

void MainWindow::DoImgClose() {

}

int MainWindow::DoInitMem( int xres, int yres, string& error, int zflag ) {
    // Maximum size of Fl_Widget is 32767...
    if( xres <= 0 || yres <= 0 || xres >= 32768 || yres >= 32768 ) {
        error = "Resolution out of range. The GUI version (currently "
                "running)\nhas a limit of 32,767; the console version "
                "can process\nresolutions up to 100,000.";
        return -1;
    }

    ImageWid* pixtmp = new ImageWid( 0, 25, xres, yres );

    if( !pixtmp->valid() ) {
        if( ZBufInMem )
            error = "Couldn't create image (probably not enough memory.)"
                    " Try to close the ZBuffer and calculate directly.";
        else {
            error = "Couldn't create image (probably not enough memory.)";
        }

        delete pixtmp;
        //		MakeTitle();
        return -1;
    }


    pixtmp->white();
    scroll->widget( pixtmp );
    scroll->redraw();
    pix = reinterpret_cast<Fl_Widget*>( pixtmp );

    if( zflag ) {
        ZBuf = new( nothrow ) unsigned char[static_cast<long>( xres )*static_cast<long>( yres ) * 3L];

        if( ZBuf == 0 ) {
            error = "Couldn't allocate memory for ZBuffer.";
            MakeTitle();
            return -1;
        }
    }

    return 0;
}

void MainWindow::DoStartCalc( char zflag ) {
    ReturnVideoInfo = FLTK_ReturnVideoInfo;
    SetColors = ( VER_SetColors )FLTK_SetColors;
    Initialize = ( VER_Initialize )FLTK_Initialize;
    Done = ( VER_Done )FLTK_Done;
    update_bitmap = ( VER_update_bitmap )FLTK_update_bitmap;
    QU_getline = ( VER_getline )FLTK_getline;
    putline = ( VER_putline )FLTK_putline;
    check_event = ( VER_check_event )FLTK_check_event;
    Change_Name = ( VER_Change_Name )FLTK_Change_Name;
    Debug = ( VER_Debug )FLTK_Debug;

    if( zflag != 2 ) {
        eol = ( VER_eol )FLTK_eol;
    } else {
        eol = ( VER_eol )FLTK_eol_4;
    }

    InCalc = true;
    update();
}

void MainWindow::Image_Open() {
    MainWin->cursor( FL_CURSOR_WAIT );
    menubar->deactivate();
    DoImgOpen( NULL, 0 );
    update();
    menubar->activate();
    MainWin->cursor( FL_CURSOR_DEFAULT );
}

int MainWindow::Image_Close() {
    assert( ImgInMem );
    status_text.seekp( 0 );

    if( ImgChanged ) {
        int choice = fl_choice( "Image: There are unsaved modifications. Do you really want to close?", "Abort", "Close", 0 );

        if( choice != 1 ) {
            update();
            return 0;
        }
    }

    if( ZBufInMem ) {
        MainWin->cursor( FL_CURSOR_WAIT );
        int picx = view.xres * view.antialiasing;

        if( view.interocular != 0.0 ) {
            picx *= 2;
        }

        int picy = view.yres * view.antialiasing;
        ImageWid* pixtmp = new ImageWid( 0, 25, picx, picy );

        if( !pixtmp->valid() ) {
            fl_alert( "Couldn't create ZBuffer pixmap (probably not enough memory.)" );
            delete pixtmp;
            update();
            return 1;
        }

        pix = reinterpret_cast<Fl_Widget*>( pixtmp );

        for( int j = 0; j < picy; j++ ) {
            for( int i = 0; i < picx; i++ ) {
                long l = static_cast<long>( ZBuf[3 * ( i + j * picx )] ) << 16
                         | static_cast<long>( ZBuf[3 * ( i + j * picx ) + 1] ) << 8
                         | static_cast<long>( ZBuf[3 * ( i + j * picx ) + 2] );
                unsigned char R = 255 - static_cast<unsigned char>
                                  ( 2.55 * static_cast<float>( l ) / static_cast<float>( view.zres ) );
                pixtmp->set_pixel( i, j, R, R, R );
            }
        }

        scroll->widget( pix );

        if( auto_resize ) {
            Image_AdjustWindow();
        }

        MainWin->cursor( FL_CURSOR_DEFAULT );
    } else {
        MakeTitle();
    }

    //	RGBmap->reset();
    ImgInMem = false;
    ImgReady = false;
    ImgChanged = false;
    calc_time = old_time;
    imgxstart = 0;
    imgystart = 0;

    act_file.auto_name();
    static string title;
    title = PROGNAME;
    title += " - ";
    title += act_file.file();
    MainWin->label( title.c_str() );

    if( !ZBufInMem && auto_resize ) {
        Image_AdjustWindow();
    }

    MainWin->redraw();

    update();
    return 1;
}

void MainWindow::Image_Save() {
    Error[0] = 0;
    MainWin->cursor( FL_CURSOR_WAIT );

    status_text.seekp( 0 );

    if( SavePNG( Error, act_file.c_str_conv(), 0, imgystart, NULL, &frac,
                 &view, &realpal, colscheme, cutbuf, 0 ) != 0 ) {
        fl_alert( "%s", Error );
        MainWin->cursor( FL_CURSOR_DEFAULT );
        update();
        return;
    }

    MainWin->cursor( FL_CURSOR_DEFAULT );
    ImgChanged = false;
    status_text.seekp( 0 );
    status_text << "Image '" << act_file.c_str() << "' written successfully.";
    update();
    return;
}

void MainWindow::Image_SaveAs() {
    Error[0] = 0;

    status_text.seekp( 0 );
	
	pathname file;
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Save Image As");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.filter("Images\t*.{png,PNG,Png}");
	fnfc.directory(act_file.c_str());
	switch ( fnfc.show() ) {
		case -1: fl_alert("%s", fnfc.errmsg()); update(); return;
		case  1: update(); return;	// Cancel
		default: file = fnfc.filename();
	}

    png_path = file.path();
    file.ext( ".png" );

    if( file.exists() ) {
        int choice = fl_choice( "Selected file already exists. Overwrite?", "Abort", "Overwrite", 0 );

        if( choice != 1 ) {
            update();
            return;
        }
    }

    MainWin->cursor( FL_CURSOR_WAIT );

    if( SavePNG( Error, file.c_str_conv(), 0, imgystart, NULL, &frac, &view,
                 &realpal, colscheme, cutbuf, 0 ) != 0 ) {
        fl_alert( "%s", Error );
        MainWin->cursor( FL_CURSOR_DEFAULT );
        update();
        return;
    }

    MainWin->cursor( FL_CURSOR_DEFAULT );
    ImgChanged = false;

    act_file = file;
    static string title;
    title = PROGNAME;
    title += " - ";
    title += act_file.file();
    MainWin->label( title.c_str() );

    status_text.seekp( 0 );
    status_text << "Image '" << file.c_str() << "' written successfully.";

    update();
    return;
}

void MainWindow::Image_AdjustWindow() {
    int w = pix->w(), h = pix->h() + menubar->h() + status->h();

    if( w < minsizeX ) {
        w = minsizeX;
    }

    if( h < minsizeY ) {
        h = minsizeY;
    }

    if( w > Fl::w() ) {
        w = Fl::w();
    }

    if( h > Fl::h() ) {
        h = Fl::h();
    }

    MainWin->size( w, h );

    status_text.seekp( 0 );
    update();
}

void MainWindow::Help_About() {
    static string a;
    AboutBox* about = new AboutBox();
    a = PROGNAME;
    a += " ";
    a += PROGVERSION;
    a += PROGSUBVERSION;
    a += PROGSTATE;
    about->state->label( a.c_str() );
    about->note->label( COMMENT );
    about->run();
    delete about;

    status_text.seekp( 0 );
    update();
}

void MainWindow::Image_Exit() {
    if( ImgChanged || ZBufChanged ) {
        int choice = fl_choice( "There are unsaved modifications. Do you really want to exit?", "Abort", "Exit", 0 );

        if( choice != 1 ) {
            return;
        }
    }

    help->hide();
    MainWin->hide();
}

void MainWindow::Help_Manual() {
    help->show();
}

void MainWindow::Calculation_StartImage() {
    char zflag;
    int xres, i;

    if( ZBufInMem ) {
        zflag = 2;
        old_time = calc_time;
    } else {
        zflag = 0;
        old_time = 0;
    }

    DoStartCalc( zflag );
    xres = view.xres;

    if( view.interocular != 0 ) {
        xres *= 2;
    }

    string error;

    if( !ImgInMem ) if( DoInitMem( xres, view.yres, error, 0 ) ) {
            fl_alert( "%s", error.c_str() );
            InCalc = false;
            status_text.seekp( 0 );
            update();
            return;
        }

    Error[0] = 0;
#ifdef WIN32
    const int pixels_per_check = 8;
#else
    const int pixels_per_check = 1;
#endif
    i = CreateImage( Error, &imgxstart, &imgystart, &frac, &view, &realpal,
                     colscheme, cutbuf, pixels_per_check, zflag );
    InCalc = false;

    if( i == 0 || i == -2 || i == -128 ) {
        ImgInMem = true;
        ImgReady = false;
        ImgChanged = true;
    }

    if( imgystart == view.yres ) {
        ImgReady = true;
    }

    if( i != -2 ) {
        if( Error[0] != 0 ) {
            fl_alert( "%s", Error );
        }

        status_text.seekp( 0 );
        update();
    }

    status_text.seekp( 0 );
    update();
    return;
}

void MainWindow::Calculation_StartZBuf() {
    string error;
    assert( !ImgInMem );
    Error[0] = 0;
    long xres = view.xres * view.antialiasing;

    if( view.interocular != 0 ) {
        xres *= 2;
    }

    if( !ZBufInMem ) if( DoInitMem( xres, view.yres * view.antialiasing, error, 1 ) ) {
            fl_alert( "%s", error.c_str() );
            status_text.seekp( 0 );
            update();
            return;
        }

    DoStartCalc( 1 );
    ZBufChanged = true;
    int i = CreateZBuf( Error, &zbufxstart, &zbufystart, &frac, &view, cutbuf, 8 );

    if( i == 0 || i == -2 || i == -128 ) {
        ZBufInMem = true;
    }

    if( zbufystart == view.yres ) {
        ZBufReady = true;
    }

    if( i != -2 ) {
        if( Error[0] != 0 ) {
            fl_alert( "%s", Error );
        }

        InCalc = false;
    }

    status_text.seekp( 0 );
    update();
    return;
}

void MainWindow::Parameters_Edit() {
    ParameterEditor* editor = new ParameterEditor();
    editor->set( frac, view, realpal, colscheme, cutbuf );

    if( ImgInMem ) {
        editor->SetState( 1 );
    } else if( ZBufInMem ) {
        editor->SetState( 2 );
    }

    menubar->deactivate();

    if( editor->run() ) {
        editor->get( frac, view, realpal, colscheme, cutbuf );
    }

    menubar->activate();
    delete editor;

    status_text.seekp( 0 );
    update();
    return;
}

void MainWindow::Parameters_Reset() {

    int choice = fl_choice( "This will reset all parameters to their default values. Continue?", "Abort", "Reset", 0 );

    if( choice == 1 ) {
        SetDefaults( &frac, &view, &realpal, colscheme, cutbuf );
    }

    status_text.seekp( 0 );
    update();
}

bool MainWindow::Parameters_ReadINI( frac_cpp& f, view_cpp& v, realpal_cpp& r,
                                     colscheme_cpp& col, cutbuf_cpp& cut, const char* fn ) {
    Error[0] = 0;

    status_text.seekp( 0 );
    pathname filename;

    if( fn != NULL ) {
        filename = fn;
    } else {
		Fl_Native_File_Chooser fnfc;
		fnfc.title("Read parameters from INI file");
		fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
		fnfc.filter("INI files\t*.{ini,col,obj,INI,COL,OBJ,Ini,Col,Obj}");
		fnfc.directory(ini_path.c_str());
		switch ( fnfc.show() ) {
			case -1: fl_alert("%s", fnfc.errmsg()); update(); return false;
			case  1: update(); return false; 	// CANCEL
			default: filename = fnfc.filename();
		}
	}

    ini_path = filename.path();

    frac_cpp _f = f;
    view_cpp _v = v;
    realpal_cpp _r = r;
    colscheme_cpp _col = col;
    cutbuf_cpp _cut = cut;

    int choice = fl_choice( "Do you want to start from default parameters? (Recommended.)", "No", "Yes", 0 );

    if( choice == 1 ) {
        SetDefaults( &_f, &_v, &_r, _col, _cut );
    }

    int i = ParseINI( filename.c_str_conv(), Error, &_f, &_v, &_r, _col, _cut );

    if( i != 0 ) {
        fl_alert( "%s", Error );
        update();
        return false;
    }

    if( ZBufInMem ) {
        for( int i = 0; i < 3; i++ ) {
            v.light[i] = _v.light[i];
        }

        v.ambient = _v.ambient;
        v.phongmax = _v.phongmax;
        v.phongsharp = _v.phongsharp;
        r = _r;
        col = _col;
        status_text.seekp( 0 );
        status_text << "Read only the modifyable parameters for a Z Buffer.";
        update();
        return true;
    }

    f = _f;
    v = _v;
    r = _r;
    col = _col;
    cut = _cut;
    status_text.seekp( 0 );

    if( choice == 1 ) {
        status_text << "Parameters read successfully.";
    } else {
        status_text << "Parameters added to current ones.";
    }

    update();
    return true;
}

bool MainWindow::Parameters_ReadPNG( frac_cpp& f, view_cpp& v, realpal_cpp& r,
                                     colscheme_cpp& col, cutbuf_cpp& cut, bool zbuf ) {
    Error[0] = 0;

    status_text.seekp( 0 );

	pathname fn;
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Import parameters from PNG file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
	fnfc.filter("Images\t*.{png,PNG,Png}");
	fnfc.directory(png_path.c_str());
	switch ( fnfc.show() ) {
		case -1: fl_alert("%s", fnfc.errmsg()); update(); return false;
		case  1: update(); return false; 	// CANCEL
		default: fn = fnfc.filename();
	}
    
    png_path = fn.path();

    frac_cpp _f = f;
    view_cpp _v = v;
    realpal_cpp _r = r;
    colscheme_cpp _col = col;
    cutbuf_cpp _cut = cut;
    int i = ReadParametersPNG( fn.c_str_conv(), Error, &_f, &_v, &_r, _col, _cut );

    if( i != 0 ) {
        fl_alert( "%s", Error );
        update();
        return false;
    }

    if( zbuf ) {
        for( i = 0; i < 3; i++ ) {
            v.light[i] = _v.light[i];
        }

        v.ambient = _v.ambient;
        v.phongmax = _v.phongmax;
        v.phongsharp = _v.phongsharp;
        r = _r;
        col = _col;
        status_text.seekp( 0 );
        status_text << "Imported only the modifyable parameters for a Z Buffer.";
        update();
        return true;
    }

    f = _f;
    v = _v;
    r = _r;
    col = _col;
    cut = _cut;
    status_text.seekp( 0 );
    status_text << "Parameters imported successfully.";
	update();
    return true;
}

void MainWindow::Parameters_SaveAs( frac_cpp& f, view_cpp& v,
                                    realpal_cpp& r, colscheme_cpp& col, cutbuf_cpp& cut ) {
    Error[0] = 0;

    int mode;
    status_text.seekp( 0 );

    if( !WriteINI( mode ) ) {
        update();
        return;
    }

    string canonical_ext( ".ini" );

    if( mode == PS_OBJ ) {
        canonical_ext = ".obj";
    } else if( mode == PS_COL ) {
        canonical_ext = ".col";
    }

    string ininame( ini_path );
    ininame += act_file.filename();
    ininame += canonical_ext;

    string filter( "INI files\t*.{" );
    filter += canonical_ext.substr(1);
	filter += ",";
	for (size_t i = 1; i < canonical_ext.size(); ++i)
		filter += std::toupper(canonical_ext[i]);
	filter += ",";
	filter += std::toupper(canonical_ext[1]);
	filter += canonical_ext.substr(2);
    filter += "}";
	
	pathname file;
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Write parameters to INI file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.filter(filter.c_str());
	fnfc.directory(ininame.c_str());
	switch ( fnfc.show() ) {
		case -1: fl_alert("%s", fnfc.errmsg()); update(); return;
		case  1: update(); return; 	// CANCEL
		default: file = fnfc.filename();
	}

    ini_path = file.path();

    file.ext( canonical_ext.c_str() );

    if( file.exists() ) {
        int choice = fl_choice( "Selected file '%s' already exists. Overwrite?", "Abort", "Overwrite", 0, file.c_str() );

        if( choice != 1 ) {
            update();
            return;
        }
    }

    status_text.seekp( 0 );

    if( WriteINI( Error, file.c_str_conv(), mode, &f, &v, &r, col, cut ) ) {
        fl_alert( "Couldn't write file '%s'.", file.c_str() );
    } else {
        status_text << "File '" << file.c_str() << "' was written successfully.";
    }

    update();
}

void MainWindow::ZBuffer_Open() {
    MainWin->cursor( FL_CURSOR_WAIT );
    menubar->deactivate();
    DoImgOpen( NULL, 1 );
    update();
    menubar->activate();
    MainWin->cursor( FL_CURSOR_DEFAULT );
}

int MainWindow::ZBuffer_Close() {
    assert( ZBufInMem );
    assert( !ImgInMem );
    status_text.seekp( 0 );

    if( ZBufChanged ) {
        int choice = fl_choice( "ZBuffer: There are unsaved modifications. Do you want to close?", "Abort", "Close", 0 );

        if( choice != 1 ) {
            update();
            return 0;
        }
    }

    operator delete[]( ZBuf, nothrow );
    ZBuf = 0;
    ZBufInMem = false;
    ZBufChanged = false;
    ZBufReady = false;
    calc_time = 0;
    old_time = 0;
    zbufxstart = 0;
    zbufystart = 0;
    MakeTitle();

    if( auto_resize ) {
        Image_AdjustWindow();
    }

    update();
    MainWin->redraw();
    return 1;
}

void MainWindow::ZBuffer_SaveAs() {
    Error[0] = 0;

    status_text.seekp( 0 );

    pathname zpnname( png_path );
    zpnname += act_file.filename();
    zpnname += ".zpn";

 	pathname filename;
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Save ZBuffer As");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.filter("ZBuffers\t*.{zpn,ZPN,Zpn}");
	fnfc.directory(zpnname.c_str());
	switch ( fnfc.show() ) {
		case -1: fl_alert("%s", fnfc.errmsg()); update(); return;
		case  1: update(); return; 	// CANCEL
		default: filename = fnfc.filename();
	}
   
    png_path = filename.path();

    filename.ext( ".zpn" );

    if( filename.exists() ) {
        int choice = fl_choice( "Selected file already exists. Overwrite?", "Abort", "Overwrite", 0 );

        if( choice != 1 ) {
            update();
            return;
        }
    }

    MainWin->cursor( FL_CURSOR_WAIT );

    if( SavePNG( Error, filename.c_str_conv(), 0, zbufystart, NULL, &frac, &view,
                 &realpal, colscheme, cutbuf, 1 ) != 0 ) {
        fl_alert( "%s", Error );
        MainWin->cursor( FL_CURSOR_DEFAULT );
        update();
        return;
    }

    MainWin->cursor( FL_CURSOR_DEFAULT );
    ZBufChanged = false;
    status_text.seekp( 0 );
    status_text << "ZBuffer '" << filename.c_str() << "' was written successfully.";
    update();
    return;
}

void MainWindow::menuItemEnabled( int i, bool b ) {
    if( !b ) {
        menubar->mode( i, menubar->mode( i ) | FL_MENU_INACTIVE );
    } else {
        menubar->mode( i, menubar->mode( i ) & ~FL_MENU_INACTIVE );
    }
}

void MainWindow::update() {
    assert( act_file.length() > 0 );
    // Image = 0
    menuItemEnabled( 1, !ImgInMem && !ZBufInMem && !InCalc );
    menuItemEnabled( 2, ImgInMem && !InCalc );
    menuItemEnabled( 3, ImgInMem && ImgChanged && !InCalc /*&& act_file[0]!=0*/ );
    menuItemEnabled( 4, ImgInMem && !InCalc );
    menuItemEnabled( 6, !InCalc );
    // Calculation = 8
    menuItemEnabled( 9, !ImgReady && ( ZBufReady || !ZBufInMem ) && !InCalc );
    menuItemEnabled( 10, !ZBufReady && !ImgInMem && !InCalc );
    menuItemEnabled( 11, InCalc );
    // Parameters = 13
    menuItemEnabled( 14, !InCalc );
    menuItemEnabled( 15, !ZBufInMem && !ImgInMem && !InCalc );
    menuItemEnabled( 16, !ImgInMem && !InCalc );
    menuItemEnabled( 17, !ImgInMem && !InCalc );
    menuItemEnabled( 18, !InCalc );
    // ZBuffer = 20
    menuItemEnabled( 21, !ImgInMem && !ZBufInMem && !InCalc );
    menuItemEnabled( 22, !ImgInMem && ZBufInMem && !InCalc );
    menuItemEnabled( 23, ZBufInMem && !InCalc );
    // Help = 25

    if( !InCalc ) {
        if( status_text.str().length() == 0 ) {
            status_text << "Image: ";

            if( ImgInMem && !ImgReady ) {
                status_text << "stopped.";
            } else if( ImgReady && ImgChanged ) {
                status_text << "ready.";
            } else if( ImgReady && !ImgChanged ) {
                status_text << "saved.";
            } else {
                status_text << "none.";
            }

            status_text << " ZBuffer: ";

            if( ZBufInMem && !ZBufReady ) {
                status_text << "stopped.";
            } else if( ZBufInMem && ZBufChanged ) {
                status_text << "ready.";
            } else if( ZBufInMem && !ZBufChanged ) {
                status_text << "saved.";
            } else {
                status_text << "none.";
            }
        }

        status_text << ends;
        delete[] _status_text_char;
        _status_text_char = new char[status_text.str().length() + 1];
        strcpy( _status_text_char, status_text.str().c_str() );
        status->label( _status_text_char );
        status->redraw();
    }
}

int MainWindow::FLTK_ReturnVideoInfo( struct vidinfo_struct* vidinfo ) {
    vidinfo->maxcol = -1;   /* FLTK simulates TrueColor through color cube */
    vidinfo->rdepth = 8;
    vidinfo->gdepth = 8;
    vidinfo->bdepth = 8;
    vidinfo->rgam = 0.6;
    vidinfo->ggam = 0.8;
    vidinfo->bgam = 0.6;
    return 0;
}

int MainWindow::FLTK_SetColors( struct disppal_struct* ) {
    // Color handling is done by FLTK. Implementation not neccessary.
    return 0;
}

int MainWindow::FLTK_Initialize( int x, int y, char* ) {
    ImageWid* p = reinterpret_cast<ImageWid*>( MainWinPtr->pix );
    assert( p != NULL );
    assert( p->w() == x && p->h() == y );

    if( MainWinPtr->auto_resize ) {
        MainWinPtr->Image_AdjustWindow();
    }

    return 0;
}

int MainWindow::FLTK_Done() {
    return 0;
}

int MainWindow::FLTK_update_bitmap( long, long, long, int, unsigned char*, int ) {
    return 0;
}

int MainWindow::FLTK_getline( unsigned char* line, int y, long xres, int whichbuf ) {
    switch( whichbuf ) {
        case 0:
            reinterpret_cast<ImageWid*>( MainWinPtr->pix )->get_line( y, line );
            break;

        case 1:
            memcpy( line, MainWinPtr->ZBuf + y * xres * 3L, xres * 3L );
            break;
    }

    return 0;
}

int MainWindow::FLTK_putline( long x1, long x2, long xres, int Y, unsigned char* Buf, int whichbuf ) {
    switch( whichbuf ) {
        case 0:
            reinterpret_cast<ImageWid*>( MainWinPtr->pix )
            ->set_area( x1, x2, Y, Buf + x1 * 3 );
            return 0;

        case 1:
            memcpy( MainWinPtr->ZBuf + 3L * ( Y * xres + x1 ), Buf + 3L * x1, ( x2 - x1 + 1 ) * 3L );

            for( long i = x1; i <= x2; i++ ) {
                long l = static_cast<long>( Buf[3 * i] )  << 16
                         | static_cast<long>( Buf[3 * i + 1] ) << 8
                         | static_cast<long>( Buf[3 * i + 2] );
                unsigned char R = 255 - static_cast<unsigned char>( 2.55
                                  * static_cast<float>( l ) / static_cast<float>( MainWinPtr->view.zres ) );
                reinterpret_cast<ImageWid*>( MainWinPtr->pix )
                ->set_pixel( i, Y, R, R, R );
            }

            return 0;
    }

    return 0;
}

int MainWindow::FLTK_check_event() {
    // For Windows, conventional method is faster.
    //#ifdef WIN32
    //	unsigned long newclock = GetTickCount();
    //	static unsigned long prevclock = 0;
    //	double elapsed = (newclock-prevclock)/1000.0;
    //#else   // unix and mac (?)
#ifndef WIN32
    static struct timeval prevclock = { 0L, 0L };
    struct timeval newclock;
    gettimeofday( &newclock, NULL );
    double elapsed = newclock.tv_sec - prevclock.tv_sec +
                     ( newclock.tv_usec - prevclock.tv_usec ) / 1000000.0;

    //#endif
    if( elapsed < 0.05 ) {
        return 0;
    }

    //#ifdef WIN32
    //	prevclock = newclock;
    //#else   // unix and mac (?)
    prevclock.tv_sec = newclock.tv_sec;
    prevclock.tv_usec = newclock.tv_usec;
#endif

    Fl::check();

    if( MainWinPtr->stop ) {
        MainWinPtr->stop = false;
        return( -128 );
    }

    return 0;
}

int MainWindow::FLTK_Change_Name( const char* s ) {
    MainWinPtr->status->label( s );
    MainWinPtr->status->redraw();
    return 0;
}

void MainWindow::FLTK_Debug( char* s ) {
    fl_message( "Quat - Debug: %s", s );
    return;
}

void MainWindow::FLTK_eol( int line ) {
    ostringstream s;
    s << ( int )line << " lines calculated." << ends;
    FLTK_Change_Name( s.str().c_str() );
}

void MainWindow::FLTK_eol_4( int ) {
    //	if ((line-1) % 4 == 0 || line==MainWinPtr->view.yres) FLTK_eol(line);
}

