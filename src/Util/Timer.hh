#ifndef UTILTIMER_HH
#define UTILTIMER_HH

#include <string>

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
	struct _Minutes { };

	/* time facilities */
	extern const _Microseconds& Microseconds;
	extern const _Miliseconds& Miliseconds;
	extern const _Seconds& Seconds;
	extern const _Minutes& Minutes;

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

			template <class T> 
			Time(const std::string &_time, T m) {
				*this=(((unsigned int)(atoi(_time.c_str()))) * m);
			}

			friend class Timer;

			friend Time operator* (unsigned int constant, const _Minutes&);
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
			bool operator != (Time time) const;
			bool operator >= (Time time) const;
			bool operator <= (Time time) const;
			bool operator > (Time time) const;
			bool operator < (Time time) const;

			/* returns time */
			double getSeconds() const;
			double getMiliseconds() const;
			double getMicroseconds() const;
	};

	Time operator* (unsigned int constant, const _Minutes&);
	Time operator* (unsigned int constant, const _Seconds&);
	Time operator* (long long unsigned int constant, const _Miliseconds&);
	Time operator* (long long unsigned int constant, const _Microseconds&);

	/*Time operator* (double constant, const _Seconds&);
	Time operator* (double constant, const _Miliseconds&);
	Time operator* (double constant, const _Microseconds&);*/

}

#endif
