#include <iksemel.h>
#include <iostream>
#include "Component.hh"
#include "Jid.hh"

using namespace XML;
using namespace std;

namespace XMPP {

	Component::Component(const TagSender& tag_sender,
			const std::string& password) :
		tag_sender(tag_sender),
		password(password),
		status(0) { }

	Component::~Component() { }

	void Component::handleTag(Tag* tag) {
		int error = 0;
		if(this->status == 0) {
			if (tag->name() != "stream:stream") {
				error = 1;
				this->status = -1;
			} else if(tag->attributes().count("id") == 0) {
				error = 1;
				this->status = -1;
			} else {
				string data = tag->attributes()["id"] + this->password;
				this->password = "";
				char hash[128];
				iks_sha(data.c_str(), hash);
				Tag* tag = new Tag("handshake");
				tag->children().push_back(new CData(hash));
				this->tag_sender(tag);
				this->status = 1;
			}
			if(not this->conn_handler.empty())
				this->conn_handler(this->status);
			delete tag;
		} else if(this->status == 1) {
			if(tag->name() == "handshake") {
				this->status = 2;
			} else {
				error = 1;
				this->status = -1;
			}
			if(not this->conn_handler.empty())
				this->conn_handler(this->status);
			delete tag;
		} else if(this->status == 2) {
			Stanza* stanza = new Stanza(tag);
			HandlerMap::const_iterator it;
			it = this->node_handlers.find(stanza->to().node());
			if(it != this->node_handlers.end())
				it->second(stanza);
			else if(not this->root_handler.empty())
				this->root_handler(stanza);
			else {
				delete stanza;
				cout << "ignoring message" << endl;
			}
		}
		//return error;
	}

	void Component::setConnectionHandler(const ConnectionHandler& handler) {
		this->conn_handler = handler;
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
		Tag *tag = stanza->tag();
		this->tag_sender(tag);
	}

}
