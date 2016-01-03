#ifndef JULIAPREVIEW_H
#define JULIAPREVIEW_H

#include "structs.h"
#include "ImageWid.h"

class Fl_Button;

class JuliaPreview : public ImageWid {
    public:
        JuliaPreview( int x, int y, int w, int h, const char* label = 0 );
        ~JuliaPreview();
        void setbutton3d( Fl_Button* b ) {
            button3d = b;
        }
        static int FLTK_putline2( long x1, long x2, long, int y, unsigned char* Buf, int );
        static void FLTK_eol2( int );
        void CalcImage3D();
        void ownView( bool v ) {
            _ownView = v;
        }
        void stereo( bool v ) {
            _stereo = v;
        }
        void set( const frac_cpp& f, const view_cpp& v, const realpal_cpp& pal,
                  const colscheme_cpp& col, const cutbuf_cpp& cut ) {
            _f = f;
            _v = v;
            _pal = pal;
            _col = col;
            _cut = cut;
        }
    private:
        Fl_Button* button3d;
        bool _updated, _stereo, _ownView, _imagestereo;
        bool _pic_ownView;
        frac_cpp _f, _calc_f, _pic_f;
        view_cpp _v, _calc_v, _pic_v;
        realpal_cpp _pal, _calc_pal, _pic_pal;
        colscheme_cpp _col, _calc_col, _pic_col;
        cutbuf_cpp _cut, _calc_cut, _pic_cut;
};

#endif
