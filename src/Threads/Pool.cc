/*
 *   Copyright (c) 2007-2008 C3SL.
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

#include "Pool.hh"
#include "Util/utils.hh"

#include <algorithm>

#include <boost/bind.hpp>


using namespace std;

namespace Threads {

    /* Is 256Kb enough? */
	const size_t StackSize = 1 << 18;

	Pool::Pool() { }

	Pool::~Pool() {
        WriteLock<vector<Thread*> > threads(this->threads);
        /* issue a stop to all threads */
		foreach(thread, *threads) {
            (*thread)->stop();
		}
        /* wait then to stop completely */
		foreach(thread, *threads) {
            (*thread)->join();
		}
        /* free resources */
		foreach(thread, *threads) {
            delete *thread;
		}
	}

	void Pool::launchTask(Task& task) {
        Thread* thread;
        if(this->idle_threads.try_pop(thread)) {
            thread->queueTask(&task);
        } else {
            Thread* thread = new Thread(boost::bind(&Pool::threadIdled, this, _1));
            if(not thread->start()) {
                delete thread;
                this->task_queue.push(&task);
            } else {
                WriteLock<vector<Thread*> > threads(this->threads);
                threads->push_back(thread);
                thread->queueTask(&task);
            }
        }
	}

    void Pool::threadIdled(Thread* thread) {
        Task* task;

        if(this->task_queue.try_pop(task)) {
            thread->queueTask(task);
        } else {
            this->idle_threads.push(thread);
        }
    }

}
