#ifndef TASK_HH
#define TASK_HH

#include "mutex.hh"
#include "condition.hh"

enum TaskStatus {
	TaskIdle,
	TaskRunning,
	TaskZombie,
	TaskCompleted
};

class Task {
	public:
		Task();

		virtual ~Task();

		virtual void run() = 0;

		void start();

		bool join();

		void _run();

		TaskStatus getStatus() const;
	private:
		Mutex mutex;
		Condition condition;
		TaskStatus status;
		bool waiting;
};

#endif
