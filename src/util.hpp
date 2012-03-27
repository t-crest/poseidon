#ifndef _UTIL_HPP
#define	_UTIL_HPP

#include <algorithm>
#include <string>
#include <cmath>
#include <set>

#define ensure(pred, x)		{if (!(pred))	{std::cerr << "Error:  \t " << x << " [" <<  __FILE__ << ":" << __LINE__ << "]" << std::endl; abort();}}
#define warn_if(pred, x)	{if ((pred))	{std::cerr << "Warning:\t " << x << " [" <<  __FILE__ << ":" << __LINE__ << "]" << std::endl;}}
#define debugf(x)			{std::cout << __FILE__ << ":" << __LINE__ << ":\t " #x " = '" << (x) << "'" << std::endl;}

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

}

#endif	/* _UTIL_HPP */

