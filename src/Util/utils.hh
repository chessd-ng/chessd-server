/*
 *   Copyright (c) 2007-2008 C3SL.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

#ifndef UTILS_HH
#define UTILS_HH

#include <iterator>
#include <functional>
#include <cctype>
#include <string>
#include <sstream>

namespace Util {

#define foreach(it, cont) \
    for(bool __continue__ = true; __continue__;) \
        for(auto& __cont__ = (cont);__continue__; __continue__ = false) \
            for(auto it = __cont__.begin(); it != __cont__.end(); ++it)

#define foreach_it(it, begin, end) \
    for(auto it = begin, __end__ = end; it != __end__; ++ it)

#define foreach_pair(it, pair) for(auto it = pair.first; it != pair.second; ++it)

#define if_find(iterator, key, container) \
    for(bool __continue__ = true; __continue__;) \
    for(auto& __cont__ = (container);__continue__;) \
    for(auto iterator = (__cont__).find(key); \
            __continue__; \
            __continue__ = false) \
    if(iterator != (__cont__).end())



    template <class ITERATOR> ITERATOR next(ITERATOR it) {
        return ++it;
    }

	template <class T, class OBJ> class _CompareMember {
		public:
			_CompareMember(const T OBJ::*member_ptr, const T& value) :
				member_ptr(member_ptr), value(value) { }
			bool operator()(const OBJ& obj) const {
				return obj.*(this->member_ptr) == this->value;
			}
		private:
			const T OBJ::* member_ptr;
			T value;
	};

	template <class T, class OBJ> _CompareMember<T, OBJ> CompareMember
		(const T OBJ::*member_ptr, const T& value) {
			return _CompareMember<T, OBJ>( member_ptr, value);
		}

	/* Check if a given string is a number */
	inline bool isNumber(const std::string& number) {
		for(int i = 0; i < int(number.size()); ++i)
			if(not isdigit(number[i]))
				return false;
		return true;
	}

	/* Check if a ginven string is a space */
	inline bool isSpace(const std::string& number) {
		for(int i = 0; i < int(number.size()); ++i)
			if(not isspace(number[i]))
				return false;
		return true;
	}

	/* Check wether a map has a key */
	template <class Map, class Key> bool has_key(const Map& m, const Key& k) {
		return m.find(k) != m.end();
	}

}

#endif
