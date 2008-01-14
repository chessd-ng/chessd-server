#ifndef POOL_HH
#define POOL_HH

#include "Util/Singleton.hh"
#include "Queue.hh"
#include "Task.hh"
#include "SafeObject.hh"

#include <pthread.h>
#include <vector>

namespace Threads {

	/*! \brief Pool of threads.
	*/
	class Pool : public Util::Singleton<Pool> {
		public:

			/*! \brief Execute the given task.
			 *
			 * The task is executed in a thread from the pool.
			 * */
			void launchTask(Task& task);

		private:

			Queue<Task*> tasks;

			SafeObject<std::vector<pthread_t> > threads;

			friend class Util::Singleton<Pool>;

			Pool();

			~Pool();

			void newThread();
	};

}
#endif
