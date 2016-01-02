#ifndef COLORCLIPBOARD_H
#define COLORCLIPBOARD_H

#include <FL/Fl_Widget.H>

class ColorClipboard : public Fl_Widget
{
public:
	ColorClipboard(int x, int y, int w, int h, const char *label)
		: Fl_Widget(x, y, w, h, label), _r(0.0), _g(0.0), _b(0.0) {}
	void Set(double r, double g, double b) { _r = r; _g = g; _b = b; }
	double GetRed() const { return _r; }
	double GetGreen() const { return _g; }
	double GetBlue() const { return _b; }
protected:
	virtual void draw();
	virtual int handle(int);
private:
	double _r, _g, _b;
};

#endif
