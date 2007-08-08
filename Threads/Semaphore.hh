#ifndef SEMAPHORE_HH
#define SEMAPHORE_HH

#include <semaphore.h>

namespace Threads {

	class Semaphore {
		public:
			Semaphore(int value = 0) {
				sem_init(&this->semaphore, 0, value);
			}
			~Semaphore() {
				sem_destroy(&this->semaphore);
			}
			void post() {
				sem_post(&this->semaphore);
			}
			void wait() {
				sem_wait(&this->semaphore);
			}
		private:
			sem_t semaphore;
	};

}

#endif
