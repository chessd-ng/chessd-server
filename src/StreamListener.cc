#include "StreamListener.hh"

#include <boost/bind.hpp>

StreamListener::StreamListener(
		const StreamListenerHandlers& handlers,
		XMPP::Stream& stream) :
	handlers(handlers),
	stream(stream),
	running(false),
	task_recv(boost::bind(&StreamListener::run_recv, this)),
	task_send(boost::bind(&StreamListener::run_send, this)),
	tag_queue()
{ }

StreamListener::~StreamListener() { }

void StreamListener::run_recv() {
	XML::Tag* tag;
	try {
		while(this->running) {
			tag = this->stream.recvTag(1);
			if(tag)
				this->handlers.handleTag(tag);
		}
	} catch (const char* error) {
		this->handlers.handleError(error);
	}
}

void StreamListener::run_send() {
	XML::Tag* tag;
	while(this->running) {
		tag = this->tag_queue.pop();
		if(tag==0)
			break;
		this->stream.sendTag(tag);
	}
}

void StreamListener::stop() {
	if(this->running) {
		this->running = false;
		this->tag_queue.push(0);
		this->task_recv.join();
		this->task_send.join();
	}
}

void StreamListener::start() {
	this->running = true;
	this->task_recv.start();
	this->task_send.start();
}

void StreamListener::sendTag(XML::Tag* tag) {
	this->tag_queue.push(tag);
}
