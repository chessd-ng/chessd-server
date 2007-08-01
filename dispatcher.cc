#include "dispatch.hh"

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

void Dispatcher::shutdown() {
	this->post(new TypedMessage<void>(sigc::mem_fun(this, &Dispatcher::_shutdown), false));
}

void Dispatcher::_shutdown() {
	this->running = false;
}

void Dispatcher::post(Message* message) {
	this->fila.push(message);
}

void Dispatcher::dispatch() {
	Message* message = this->fila.pop();
	message->send();
	/* if the message is asynchronous it has
	 * to be deleted here, because the sender
	 * is not wating for it*/
	if(not message->isSync())
		delete message;
}

void Dispatcher::setShutdownHandler(const ShutdownHandler& handler) {
	this->shutdown_handler = handler;
}
