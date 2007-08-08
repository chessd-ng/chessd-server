#ifndef MUTEX_HH
#define MUTEX_HH

#include <pthread.h>

namespace Thread {

	class Mutex {
		private:
			pthread_mutex_t mutex;
			friend class Condition;
		public:
			Mutex() {
				pthread_mutex_init(&mutex, 0);
			}

			~Mutex() {
				pthread_mutex_destroy(&mutex);
			}

			void lock() {
				pthread_mutex_lock(&mutex);
			}

			void unlock() {
				pthread_mutex_unlock(&mutex);
			}
			bool tryLock() {
				return pthread_mutex_trylock(&mutex) == 0;
			}
	};

}

#endif
