#include "Dispatcher.hh"

namespace Threads {

	Dispatcher::Dispatcher() : 
		task(boost::bind(&Dispatcher::run, this)),
		running(false) { }

	Dispatcher::~Dispatcher() {
	}

	void Dispatcher::start() {
		this->running = true;
		this->task.start();
	}

	void Dispatcher::run() {
		while(this->running) {
			Message* message = this->_queue.pop();
			message->send();
			/* if the message is asynchronous it has
			 * to be deleted here, because the sender
			 * is not waiting for it*/
			if(not message->isSync())
				delete message;
		}
	}

	void Dispatcher::stop() {
		/* A stop message is sent to the dispatcher instead of just
		 * setting running to false. This we make sure the dispatcher
		 * will stop. */
		if(this->running == true) {
			this->queue(boost::bind(&Dispatcher::_stop, this));
			this->task.join();
		}
	}

	void Dispatcher::_stop() {
		this->running = false;
	}

	void Dispatcher::dispatch() {
	}

}
