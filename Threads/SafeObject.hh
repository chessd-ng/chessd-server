#ifndef SAFEOBJECT_HH
#define SAFEOBJECT_HH

#include "rwlock.hh"

template <class T>
class SafeObject {
	private:
		T object;
		RwLock rwlock;
	public:

		SafeObject() { }
		SafeObject(const T& object) : object(object) { }
		~SafeObject() { }

		T& getWriteLock() {
			this->rwlock.wrlock();
			return object;
		}
		const T& getReadLock() {
			this->rwlock.rdlock();
			return object;
		}
		void releaseLock() {
			this->rwlock.unlock();
		}
};

template <class T> class ReadLock {
	public:
		ReadLock(SafeObject<T>& safe_object) :
			safe_object(safe_object),
			object(safe_object.getReadLock())
		{ }

		~ReadLock() {
			safe_object.releaseLock();
		}
		const T* operator->() const {
			return &this->object;
		}
	private:
		SafeObject<T>& safe_object;
		const T& object;
};

template <class T> class WriteLock {
	public:
		WriteLock(SafeObject<T>& safe_object) :
			safe_object(safe_object),
			object(safe_object.getWriteLock())
		{ }

		WriteLock() {
			safe_object.releaseLock();
		}
		T* operator->() {
			return &this->object;
		}
		const T* operator->() const {
			return &this->object;
		}
	private:
		SafeObject<T>& safe_object;
		T& object;
};

#endif
