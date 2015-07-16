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


	// Assumes 0 <= max <= RAND_MAX
	// Returns in the half-open interval [0, max]
	long random_at_most(long max) {
	  unsigned long
	    // max <= RAND_MAX < ULONG_MAX, so this is okay.
	    num_bins = (unsigned long) max + 1,
	    num_rand = (unsigned long) RAND_MAX + 1,
	    bin_size = num_rand / num_bins,
	    defect   = num_rand % num_bins;

	  long x;
	  do {
	   x = random();
	  }
	  // This is carefully written not to overflow
	  while (num_rand - defect <= (unsigned long)x);

	  // Truncated division is intentional
	  return x/bin_size;
	}
	
}
