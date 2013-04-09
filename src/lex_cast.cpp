#include "lex_cast.h"

namespace lex_cast_helpers {

	void consume(std::istream& stream, const std::set<char>& items) {
		while (items.count(char(stream.peek())))
			stream.get();
	}

}
