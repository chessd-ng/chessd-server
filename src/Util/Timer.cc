/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
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

#include "Timer.hh"

#include <ctime>
#include <cmath>

namespace Util {

	/* time facilities */
	const _Microseconds& Microseconds;
	const _Miliseconds& Miliseconds;
	const _Seconds& Seconds;
	const _Minutes& Minutes;

	static const int one_second = 1000000000;


	Time Timer::Now() {
		timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return Time(ts.tv_sec, ts.tv_nsec);
	}
	
	const Time& Time::operator += (Time time) {
		this->_seconds += time._seconds;
		this->_nanoseconds += time._nanoseconds;
		if(this->_nanoseconds >= one_second) {
			this->_nanoseconds -= one_second;
			this->_seconds ++;
		}
		return *this;
	}

	const Time& Time::operator -= (Time time) {
		this->_seconds -= time._seconds;
		this->_nanoseconds -= time._nanoseconds;
		if(this->_nanoseconds < 0) {
			this->_nanoseconds += one_second;
			this->_seconds --;
		}
		return *this;
	}


	Time Time::operator+(Time time) const {
		Time tmp(*this);
		tmp += time;
		return tmp;
	}

	Time Time::operator-(Time time) const {
		Time tmp = *this;
		tmp -= time;
		return tmp;
	}


	const Time& Time::operator=(Time time) {
		this->_seconds = time._seconds;
		this->_nanoseconds = time._nanoseconds;
		return *this;
	}


	bool Time::operator==(Time time) const {
		return this->_seconds == time._seconds and
			this->_nanoseconds == time._nanoseconds;
	}

	bool Time::operator!=(Time time) const {
		return this->_seconds != time._seconds or
			this->_nanoseconds != time._nanoseconds;
	}

	bool  Time::operator>=(Time time) const {
		return this->_seconds > time._seconds or
			(this->_seconds == time._seconds and
			 this->_nanoseconds >= time._nanoseconds);
	}

	bool  Time::operator<=(Time time) const {
		return this->_seconds < time._seconds or
			(this->_seconds == time._seconds and
			 this->_nanoseconds <= time._nanoseconds);
	}

	bool  Time::operator>(Time time) const {
		return this->_seconds > time._seconds or
			(this->_seconds == time._seconds and
			 this->_nanoseconds > time._nanoseconds);
	}

	bool  Time::operator<(Time time) const {
		return this->_seconds < time._seconds or
			(this->_seconds == time._seconds and
			 this->_nanoseconds < time._nanoseconds);
	}


	double Time::getSeconds() const {
		return (this->_seconds + this->_nanoseconds / double(one_second));
	}

	double Time::getMiliseconds() const {
		double factor = 1000.0;
		return (this->_seconds + this->_nanoseconds / double(one_second)) * 1000.0;
	}

	double Time::getMicroseconds() const {
		double factor = 1000000.0;
		return (this->_seconds + this->_nanoseconds / double(one_second)) * 1000000.0;
	}

	Time operator* (unsigned int constant, const _Minutes&) {
		Time tmp;
		tmp._seconds = constant*60u;
		tmp._nanoseconds = 0;
		return tmp;
	}

	Time operator* (unsigned int constant, const _Seconds&) {
		Time tmp;
		tmp._seconds = constant;
		tmp._nanoseconds = 0;
		return tmp;
	}

	Time operator* (long long unsigned constant, const _Miliseconds&) {
		Time tmp;
		tmp._seconds = (constant / 1000ll);
		tmp._nanoseconds = (constant % 1000ll) * 1000000;
		return tmp;
	}

	Time operator* (unsigned long long constant, const _Microseconds&) {
		Time tmp;
		tmp._seconds = (constant / 1000000ll);
		tmp._nanoseconds = (constant % 1000000ll) * 1000;
		return tmp;
	}

	Time operator* (double constant, const _Seconds&) {
		Time tmp;
		tmp._seconds = int(trunc(constant));
		tmp._nanoseconds = int((constant - trunc(constant)) * 1000000000.0);
		return tmp;
	}

	Time operator* (double constant, const _Miliseconds&) {
		return constant / 1000.0 * Seconds;;
	}

	Time operator* (double constant, const _Microseconds&) {
		return constant / 1000000.0 * Seconds;
	}

}
