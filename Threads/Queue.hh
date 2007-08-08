#ifndef FILA_HH
#define FILA_HH 

#include <queue>
#include <semaphore.h>
#include <iostream>
#include "mutex.hh"
#include "condition.hh"

template <class T>
class Fila {
	private:
		std::queue<T> fila;
		Mutex mutex; 
		Condition condition;
		size_t wait_count, tickets;
	public:
		Fila() : wait_count(0), tickets(0) { }
		~Fila() { }
		T pop() {
			mutex.lock();
			//std::cerr << "pop_lock wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
			if(tickets == 0) {
				//std::cerr << "pop_wait wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
				++wait_count;
				condition.wait(mutex);
				//std::cerr << "pop_release wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
			} else
				--tickets;
			T tmp = fila.front();
			fila.pop();
			//std::cerr << "pop_unlock wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
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
			t = fila.front();
			fila.pop();
			mutex.unlock();
			return true;
		}
		void push(const T& t) {
			mutex.lock();
			//std::cerr << "push_lock wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
			fila.push(t);
			if(wait_count > 0) {
				--wait_count;
				condition.signal();
				//std::cerr << "push_signal wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
			} else
				++tickets;
			//std::cerr << "push_unlock wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
			mutex.unlock();
		}
		bool try_push(const T& t) {
			mutex.lock();
			//std::cerr << "try_push_lock wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
			if(wait_count == 0) {
				//std::cerr << "try_push_fail wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
				mutex.unlock();
				return false;
			}
			fila.push(t);
			--wait_count;
			condition.signal();
			//std::cerr << "try_push_unlock wait_count=" << wait_count << " fila.size()=" << fila.size() << //std::endl;
			mutex.unlock();
			return true;
		}
};

#endif
