#include "Dispatcher.hh"

namespace Threads {

	Dispatcher::Dispatcher() : running(true) { }

	Dispatcher::~Dispatcher() {
		if(this->running) {
			this->shutdown();
			this->join();
		}
	}

	void Dispatcher::run() {
		while(this->running)
			this->dispatch();
	}

	void Dispatcher::stop() {
		this->post(new TypedMessage<void>(sigc::mem_fun(this, &Dispatcher::_stop), false));
	}

	void Dispatcher::_stop() {
		this->running = false;
	}

	void Dispatcher::post(Message* message) {
		this->queue.push(message);
	}

	void Dispatcher::dispatch() {
		Message* message = this->queue.pop();
		message->send();
		/* if the message is asynchronous it has
		 * to be deleted here, because the sender
		 * is not waiting for it*/
		if(not message->isSync())
			delete message;
	}

}