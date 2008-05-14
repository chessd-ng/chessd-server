#ifndef THREADS_HH
#define THREADS_HH

#include <pthread.h>

#include "Task.hh"

#include "Queue.hh"

#include <boost/function.hpp>

namespace Threads {

    class Thread {
        public:
            Thread(boost::function<void (Thread*)> on_idle);

            ~Thread();

            bool start();

            void run();

            void queueTask(Task* task);

            void stop();

            void join();

        private:

            bool running;

            pthread_t thread;

            Queue<Task*> queue;

            boost::function<void (Thread*)> on_idle;
    };

}


#endif
