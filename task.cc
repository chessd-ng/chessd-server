#include "task.hh"
#include "pool.hh"

Task::Task() : status(TaskIdle), waiting(false) { }

Task::~Task() { }

void Task::start() {
	Pool::singleton()->launchTask(this);
}

void Task::_run() {
	mutex.lock();
	status = TaskRunning;
	mutex.unlock();
	this->run();
	mutex.lock();
	status = TaskZombie;
	condition.signal();
	mutex.unlock();
}

bool Task::join() {
	mutex.lock();
	if(waiting) {
		mutex.unlock();
		return false;
	}
	waiting = true;
	if(status == TaskIdle || status == TaskRunning)
		condition.wait(mutex);
	status = TaskCompleted;
	waiting = false;
	mutex.unlock();
	return true;
}

TaskStatus Task::getStatus() const {
	return status;
}
