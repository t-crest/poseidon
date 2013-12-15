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

#ifndef _UTIL_HPP
#define	_UTIL_HPP

#include <algorithm>
#include <string>
#include <cmath>
#include <set>
#include <random>

#define ensure(pred, x)		{if (!(pred))	{std::cerr << "Error:  \t " << x << " [" <<  __FILE__ << ":" << __LINE__ << "]" << std::endl; abort();}}
#define warn_if(pred, x)	{if ((pred))	{std::cerr << "Warning:\t " << x << " [" <<  __FILE__ << ":" << __LINE__ << "]" << std::endl;}}
#ifdef DEBUG_VERBOSE
#define debugf(x)			{std::cout << __FILE__ << ":" << __LINE__ << ":\t " #x " = '" << (x) << "'" << std::endl;}
#define debugs(x)			{std::cout << __FILE__ << ":" << __LINE__ << ":\n " << x << "'" << std::endl;}
#else
#define debugf(x)
#define debugs(x)
#endif


typedef unsigned int uint;

namespace util {

	template <typename C, typename E>
	inline bool contains(const C& collection, const E& elem) {
		return (collection.find(elem) != collection.end());
	}

	template <typename T>
	inline T max(const T& a, const T& b) {
		return (a > b) ? a : b;
	}

	template <typename T>
	inline T min(const T& a, const T& b) {
		return (a < b) ? a : b;
	}

	#define UTIL_RAND_MAX 32767 /*DO NOT CHANGE*/

	namespace rand_global {
		static unsigned long next = 1;
	}
	int rand(void);
	void srand(unsigned seed);
	void srand();
}

#endif	/* _UTIL_HPP */

