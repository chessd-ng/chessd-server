#ifndef UTILSINGLETON_HH
#define UTILSINGLETON_HH

namespace Util {

	template <class T> struct Singleton {
		static T* singleton() {
			static T st;
			return &st;
		}
	};

}

#endif
