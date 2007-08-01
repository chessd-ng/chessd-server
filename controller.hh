#ifndef CONTROLEER_HH
#define CONTROLEER_HH

#include <pthread.h>

class Controller {
	private:
		pthread_mutex_t mutex;
		pthread_cond_t condition;
		int wating_count;

	public:

		Controller();

		~Controller();



};

#endif
