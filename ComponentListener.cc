#include "ComponentListener.hh"

ComponentListener::ComponentListener(XMPP::Component& component) :
	component(component), running(false) {
}

ComponentListener::~ComponentListener() {

}

void ComponentListener::run() {
	this->running = true;
	while(this->running) {
		this->component.recv(1);
	}
}

void ComponentListener::stop() {
	this->running = false;
}
