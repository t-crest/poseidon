#ifndef SVG_H
#define	SVG_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include "lex_cast.h"


class element {
public:
	typedef std::map<std::string/*name*/, std::string/*value*/> attrs_t;
	typedef std::list<element> children_t;

	std::string tag;
	attrs_t attrs;
	children_t children;	// nestede elements
	std::string value;			// fx <title>Prison Break</title>

	template <typename T>
	element& Set(std::string attr_name, const T& val) {
		attrs[attr_name] = ::lex_cast<std::string>(val);
		return *this;
	}

	template <typename T>
	element& SetValue(const T& val) {
		this->value = ::lex_cast<std::string>(val);
		return *this;
	}

	element& Tag(const std::string& t);
	element& AddChild(const element& c);
	std::string toString(int indent = 0);
};


class svg_pic {
public:
	element root;

	std::string toString();
	void WriteToFile(const std::string& out_file);
};

#endif	/* SVG_H */
