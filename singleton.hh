#ifndef SINGLETON_HH
#define SINGLETON_HH

template <class T> struct Singleton {
	static T* singleton() {
		static T st;
		return &st;
	}
};

#endif
