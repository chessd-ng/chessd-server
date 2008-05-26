#include "Thread.hh"

namespace Threads {

    /* Is 256Kb enough? */
    const size_t StackSize = 1 << 18;

    Thread::Thread(boost::function<void (Thread*)> on_idle) :
        running(false),
        on_idle(on_idle) { }

    Thread::~Thread() {
        if(this->running) {
            this->stop();
            this->join();
        }
    }

    static void * start_routine(void* _thread) {
        Thread* thread = static_cast<Thread*>(_thread);

        thread->run();

        return 0;
    }

    bool Thread::start() {
        pthread_attr_t thread_attr;
        pthread_attr_init(&thread_attr);
        pthread_attr_setstacksize(&thread_attr, StackSize);
        if(pthread_create(&this->thread, &thread_attr, start_routine, static_cast<void*>(this))==0) {
            return this->running = true;
        } else {
            return this->running = false;
        }
    }

    void Thread::run() {
        Task* task;
        while(1) {
            if(not this->queue.try_pop(task)) {
                this->on_idle(this);
                task = this->queue.pop();
            }
            if(task == 0) {
                break;
            }
            task->run();
            /* delete task? */
        }
        this->running = false;
    }

    void Thread::queueTask(Task* task) {
        this->queue.push(task);
    }

    void Thread::stop() {
        this->queue.push(0);
    }

    void Thread::join() {
        pthread_join(this->thread, 0);
    }

}
