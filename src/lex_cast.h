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
 
#ifndef LEX_CAST_H
#define LEX_CAST_H

#include <sstream>
#include <typeinfo>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <array>
#include <boost/optional/optional.hpp>

namespace lex_cast_helpers {
	void consume(std::istream& stream, const std::set<char>& items);
}

template <typename A, typename B>
std::istream& operator>>(std::istream& stream, std::pair<A,B>& p)
{
	using lex_cast_helpers::consume;
	
	consume(stream, std::set<char>({' ', '('}));
	stream >> p.first;
	stream.ignore(1,',');
	stream >> p.second;	
	consume(stream, std::set<char>({' ', ')'}));
	
	return stream; 
}

template <typename A, typename B>
std::ostream& operator<<(std::ostream& stream, const std::pair<A,B>& p)
{
	stream << "(" << p.first << "," << p.second << ")";
	return stream; 
}

template <typename A>
std::ostream& operator<<(std::ostream& stream, const std::set<A>& s)
{
	stream << "{";
	for (auto it = s.begin(); it != s.end(); ++it) {
		stream << ((it == s.begin()) ? "" : ",");
		stream << *it;
	}
	stream << "}";
	
	return stream; 
}

template <typename A>
std::ostream& operator<<(std::ostream& stream, const std::vector<A>& s)
{
	stream << "<";
	for (auto it = s.begin(); it != s.end(); ++it) {
		stream << ((it == s.begin()) ? "" : ",");
		stream << *it;
	}
	stream << ">";
	
	return stream; 
}

template <typename A, size_t L>
std::ostream& operator<<(std::ostream& stream, const std::array<A, L>& s)
{
	stream << "<";
	for (auto it = s.begin(); it != s.end(); ++it) {
		stream << ((it == s.begin()) ? "" : ",");
		stream << *it;
	}
	stream << ">";
	
	return stream; 
}

template <typename A>
std::ostream& operator<<(std::ostream& stream, const boost::optional<A>& s)
{
	if (s)	stream << *s;
	else	stream << " ";
	return stream; 
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& stream, const std::map<K,V>& m)
{
	stream << "[";
	for (auto it = m.begin(); it != m.end(); ++it) {
		stream << ((it == m.begin()) ? "" : ",");
		stream << it->first;
		stream << "->";
		stream << it->second;
	}
	stream << "]";
	
	return stream; 
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& stream, const std::unordered_map<K,V>& m)
{
	stream << "[";
	for (auto it = m.begin(); it != m.end(); ++it) {
		stream << ((it == m.begin()) ? "" : ",");
		stream << it->first;
		stream << "->";
		stream << it->second;
	}
	stream << "]";
	
	return stream; 
}


/**
 * Like boost::lexical_cast but self contained.
 *
 * Credit goes to sbi at stack overflow:
 *	http://stackoverflow.com/questions/4058052/lex-cast-make-formatted-streams-unformatted/4058089#4058089
 */
namespace ugly_details
{
	template<typename T, typename S>
	struct struct_wrapper
	{
		static T my_lexical_cast(const S& s)
		{
			std::stringstream ss;
			T t;
			if (!(ss << s)) throw std::bad_cast();
			if (!(ss >> t)) throw std::bad_cast();
			return t;
		}
	};

	template<typename S>
	struct struct_wrapper<std::string, S>
	{
		static std::string my_lexical_cast(const S& s)
		{
			std::ostringstream oss;
			if (!(oss << s)) throw std::bad_cast();
			return oss.str();
		}
	};

	template<typename T>
	struct struct_wrapper<T, std::string>
	{
		static T my_lexical_cast(const std::string& s)
		{
			std::stringstream ss(s);
			T t;
			if (!(ss >> t)) throw std::bad_cast();
			return t;
		}
	};

	template<typename T>
	struct struct_wrapper<T, T>
	{
		static const T& my_lexical_cast(const T& s)
		{
			return s;
		}
	};

	template<>
	struct struct_wrapper<std::string, std::string>
	{
		static const std::string& my_lexical_cast(const std::string& s)
		{
			return s;
		}
	};
}

template<typename T, typename S>
inline T lex_cast(const S& s)
{
	return ugly_details::struct_wrapper<T,S>::my_lexical_cast(s);
}

template<typename S>
inline std::string stringify(const S& s)
{
	return ::lex_cast<std::string>(s);
}

#endif	/* LEX_CAST_H */
