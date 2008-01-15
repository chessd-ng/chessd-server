#ifndef CASTFUNCTOR_HH
#define CASTFUNCTOR_HH

#include <functional>

template <class T1, class T2>
struct CastFunctor : public std::unary_function<T2&, T1&> {
	T1& operator()(T2& item) const {
		return static_cast<T1&>(item);
	}
	const T1& operator()(const T2& item) const {
		return static_cast<const T1&>(item);
	}
};



#endif
