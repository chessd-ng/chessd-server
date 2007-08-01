#ifndef SAFEOBJECT_HH
#define SAFEOBJECT_HH
#include <pthread.h>

template <class T>
class SafeObject {
	private:
		T* object;
		Mutex mutex;
	public:
		SafeObject(T* object = new T) : object(object) { }
		~SafeObject() {
			delete object;
		}
		T* getLock() {
			mutex.lock();
			return object;
		}
		void releaseLock() {
			mutex.unlock();
		}

};

#endif
