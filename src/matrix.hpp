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
        
        template <typename X, typename Y>
        inline T operator() (std::pair<X, Y> arg) const {
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

