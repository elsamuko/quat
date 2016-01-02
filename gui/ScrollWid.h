#ifndef SCROLLWID_H
#define SCROLLWID_H

#include <FL/Fl_Group.H>

class Fl_Scrollbar;
class Fl_Widget;

class ScrollWid : public Fl_Group
{
public:
	ScrollWid(int x,int y ,int w,int h,const char* label=0);
	virtual ~ScrollWid();
	void widget(Fl_Widget *wd); // Takes ownership of widget!!
	Fl_Widget* widget() const { return _widget; }
	void posX(int x, bool redraw=true);
	void posY(int y, bool redraw=true);
	static void hscroll_cb(Fl_Widget *, void *);
	static void vscroll_cb(Fl_Widget *, void *);
	static const unsigned long TYPE;
	unsigned long type() { return _type; }
protected:
	virtual void draw();
	virtual void resize(int x, int y, int w, int h);
//	int handle(int event) { return Fl_Group::handle(event); }
private:
	static const int _scrollSize;
	int _effw, _effh;
	Fl_Widget *_widget;
	Fl_Scrollbar *HBar, *VBar;
	const unsigned long _type;
};

#endif
