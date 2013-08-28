/*******************************************************************************
 * Copyright 2012 Mark Ruvald Pedersen <mark@radix63.dk>
 * Copyright 2012 Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 * Copyright 2012 Jaspur Hoejgaard <jaspurh@gmail.com>
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the copyright holder.
 ******************************************************************************/
 
#include "svg.h"

using namespace std;

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
