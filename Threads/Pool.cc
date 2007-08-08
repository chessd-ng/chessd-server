#include "Pool.hh"
#include "../utils.hh"

#include <algorithm>


using namespace std;

namespace Threads {

	const size_t StackSize = 1 << 18;

	Pool::Pool() { }

	Pool::~Pool() {
		vector<pthread_t*>& vetor = threads.getWriteLock();
		foreach(thread, vetor) {
			tarefas.push(0);
		}
		foreach(thread, vetor) {
			pthread_join(*thread, 0);
			delete *thread;
		}
		threads.releaseLock();
	}

	void Pool::launchTask(Task* task) {
		if(not tarefas.try_push(task)) {
			newThread();
			tarefas.push(task);
		}
	}

	static void * start_routine(void* queue) {
		Queue<Task*>* tarefas = static_cast<Queue<Task*>*>(queue);
		Task* tarefa;
		// cerr << "newThread()" << endl;
		while((tarefa = tarefas->pop())) {
			tarefa->_run();
		}
		// cerr << "thread closed" << endl;
		return 0;
	}

	void Pool::newThread() {
		pthread_t * thread = new pthread_t;
		pthread_attr_t thread_attr;
		pthread_attr_init(&thread_attr);
		pthread_attr_setstacksize(&thread_attr, StackSize);
		if(pthread_create(thread, &thread_attr, start_routine, static_cast<void*>(&tarefas))!=0) {
			//cerr << "Erro ao criar a thread" << endl;
			delete thread;
		} else {
			vector<pthread_t*>& vetor = threads.getWriteLock();
			vetor.push_back(thread);
			threads.releaseLock();
		}
	}

}
