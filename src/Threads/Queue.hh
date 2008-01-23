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

#ifndef FILA_HH
#define FILA_HH 

#include "Mutex.hh"
#include "Condition.hh"

#include <queue>
#include <semaphore.h>
#include <iostream>

namespace Threads {

	template <class T>
		class Queue {
			private:
				std::queue<T> queue;
				Mutex mutex; 
				Condition condition;
				size_t wait_count, tickets;
			public:
				Queue() : wait_count(0), tickets(0) { }
				~Queue() { }
				T pop() {
					mutex.lock();
					//std::cerr << "pop_lock wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
					if(tickets == 0) {
						//std::cerr << "pop_wait wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
						++wait_count;
						condition.wait(mutex);
						//std::cerr << "pop_release wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
					} else
						--tickets;
					T tmp = queue.front();
					queue.pop();
					//std::cerr << "pop_unlock wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
					mutex.unlock();
					return tmp;
				}
				bool try_pop(T& t) {
					mutex.lock();
					if(tickets == 0) {
						mutex.unlock();
						return false;
					}
					--tickets;
					t = queue.front();
					queue.pop();
					mutex.unlock();
					return true;
				}
				void push(const T& t) {
					mutex.lock();
					//std::cerr << "push_lock wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
					queue.push(t);
					if(wait_count > 0) {
						--wait_count;
						condition.signal();
						//std::cerr << "push_signal wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
					} else
						++tickets;
					//std::cerr << "push_unlock wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
					mutex.unlock();
				}
				bool try_push(const T& t) {
					mutex.lock();
					//std::cerr << "try_push_lock wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
					if(wait_count == 0) {
						//std::cerr << "try_push_fail wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
						mutex.unlock();
						return false;
					}
					queue.push(t);
					--wait_count;
					condition.signal();
					//std::cerr << "try_push_unlock wait_count=" << wait_count << " queue.size()=" << queue.size() << //std::endl;
					mutex.unlock();
					return true;
				}
		};

}

#endif
