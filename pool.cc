#include "pool.hh"
#include <algorithm>
#include <iostream>

const size_t StackSize = 1 << 18;

using namespace std;

Pool::Pool() { }

Pool::~Pool() {
	vector<pthread_t*>* vetor = threads.getLock();
	for(int i = 0; i < int(vetor->size()); ++i) {
		tarefas.push(0);
	}
	for(int i = 0; i < int(vetor->size()); ++i) {
		pthread_join(*vetor->operator[](i), 0);
		delete vetor->operator[](i);
	}
	threads.releaseLock();
}

void Pool::launchTask(Task* task) {
	if(not tarefas.try_push(task)) {
		newThread();
		tarefas.push(task);
	}
}

static void * start_routine(void* fila) {
	Fila<Task*>* tarefas = static_cast<Fila<Task*>*>(fila);
	Task* tarefa;
	cerr << "newThread()" << endl;
	while((tarefa = tarefas->pop())) {
		tarefa->_run();
	}
	cerr << "thread closed" << endl;
	return 0;
}

void Pool::newThread() {
	vector<pthread_t*>* vetor = threads.getLock();
	pthread_t * thread = new pthread_t;
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setstacksize(&thread_attr, StackSize);
	if(pthread_create(thread, &thread_attr, start_routine, static_cast<void*>(&tarefas))!=0) {
		cerr << "Erro ao criar a thread" << endl;
		threads.releaseLock();
		delete thread;
		return;
	}
	vetor->push_back(thread);
	threads.releaseLock();
}

