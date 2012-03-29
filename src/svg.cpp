#include "svg.h"


element& element::Tag(const string& t) {
	this->tag = t;
	return *this;
}

element& element::AddChild(const element& c) {
	this->children.push_back(c);
	return *this;
}

string element::toString(int indent) {
	string tabs(indent, '\t');
	string ret = tabs + "<" + tag;

	for (attrs_t::iterator it = attrs.begin(); it != attrs.end(); ++it) {
		ret += " " + it->first + "=\"" + it->second + "\"";
	}

	if (children.empty() && value.empty()) {
		ret += "/>";
	} else {
		ret += ">";
		ret += value;
		for (children_t::iterator it = children.begin(); it != children.end(); ++it) {
			element& tmp = *it;
			ret += "\n";
			ret += tmp.toString(indent+1);
		}
		if (!children.empty())	ret += "\n" + tabs;
		ret += "</" + tag + ">";
	}
	return ret;
}

string svg_pic::toString() {
	stringstream oss;

	oss << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	oss << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
	oss << root.toString();
	return oss.str();
}

void svg_pic::WriteToFile(const string& out_file) {
	ofstream out(out_file.c_str());
	out << this->toString();
	out.close();
}
