#ifndef UTILTIMER_HH
#define UTILTIMER_HH

namespace Util {

	class Time;

	class Timer {
		public:

			/* returns current time */
			static Time Now();

			/* block the given time */
			void Block(Time tempo);
	};

	/* time units */
	struct _Microseconds { };
	struct _Miliseconds { };
	struct _Seconds { };

	/* time facilities */
	extern const _Microseconds& Microseconds;
	extern const _Miliseconds& Miliseconds;
	extern const _Seconds& Seconds;

	class Time {
		private:

			int _seconds;
			int _nanoseconds;

			Time(int _seconds, int _nanoseconds) :
				_seconds(_seconds),
				_nanoseconds(_nanoseconds) { }

		public:

			Time() : _seconds(0), _nanoseconds(0) { }
			Time(const Time& time) :
				_seconds(time._seconds),
				_nanoseconds(time._nanoseconds) { }

			friend class Timer;

			friend Time operator* (unsigned int constant, const _Seconds&);
			friend Time operator* (long long unsigned constant, const _Miliseconds&);
			friend Time operator* (long long unsigned constant, const _Microseconds&);

			friend Time operator* (double constant, const _Seconds&);
			friend Time operator* (double constant, const _Miliseconds&);
			friend Time operator* (double constant, const _Microseconds&);

			const Time& operator+=(Time time);
			const Time& operator-=(Time time);

			Time operator+(Time time) const;
			Time operator-(Time time) const;

			const Time& operator=(Time time);

			bool operator == (Time time) const;
			bool operator >= (Time time) const;
			bool operator <= (Time time) const;
			bool operator > (Time time) const;
			bool operator < (Time time) const;

			/* returns time */
			double getSeconds() const;
			double getMiliseconds() const;
			double getMicroseconds() const;
	};

	Time operator* (unsigned int constante, const _Seconds&);
	Time operator* (long long unsigned int constante, const _Miliseconds&);
	Time operator* (long long unsigned int constante, const _Microseconds&);

	Time operator* (double constant, const _Seconds&);
	Time operator* (double constant, const _Miliseconds&);
	Time operator* (double constant, const _Microseconds&);

}

#endif
