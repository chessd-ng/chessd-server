#include "Component.hh"
#include "Jid.hh"

#include <iksemel.h>
#include <iostream>
#include <boost/scoped_ptr.hpp>

using namespace XML;
using namespace std;

namespace XMPP {

	Component::Component(const std::string& node_name, const StanzaHandler& root_handler) :
		stream("jabber:component:accept"),
		node_name(node_name),
		root_handler(root_handler) { }

	Component::~Component() { }

	bool Component::connect(const std::string& host, int port, const std::string& password) {
		bool success;
		success = this->stream.connect(host, port);
		if(not success)
			return false;
		success = this->auth(password);
		if(not success)
			return false;
		else
			return true;
	}

	void Component::close() {
		this->stream.close();
	}

	bool Component::auth(const std::string& password) {
		boost::scoped_ptr<Tag> tag(this->stream.recv());
		if(tag.get() == 0)
			return false;
		if (tag->name() != "stream:stream") {
			return false;
		} else if(not tag->hasAttribute("id")) {
			return false;
		} else {
			string data = tag->getAttribute("id") + password;
			char hash[128];
			iks_sha(data.c_str(), hash);
			Tag* tag_tmp = new Tag("handshake");
			tag_tmp->children().push_back(new CData(hash));
			if(not this->stream.send(tag_tmp))
				return false;
		}
		tag.reset(this->stream.recv());
		if(tag.get() == 0)
			return false;
		if(tag->name() == "handshake") {
			return true;
		} else {
			return false;
		}
	}

	void Component::deliverStanza(int timeout) {
		Tag* tag = this->stream.recv(timeout);
		if(tag == 0)
			return;
		Stanza* stanza = new Stanza(tag);
		HandlerMap::const_iterator it;
		it = this->node_handlers.find(stanza->to().node());
		if(it != this->node_handlers.end())
			it->second(stanza);
		else 
			this->root_handler(stanza);
		}
	}

	void Component::setStanzaHandler(const StanzaHandler& handler,
			const string& node) {
		this->node_handlers[node] = handler;
	}

	void Component::removeStanzaHandler(const string& node) {
		this->node_handlers.erase(node);
	}

	void Component::setRootStanzaHandler(const StanzaHandler& handler) {
		this->root_handler = handler;
	}

	void Component::send(Stanza* stanza) {
		if(stanza->from().empty())
			stanza->from() = this->node_name;
		Tag *tag = stanza->tag();
		this->stream.send(tag);
	}
}
