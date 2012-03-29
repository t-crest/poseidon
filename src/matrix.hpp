#ifndef MATRIX_HPP
#define	MATRIX_HPP

#include <sstream>
#include <cassert>
#include <vector>
#include <utility>
#include "util.hpp"

template <class T>
class matrix {
	const uint num_rows;
	const uint num_cols;
	std::vector<T> data;

public:

	matrix(uint _row, uint _col)
	:	num_rows(_row), num_cols(_col) 
	{
		assert(this->num_rows > 0);
		assert(this->num_cols > 0);
		data.resize(this->num_rows * this->num_cols);
	}

	uint rows() const {
		return this->num_rows;
	}

	uint cols() const {
		return this->num_cols;
	}

	inline T& operator() (uint r, uint c) {
		assert(0 <= r && r <= this->num_rows);
		assert(0 <= c && c <= this->num_cols);
		return this->data.at(c * this->num_cols + r);
	}

	inline T operator() (uint r, uint c) const {
		assert(0 <= r && r <= this->num_rows);
		assert(0 <= c && c <= this->num_cols);
		return this->data.at(c * this->num_cols + r);
	}

	template <typename X, typename Y>
	inline T& operator() (std::pair<X, Y> arg) {
		return (*this)(arg.first, arg.second);
	}
};

template <typename A>
std::ostream& operator<<(std::ostream& stream, const matrix<A>& m) {

	stream << "[";
	for (uint r = 0; r < m.num_rows(); r++) {
		stream << "[";
		for (uint c = 0; c < m.num_cols(); c++) {
			const bool endc = (c == m.num_cols()-1);
			stream << m(r,c) << (endc ? "" : ", ");
		}
		const bool endr = (r == m.num_rows()-1);
		stream << "]" << (endr ? "" : ", ");
	}
	stream << "]";

	return stream;
}



#endif	/* MATRIX_HPP */

