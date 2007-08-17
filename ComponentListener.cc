#include "ComponentListener.hh"
#include <boost/bind.hpp>

ComponentListener::ComponentListener(XMPP::Component& component) :
	component(component), running(false),
	task(boost::bind(&ComponentListener::run, this)) { }

ComponentListener::~ComponentListener() {
	if(this->running) {
		this->stop();
	}
}

void ComponentListener::start() {
	this->running = true;
	this->task.start();
}

void ComponentListener::run() {
	while(this->running) {
		this->component.deliverStanza(1);
	}
}

void ComponentListener::stop() {
	this->running = false;
	this->task.join();
}
