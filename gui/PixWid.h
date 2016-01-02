#ifndef PIXWID_H
#define PIXWID_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <FL/Fl_Widget.H>
#include <FL/Fl_Pixmap.H>

#ifndef XPMCONST
#define XPMCONST
#endif

class PixWid : public Fl_Widget
{
public:
	PixWid(int x, int y, int w, int h, const char *label=0);
	virtual ~PixWid();
	void setPixmap(XPMCONST char *const* data);
	static const unsigned long TYPE;
	unsigned long type() { return _type; }
protected:
	void draw();
private:
	Fl_Pixmap *_pixmap;
	const unsigned long _type;
};

#endif
