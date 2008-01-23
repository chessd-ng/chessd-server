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

#ifndef CONDITION_HH
#define CONDITION_HH

#include <pthread.h>

#include "Mutex.hh"

namespace Threads {

	class Condition {
		private:
			pthread_cond_t condition;
		public:
			Condition() {
				pthread_cond_init(&condition, 0);
			}
			~Condition() {
				pthread_cond_destroy(&condition);
			}
			void signal() {
				pthread_cond_signal(&condition);
			}
			void wait(Mutex& mutex) {
				pthread_cond_wait(&condition, &mutex.mutex);
			}
			void broadcast() {
				pthread_cond_broadcast(&condition);
			}
	};

}

#endif
