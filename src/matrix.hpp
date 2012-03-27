#ifndef MATRIX_HPP
#define	MATRIX_HPP

#include <sstream>
#include <cassert>
#include <vector>
#include <utility>
#include "util.hpp"

template <class T>
class matrix {
	const uint row;
	const uint col;
	std::vector<T> data;

public:

	matrix(uint _row, uint _col)
	:	row(_row), col(_col) 
	{
		assert(this->row > 0);
		assert(this->col > 0);
		data.resize(this->row * this->col);
	}

	uint rows() const {
		return this->row;
	}

	uint cols() const {
		return this->col;
	}

	inline T& operator() (uint r, uint c) {
		assert(0 <= r && r <= this->row);
		assert(0 <= c && c <= this->col);
		return this->data.at(c * this->row + r);
	}

	inline T operator() (uint r, uint c) const {
		assert(0 <= r && r <= this->row);
		assert(0 <= c && c <= this->col);
		return this->data.at(c * this->row + r);
	}

	template <typename X, typename Y>
	inline T& operator() (std::pair<X, Y> arg) {
		return (*this)(arg.first, arg.second);
	}
};

template <typename A>
std::ostream& operator<<(std::ostream& stream, const matrix<A>& m) {

	stream << "[";
	for (uint r = 0; r < m.rows(); r++) {
		stream << "[";
		for (uint c = 0; c < m.cols(); c++) {
			const bool endc = (c == m.cols()-1);
			stream << m(r,c) << (endc ? "" : ", ");
		}
		const bool endr = (r == m.rows()-1);
		stream << "]" << (endr ? "" : ", ");
	}
	stream << "]";

	return stream;
}



#endif	/* MATRIX_HPP */

