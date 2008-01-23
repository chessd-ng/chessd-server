/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

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
