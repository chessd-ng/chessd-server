#ifndef CONDITION_HH
#define CONDITION_HH

#include <pthread.h>
#include "mutex.hh"

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

#endif
