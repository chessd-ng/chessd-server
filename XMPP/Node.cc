#include "Node.hh"
#include <boost/bind.hpp>

using namespace std;

namespace XMPP {

	Node::Node(const StanzaSender& sender,
			const std::string& name,
			const std::string& category,
			const std::string& type) :
		stanza_sender(sender),
		_disco(sender, name, category, type) {
			this->setIqHandler(boost::bind(&Disco::handleIqInfo, &this->_disco, _1),
					"http://jabber.org/protocol/disco#info");
			this->setIqHandler(boost::bind(&Disco::handleIqItems, &this->_disco, _1),
					"http://jabber.org/protocol/disco#items");
		}

	Node::~Node() { }

	void Node::setMessageHandler(const StanzaHandler& handler,
			const string& subtype) {
		this->message_handlers[subtype] = handler;
	}

	void Node::removeMessageHandler(const string& subtype) {
		this->message_handlers.erase(subtype);
	}

	void Node::setIqHandler(const StanzaHandler& handler, const string& xmlns) {
		this->iq_handlers[xmlns] = handler;
	}

	void Node::removeIqHandler(const string& xmlns) {
		this->iq_handlers.erase(xmlns);
	}

	void Node::setPresenceHandler(const StanzaHandler& handler) {
		this->presence_handler = handler;
	}

	void Node::removePresenceHandler() {
		this->presence_handler = StanzaHandler();
	}

	void Node::notifyIq(Stanza* stanza) {
		if(not stanza->children().empty() and not stanza->id().empty()) {
			HandlerMap::const_iterator it;
			const string& ns = stanza->children().front()->attributes()["xmlns"];
			it = this->iq_handlers.find(ns);
			if(it == this->iq_handlers.end()) {
				Stanza* error = Stanza::createErrorStanza(stanza,
						"cancel", "feature-not-implemented");
				this->stanza_sender(error);
			} else {
				it->second(stanza);
			}
		} else {
			Stanza* error = Stanza::createErrorStanza
				(stanza, "modify", "bad-request");
			this->stanza_sender(error);
		}
	}

	void Node::notifyMessage(Stanza* stanza) {
		if(not stanza->subtype().empty()) {
			HandlerMap::const_iterator it;
			it = this->message_handlers.find(stanza->subtype());
			if(it == this->message_handlers.end()) {
				Stanza* error = Stanza::createErrorStanza(stanza,
						"cancel", "feature-not-implemented");
				this->stanza_sender(error);
			} else {
				it->second(stanza);
			}
		} else {
			Stanza* error = Stanza::createErrorStanza(stanza,
					"modify", "bad-request");
			this->stanza_sender(error);
		}
	}

	void Node::notifyPresence(Stanza* stanza) {
		if(not this->presence_handler.empty()) {
			this->presence_handler(stanza);
		} else {
			delete stanza;
		}
	}

	void Node::handleStanza(Stanza* stanza) {
		if(stanza->type() == "presence") {
			this->notifyPresence(stanza);
		} else if(stanza->type() == "iq") {
			this->notifyIq(stanza);
		} else if(stanza->type() == "message") {
			this->notifyMessage(stanza);
		} else {
			Stanza* error = Stanza::createErrorStanza(stanza,
					"modify", "bad-request");
			this->stanza_sender(error);
		}
	}

}
