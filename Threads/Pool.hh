#ifndef POOL_HH
#define POOL_HH

#include <pthread.h>
#include <vector>

#include "../singleton.hh"
#include "Queue.hh"
#include "Task.hh"
#include "SafeObject.hh"

namespace Thread {

	class Pool : public Singleton<Pool> {
		private:

			Fila<Task*> tarefas;

			SafeObject<std::vector<pthread_t*> > threads;

			friend class Singleton<Pool>;

			Pool();

			~Pool();

			void newThread();

		public:

			void launchTask(Task* task);

	};

}
#endif
