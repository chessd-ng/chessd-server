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

#ifndef UTILTIMER_HH
#define UTILTIMER_HH

#include <string>
#include <time.h>

namespace Util {

	class Time;

	class Timer {
		public:

			/* returns current time */
			static Time now();

			/* block the given time */
			void block(Time tempo);
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

            /* XXX This is evil!!! */
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

            timespec getTimespec() const {
                timespec ret;
                ret.tv_sec = this->_seconds;
                ret.tv_nsec = this->_nanoseconds;
                return ret;
            }
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
