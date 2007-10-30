#ifndef TASK_HH
#define TASK_HH

#include "Mutex.hh"
#include "Condition.hh"

#include <boost/function.hpp>

namespace Threads {

	enum TaskStatus {
		/* The task is ready to run */
		TaskReady,
		/* The task is waiting to be executed */
		TaskWaiting,
		/* The task is running */
		TaskRunning
	};


	/*! \brief Executes stuff in parallel
	 */
	class Task {
		public:
			/*! \brief Constructor
			 *
			 * \param function to be called in a separate thread
			 */
			Task(const boost::function<void ()>& function);

			/*! \brief Destructor
			 * 
			 * If the task is still running, it will joi it.
			 * */
			~Task();

			/*! \brief Launch the task.
			 *
			 * The given function in the constructor will be
			 * called in a separated thread
			 */
			void start();

			/*! \brief Join the task.
			 *
			 * It will block until the task finishes.
			 */
			bool join();

			/*! \brief Get task status.
			 *
			 * \return Return the current task status
			 */
			TaskStatus getStatus() const;

			/*! \brief Run the task */
			void run();
		private:
			Mutex mutex;
			Condition condition;
			TaskStatus status;
			bool waiting;
			boost::function<void ()> function;
	};

}

#endif
