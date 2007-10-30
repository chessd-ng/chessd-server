#include "ComponentWrapper.hh"

#include <iostream>

ComponentWrapper::ComponentWrapper(
		ComponentWrapperHandlers handlers,
		const std::string& component_name,
		const XMPP::StanzaHandler& root_handler) :
	stream("jabber:component:accept"),
	component(component_name,
			XMPP::ComponentHandlers(
				boost::bind(&XMPP::Stream::sendTag, &this->stream, _1),
				root_handler,
				boost::bind(&ComponentWrapper::onConnect, this),
				boost::bind(&ComponentWrapper::handleError, this, _1))),
	handlers(handlers),
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
		this->component.connect(password);
		this->stream.connect(host_name, host_port);
		this->task_recv.start();
	} catch(const char* error) {
		this->_close();
		this->handlers.handleError(error);
	}
}

void ComponentWrapper::_close() {
	if(this->running) {
		this->running = false;
		this->stanza_queue.push(0);
		this->task_recv.join();
		this->task_send.join();
		this->stream.close();
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
	this->handlers.handleError(error);
}

void ComponentWrapper::sendStanza(XMPP::Stanza* stanza) {
	this->component.sendStanza(stanza);
}

void ComponentWrapper::run_recv() {
	XML::Tag* tag;
	try {
		while(this->running) {
			tag = this->stream.recvTag(1);
			if(not running) {
				if(tag) delete tag;
				tag = 0;
			}
			if(tag)
				this->component.handleTag(tag);
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

void ComponentWrapper::onConnect() {
	this->task_send.start();
}
