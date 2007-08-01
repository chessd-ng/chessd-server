#include "stream_listener.hh"

StreamListener::StreamListener(const TagHandler& handler,
		Stream* stream) :
	handler(handler), stream(stream), running(true) {
		stream->setTagHandler(sigc::mem_fun (this,
					&StreamListener::handleTag));
}

StreamListener::~StreamListener() { }

void StreamListener::run() {
	while(this->running) {
		this->stream->recv(1);
	}
}

void StreamListener::shutdown() {
	this->running = false;
}

void StreamListener::handleTag(XML::Tag* tag) {
	this->handler(tag);
}
