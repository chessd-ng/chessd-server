#include "Task.hh"
#include "Pool.hh"

namespace Threads {

	Task::Task(const boost::function<void ()>& function) : status(TaskReady), waiting(false), function(function) { }

	Task::~Task() { }

	void Task::start() {
		this->mutex.lock();
		if(this->status != TaskReady)
			return; /* TODO throw exception */
		this->status = TaskWaiting;
		Pool::singleton()->launchTask(*this);
		this->mutex.unlock();
	}

	void Task::run() {
		this->mutex.lock();
		this->status = TaskRunning;
		this->mutex.unlock();
		this->function();
		this->mutex.lock();
		this->status = TaskReady;
		this->condition.signal();
		this->mutex.unlock();
	}

	bool Task::join() {
		this->mutex.lock();
		if(this->waiting) {
			this->mutex.unlock();
			return false;
		}
		this->waiting = true;
		if(this->status != TaskReady)
			this->condition.wait(this->mutex);
		this->waiting = false;
		this->mutex.unlock();
		return true;
	}

	TaskStatus Task::getStatus() const {
		return this->status;
	}

}
