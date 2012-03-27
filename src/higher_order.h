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
