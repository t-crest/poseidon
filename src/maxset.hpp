#ifndef MAXSET_H
#define	MAXSET_H

#include <set>


template < 
	typename T, 
	class minor_comparator = std::less<T>,
	class major_comparator = std::less<T> 
>
class maxset 
{
public:
	typedef T value_type;
	typedef std::set<T, major_comparator> set_t;
	
	template<typename IT>
	bool insert(IT begin, IT end) {
		for (IT it = begin; it != end; ++it) {
			this->insert(*it);
		}
		return true;
	}
	
	bool insert(const T& x) 
	{
		auto y = this->store.find(x);
		const bool same_major = (y != this->store.end());

		if (!same_major) {
			this->store.insert(x);
			return true;
		}
		
		minor_comparator better;
		if (better(x, *y)) { // if x is better, replace *y
			this->store.erase(y);
			this->store.insert(x);
			return true;
		}
		
		return false;
	}
	
	const set_t& get_set() const {
		return this->store;
	}
	
private:
	set_t store;
};


#endif	/* MAXSET_H */

