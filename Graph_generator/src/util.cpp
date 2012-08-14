#include "util.hpp"

namespace util {
	
	/**	POSIX.1-2001 gives the following example of an implementation of
	 *	rand() and srand(), possibly useful when one needs the same sequence on
	 *	two different machines. It is also very portable. 
	 *	It is pratically uniform, but much faster than any of C++11's 
	 *	uniform random number generators.
	 */
	int rand(void) {
		using rand_global::next;
		next = next * 1103515245 + 12345;
		return ((unsigned) (next / 65536) % 32768); // Goes up to and including UTIL_RAND_MAX 
	}

	/**	Seed the RNG with specific value
	 */
	void srand(unsigned seed) {
		using rand_global::next;
		next = seed;
	}	

	/** Seed the RNG automatically with a really good (and expensive to get!) random value 
	 */
	void srand() {
		std::random_device seed;
		srand(seed());
	}	
}
