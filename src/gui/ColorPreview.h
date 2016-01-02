#ifndef COLORPREVIEW_H
#define COLORPREVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>

#ifndef NO_NAMESPACE
using namespace std;
#endif

#include <FL/Fl_Valuator.H>
#include <FL/Fl_Button.H>

#include "structs.h"

class Fl_Value_Input;
class Fl_Output;
class Fl_Button;

class ColorPreview : public Fl_Valuator
{
static const int CP_LEFT = 1;
static const int CP_RIGHT = 2;
public:
	ColorPreview(int x, int y, int w, int h, const char *label);
	~ColorPreview();
	void SetInputs(Fl_Button *add, Fl_Button *del, Fl_Value_Input *weight,
		Fl_Value_Input *red1, Fl_Value_Input *green1, Fl_Value_Input *blue1,
		Fl_Value_Input *red2, Fl_Value_Input *green2, Fl_Value_Input *blue2,
		Fl_Output *out)
	{
		_button_add = add; _button_del = del; _input_weight = weight;
		_input_red1 = red1; _input_green1 = green1; _input_blue1 = blue1;
		_input_red2 = red2; _input_green2 = green2; _input_blue2 = blue2;
		_output_range = out;
		UpdateInputs();
	}
	void SetPal(const realpal_cpp &pal) {
		_pal = pal; _idx = 0; Update(); UpdateInputs(); 
		if (_pal.colnum>1) _button_del->activate();
	}
	void GetPal(struct realpal_struct &pal) const { pal = _pal; }
	int current() const { return _idx; }
	void add();
	void del();
	void weight(int pos, double v) { if (pos>_idx) return; _pal.cols[pos].weight = v; Update(); }
	void red1(int pos, double v) { if (pos>_idx) return; _pal.cols[pos].col1[0] = v; Update(); }
	void green1(int pos, double v) { if (pos>_idx) return; _pal.cols[pos].col1[1] = v; Update(); }
	void blue1(int pos, double v) { if (pos>_idx) return; _pal.cols[pos].col1[2] = v; Update(); }
	void red2(int pos, double v) { if (pos>_idx) return; _pal.cols[pos].col2[0] = v; Update(); }
	void green2(int pos, double v) { if (pos>_idx) return; _pal.cols[pos].col2[1] = v; Update(); }
	void blue2(int pos, double v) { if (pos>_idx) return; _pal.cols[pos].col2[2] = v; Update(); }
protected:
	virtual void draw();
	virtual int handle(int event);
private:
	void Update();
	void UpdateInputs();
	bool _defined;
	int _state;
	int _idx;
	unsigned char *_pixmap;
	vector<int> _pos;
	realpal_cpp _pal;
	Fl_Button *_button_add, *_button_del;
	Fl_Value_Input *_input_weight, *_input_red1, *_input_green1, *_input_blue1;
	Fl_Value_Input *_input_red2, *_input_green2, *_input_blue2;
	Fl_Output *_output_range;
};

#endif
