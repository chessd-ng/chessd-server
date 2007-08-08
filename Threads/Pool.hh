#ifndef POOL_HH
#define POOL_HH

#include "../singleton.hh"
#include "Queue.hh"
#include "Task.hh"
#include "SafeObject.hh"

#include <pthread.h>
#include <vector>

namespace Threads {

	class Pool : public Singleton<Pool> {
		public:

			void launchTask(Task* task);

		private:

			Fila<Task*> tarefas;

			SafeObject<std::vector<pthread_t*> > threads;

			friend class Singleton<Pool>;

			Pool();

			~Pool();

			void newThread();
	};

}
#endif
