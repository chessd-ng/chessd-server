#ifndef MOVE_HH
#define MOVE_HH

template <class T>
class Moved {
	public:
		explicit Moved(T& ref) : _ref(ref) { }

		~Moved() { }

		T& operator*() { return _ref; }
		const T& operator*() const { return _ref; }

		T* operator->() { return &_ref; }
		const T* operator->() const { return &_ref; }


	private:
		T& _ref;
};

template <class T, class U>
Moved<T> move(Moved<U> ref) {
	return Moved<T>(*ref);
}

template <class T>
Moved<T> move(T& ref) {
	return Moved<T>(ref);
}


#endif
