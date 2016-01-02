#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <FL/Fl_Window.H>

class ChildWindow : public Fl_Window
{
public:
	ChildWindow(int x, int y, const char *label=0) : Fl_Window(0, 0, x, y, label) {}
};

#endif

