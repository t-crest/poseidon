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
 
#ifndef HIGHER_ORDER_H
#define	HIGHER_ORDER_H

#include <functional>

/**
 * Applies f to all elements in obj
 */
template <typename T, typename Function>
void for_each(T& obj, Function f) {
	for (auto it = obj.begin(); it != obj.end(); ++it)
		f(*it);
}

/**
 * Applies f to all elements in obj, in reverse order
 */
template <typename T, typename Function>
void for_each_reverse(T& obj, Function f) {
	for (auto rit = obj.rbegin(); rit != obj.rend(); ++rit)
		f(*rit);
}

/**
 * Returns only the elements for which the predicate holds true.
 */
template <typename T, typename Function>
T filter(T& obj, Function pred) {
	T ret;
	for (auto it = obj.begin(); it != obj.end(); ++it)
		if (pred(*it)) ret.insert(ret.end(), *it);
	return ret;
}

/**
 * @return True if predicate holds true for all elements in obj
 */
template <typename T, typename Function>
bool all(T& obj, Function pred) {
	for (auto it = obj.begin(); it != obj.end(); ++it)
		if (!(pred(*it))) return false;
	return true;
}

/**
 * Transform each element by applying f, returning this as a copy
 */
template <typename T, typename Function>
T transform(T& obj, Function transformer) {
	T ret;
	for (auto it = obj.begin(); it != obj.end(); ++it)
		ret.insert(ret.end(), transformer(*it));
	return ret;
}

/**
 * Reduce the entire collection to 1 value, via the combine function taking two elements
 */
template <typename T, typename Function>
auto reduce(T& obj, Function combiner) -> typename T::value_type {
	typedef typename T::value_type ret_t;
	ret_t res;
	for (auto it = obj.begin(); it != obj.end(); ++it)
		res = combiner(res, *it);
	return res;
}


#endif	/* HIGHER_ORDER_H */
