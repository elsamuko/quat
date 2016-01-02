#include <FL/Fl_Window.H>

class dragWindow : public Fl_Window {
public:
	dragWindow(int w, int h, const char* label=0)
		: Fl_Window(w, h, label), _rel_x(0), 
		_rel_y(0) {}
	virtual void show() {
		Fl_Window::show();
		cursor(FL_CURSOR_MOVE);
	}	
protected:
	virtual int handle(int event) {
		int diffx, diffy;
		switch (event) {
			case FL_PUSH:
			_rel_x = Fl::event_x();
			_rel_y = Fl::event_y();
			return 1;
			case FL_RELEASE:
			return 1;
			case FL_DRAG:
			diffx = Fl::event_x() - _rel_x;
			diffy = Fl::event_y() - _rel_y;
			Fl_Window::resize(x()+diffx, y()+diffy, w(), h());
			return 1;
			default: return Fl_Window::handle(event);
		}
//		return Fl_Window::handle(event);	// unreachable
	}
	int _rel_x, _rel_y;
};
