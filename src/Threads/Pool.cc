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

#include "Pool.hh"
#include "Util/utils.hh"

#include <algorithm>


using namespace std;

namespace Threads {

	const size_t StackSize = 1 << 18;

	Pool::Pool() { }

	Pool::~Pool() {
		vector<pthread_t>& vetor = threads.getWriteLock();
		foreach(thread, vetor) {
			tasks.push(0);
		}
		foreach(thread, vetor) {
			pthread_join(*thread, 0);
		}
		threads.releaseLock();
	}

	void Pool::launchTask(Task& task) {
		if(not tasks.try_push(&task)) {
			this->newThread();
			this->tasks.push(&task);
		}
	}

	static void * start_routine(void* queue) {
		Queue<Task*>& tasks = *static_cast<Queue<Task*>*>(queue);
		Task* tarefa;
		// cerr << "newThread()" << endl;
		while((tarefa = tasks.pop())) {
			tarefa->run();
		}
		// cerr << "thread closed" << endl;
		return 0;
	}

	void Pool::newThread() {
		vector<pthread_t>& vetor = threads.getWriteLock();
		vetor.push_back(pthread_t());
		pthread_t& thread = vetor.back();
		pthread_attr_t thread_attr;
		pthread_attr_init(&thread_attr);
		pthread_attr_setstacksize(&thread_attr, StackSize);
		if(pthread_create(&thread, &thread_attr, start_routine, static_cast<void*>(&tasks))!=0) {
			//cerr << "Erro ao criar a thread" << endl;
			/* TODO: throw exception, watch the locks! */
			vetor.pop_back();
		}
		threads.releaseLock();
	}

}
