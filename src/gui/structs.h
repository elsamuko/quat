#ifndef STRUCTS_H
#define STRUCTS_H

// C++ Wrapper structures for libquat

#include <cstring>
#include "common.h"

struct frac_cpp : public frac_struct
{
	frac_cpp() { reset(); }
	void reset();
	frac_cpp operator = (frac_struct& v);
	void print();
};

struct view_cpp : public view_struct
{
	view_cpp() { reset(); }
	void reset();
	view_cpp operator = (view_struct& v);
	void print();
};

struct realpal_cpp : public realpal_struct
{
	realpal_cpp() { reset(); }
	void reset();
	realpal_cpp operator = (realpal_struct& v);
	void print();
};

struct colscheme_cpp
{
	colscheme_cpp() { reset(); }
	colscheme_cpp(const char v[]) { set(v); }
	void reset();
	void set(const char v[]) {
		if (strlen(v)>251) strncpy(_data, v, 251);
		else strcpy(_data, v);
	}
	operator char*() { return _data; }
	operator const char*() { return reinterpret_cast<const char*>(_data); }
private:
	char _data[251];
};

struct cutbuf_cpp
{
	cutbuf_cpp() { reset(); }       // num(0)
	cutbuf_cpp(const double v[]) { set(v); }
	void reset();
	void print();
	int getnum();
	void set(const double v[]) { memcpy(_data, v, 140*sizeof(double)); }
	void add();
	void del(int idx);
	operator double*() { return _data; }
private:
	double _data[140];
//	int num;
};

#endif
