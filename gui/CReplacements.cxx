#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sstream>
#include <iostream>
#include <fstream>
#include <cctype>	// toupper

#ifndef NO_NAMESPACE
using namespace std;
#endif

#include "CReplacements.h"

pathname::pathname(const char* c) : string(_convert_slash(c)) {}
pathname::pathname(const string& s) : string(_convert_slash(s.c_str())) {}

string pathname::_convert_slash(const char* c) const
{
	string s(c);
#ifdef WIN32
	for (unsigned int i=0; i<s.length(); ++i)
		if (s[i] == '\\') s[i] = '/';
#endif
	return s;
}

void pathname::uppercase() {
	for (size_type i=0; i<length(); ++i)
	at(i) = ::toupper(at(i));
}

string pathname::path() const {
	return substr(0, rfind("/")+1);
}

string pathname::file() const {
	size_type last_slash = rfind("/");
	if (last_slash>length()) return *this;
	return substr(last_slash+1);
}

void pathname::file(const char* c) {
	*this = path();
	*this += c;
}

string pathname::filename() const {
	size_type last_slash = rfind("/");
	size_type last_dot = rfind(".");
	if (last_slash < length() && last_dot < last_slash)
		last_dot = (size_type)(-1);
	return substr(last_slash+1, last_dot-last_slash-1);
}

string pathname::ext() const {
	size_type last_slash = rfind("/");
	size_type last_dot = rfind(".");
	if (last_slash < length() && last_dot < last_slash)
		return string("");
	if (last_dot >= length()) return string("");
	return substr(last_dot);
}

void pathname::ext(const char* c) {
	*this = path() + filename();
	if (c[0] != '.') *this += '.';
	*this += c;
}

bool pathname::exists() const {
	ifstream f;
	f.open(c_str(), ios::binary);
	if (!f) return false;
	f.close();
	return true;
}

void pathname::seperate(string& _path, string& _filename, string& _ext) const {
	_path = path(); _filename = filename(); _ext = ext();
}

string pathname::next_name(int digits) const {
	string _path = path();
	string _filename = filename();
	string _ext = ext();
	if (_filename.length() == 0) _filename = "Noname";
	size_type nr = _filename.find_last_not_of("0123456789");
	string base(_filename.substr(0, nr+1));
	size_type stellen = _filename.length() - nr - 1;
	int number = 0;
	if (stellen > 0) {
		stringstream ss;
		ss << _filename.substr(nr+1, stellen);
		ss >> number;
	}
	if (stellen == 0) {
		stellen = digits;
		number = 0;
		if (base[base.length()-1] != '-') base += '-';
	}
	++number;
	stringstream tmp;
	tmp << number;
	if (tmp.str().length() > stellen) ++stellen;
	stringstream s;
	s << _path << base;
	s.fill('0');
	s.width(stellen);
	s << number << _ext;
	return s.str();
}

void pathname::auto_name(int digits) {
	while (exists()) *this = next_name(digits);
}
