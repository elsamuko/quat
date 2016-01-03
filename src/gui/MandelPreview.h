#ifndef MANDELPREVIEW_H
#define MANDELPREVIEW_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Button.H>

class MandelPreview : public Fl_Widget {
    public:
        MandelPreview( int x, int y, int w, int h, const char* );
        ~MandelPreview();
        void CheckUpdate();
        void CalcImage();
        void SetInputs( Fl_Value_Input* cre, Fl_Value_Input* ci, Fl_Button* upd ) {
            _input_cre = cre;
            _input_ci = ci;
            _button_update = upd;
        }
        void SetCX( double cx ) {
            int dummy = 0;
            _cx = cx;
            C2Coo( cx, 0.0, _CursorX, dummy );
            redraw();
        }
        void SetCY( double cy ) {
            int dummy = 0;
            _cy = cy;
            C2Coo( 0.0, cy, dummy, _CursorY );
            redraw();
        }
        void MoveUp();
        void MoveDown();
        void MoveLeft();
        void MoveRight();
        void ZoomIn();
        void ZoomOut();
        int maxiter() const {
            return _Maxiter;
        }
        void maxiter( int v ) {
            _Maxiter = v;
            CheckUpdate();
            return;
        }
        int formula() const {
            return _Formula;
        }
        void formula( int v ) {
            _Formula = v;
            CheckUpdate();
            return;
        }
        double bailout() const {
            return _Bailout;
        }
        void bailout( double v ) {
            _Bailout = v;
            _BailoutSQR = v * v;
            CheckUpdate();
            return;
        }
        void p( int i, int j, double v ) {
            _p[i][j] = v;
            CheckUpdate();
        }
        double cj() const {
            return _cj;
        }
        void cj( double v ) {
            _cj = v;
            CheckUpdate();
            return;
        }
        double ck() const {
            return _ck;
        }
        void ck( double v ) {
            _ck = v;
            CheckUpdate();
            return;
        }
        bool updated( bool v ) {
            _updated = v;
            return true;
        }

    protected:
        void draw();
        int handle( int event );

    private:
        void C2Coo( double cx, double cy, int& x, int& y );
        void Coo2C( int x, int y, double& cx, double& cy );
        int CalcMPixel( int x, int y );
        void SetMyCursor( int x, int y );
        bool isValid();
        int _Maxiter, _Formula;
        double _Bailout, _BailoutSQR;
        double _cx, _cy, _cj, _ck;
        double _p[4][4];
        double _mandel_x, _mandel_y, _mandel_lx, _mandel_ly;
        int _len_x, _len_y;
        unsigned char* _pixmap;
        bool _updated;
        int _pic_Maxiter, _pic_Formula;
        double _pic_Bailout, _pic_cj, _pic_ck;
        double _pic_p[4][4];
        int _CursorX, _CursorY;
        int _oldCursorX, _oldCursorY;
        Fl_Value_Input* _input_cre, *_input_ci;
        Fl_Button* _button_update;
};

#endif
