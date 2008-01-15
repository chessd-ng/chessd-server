#include "ComponentWrapper.hh"

#include <iostream>

ComponentWrapper::ComponentWrapper(
		const std::string& component_name,
		const XMPP::ErrorHandler& error_handler,
		const XMPP::StanzaHandler& root_handler) :
	component(component_name),
    root_handler(root_handler),
	error_handler(error_handler),
	task_recv(boost::bind(&ComponentWrapper::run_recv, this)),
	task_send(boost::bind(&ComponentWrapper::run_send, this)),
	running(false) {
		this->dispatcher.start();
	}

ComponentWrapper::~ComponentWrapper() {
	this->dispatcher.stop();
}

void ComponentWrapper::connect(const std::string& host_name, int host_port, const std::string& password) {
	this->running = true;
	this->dispatcher.queue(boost::bind(&ComponentWrapper::_connect,
				this, host_name, host_port, password));
}

void ComponentWrapper::_connect(const std::string& host_name, int host_port, const std::string& password) {
	try {
		this->component.connect(host_name, host_port, password);
		this->task_recv.start();
        this->task_send.start();
	} catch(const char* error) {
        this->_handleError(error);
	}
}

void ComponentWrapper::_close() {
	if(this->running) {
		this->running = false;
		this->stanza_queue.push(0);
		this->task_recv.join();
		this->task_send.join();
		this->component.close();
	}
}

void ComponentWrapper::close() {
	this->dispatcher.queue(boost::bind(&ComponentWrapper::_close, this));
}

void ComponentWrapper::handleError(const std::string& error) {
	this->dispatcher.queue(boost::bind(&ComponentWrapper::_handleError, this, error));
}

void ComponentWrapper::_handleError(const std::string error) {
	this->_close();
	this->error_handler(error);
}

void ComponentWrapper::sendStanza(XMPP::Stanza* stanza) {
	this->component.sendStanza(stanza);
}

void ComponentWrapper::run_recv() {
    XMPP::Stanza* stanza;
	try {
		while(this->running) {
			stanza = this->component.recvStanza(1);
            if(stanza != 0) {
                this->root_handler(stanza);
            }
		}
	} catch (const char* error) {
		if(this->running)
			this->handleError(error);
	}
}

void ComponentWrapper::run_send() {
	XMPP::Stanza* stanza;
	while(this->running) {
		stanza = this->stanza_queue.pop();
		if(stanza==0)
			break;
		this->component.sendStanza(stanza);
	}
}
