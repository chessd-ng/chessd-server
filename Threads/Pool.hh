#ifndef POOL_HH
#define POOL_HH

#include "../Util/Singleton.hh"
#include "Queue.hh"
#include "Task.hh"
#include "SafeObject.hh"

#include <pthread.h>
#include <vector>

namespace Threads {

	class Pool : public Util::Singleton<Pool> {
		public:

			void launchTask(Task* task);

		private:

			Queue<Task*> tasks;

			SafeObject<std::vector<pthread_t*> > threads;

			friend class Singleton<Pool>;

			Pool();

			~Pool();

			void newThread();
	};

}
#endif
