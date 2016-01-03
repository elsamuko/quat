#ifndef IMAGEWID_H
#define IMAGEWID_H

#include <cassert>
#include <cstring>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>

class ImageWid : public Fl_Widget {
    public:
        ImageWid( int x, int y, int w, int h, const char* label = 0 );
        virtual ~ImageWid();
        bool newImage( int w, int h );
        bool valid() const {
            return _data != 0;
        }
        void white();
        void fill( Fl_Color& col );
        void set_pixel( int x, int y, unsigned char r, unsigned char g, unsigned char b );
        void set_area( int x1, int x2, int y, unsigned char* d );
        void set_line( int n, unsigned char* d );
        void get_line( int n, unsigned char* d ) const;
        //	unsigned char *line(int n) const;
        unsigned long type() const {
            return _type;
        }
        static const unsigned long TYPE;
    protected:
        void size( int, int ) {}
        virtual void draw();
        virtual int handle( int );
        //	void _damage(int x, int y, int w, int h);
    private:
        unsigned char* _data;
        //	int _damage_x, _damage_y, _damage_w, _damage_h;
        //	bool _damaged;
        const unsigned long _type;
};

#endif
