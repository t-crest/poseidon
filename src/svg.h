#ifndef SVG_H
#define	SVG_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include "lex_cast.h"
using namespace std;


class element {
public:
	typedef map<string/*name*/, string/*value*/> attrs_t;
	typedef list<element> children_t;

	string tag;
	attrs_t attrs;
	children_t children;	// nestede elements
	string value;			// fx <title>Prison Break</title>

	template <typename T>
	element& Set(string attr_name, const T& val) {
		attrs[attr_name] = ::lex_cast<string>(val);
		return *this;
	}

	template <typename T>
	element& SetValue(const T& val) {
		this->value = ::lex_cast<string>(val);
		return *this;
	}

	element& Tag(const string& t);
	element& AddChild(const element& c);
	string toString(int indent = 0);
};


class svg_pic {
public:
	element root;

	string toString();
	void WriteToFile(const string& out_file);
};

#endif	/* SVG_H */
