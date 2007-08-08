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
