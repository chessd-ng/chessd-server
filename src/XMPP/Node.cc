#include "Node.hh"
#include "Util/utils.hh"
#include <boost/bind.hpp>

using namespace std;

namespace XMPP {

	Node::Node(const StanzaHandler& sender,
			const Jid& jid,
			const std::string& name,
			const std::string& category,
			const std::string& type) :
		send_stanza(sender),
		_disco(sender, name, category, type), 
	jid(jid){
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

	void Node::sendStanza(Stanza* stanza) {
        if(stanza->from().empty())
            stanza->from() = jid;
		this->send_stanza(stanza);
	}

	void Node::handleIq(Stanza* stanza) {
		try {
			if(not stanza->children().empty() and not stanza->id().empty()) {
				if(stanza->subtype() == "set" or stanza->subtype() == "get") {
					HandlerMap::const_iterator it;
					const string& ns = stanza->children().tags().front().attributes()["xmlns"];
					it = this->iq_handlers.find(ns);
					if(it == this->iq_handlers.end()) {
						Stanza* error = Stanza::createErrorStanza(stanza,
								"cancel", "feature-not-implemented");
						this->sendStanza(error);
					} else {
						it->second(stanza);
					}
				} else if((stanza->subtype() == "result" or stanza->subtype() == "error") and
						Util::isNumber(stanza->id())) {
					int id = Util::parse_string<int>(stanza->id());
					if(not Util::has_key(this->iq_tracks, id))
						throw "";
					const IQTrack& iq_track = this->iq_tracks.find(id)->second;
					if(iq_track.jid != stanza->from())
						throw "";
					if(iq_track.on_result) {
						iq_track.on_result(stanza);
					} else {
						delete stanza;
					}
					this->iq_ids.releaseID(id);
					this->iq_tracks.erase(id);
				} else {
					throw "";
				}
			} else {
				throw "";
			}
		} catch(const char* msg) {
			Stanza* error = Stanza::createErrorStanza
				(stanza, "modify", "bad-request", msg);
			this->sendStanza(error);
		}
	}

	void Node::handleMessage(Stanza* stanza) {
		if(not stanza->subtype().empty()) {
			HandlerMap::const_iterator it;
			it = this->message_handlers.find(stanza->subtype());
			if(it == this->message_handlers.end()) {
				Stanza* error = Stanza::createErrorStanza(stanza,
						"cancel", "feature-not-implemented");
				this->sendStanza(error);
			} else {
				it->second(stanza);
			}
		} else {
			Stanza* error = Stanza::createErrorStanza(stanza,
					"modify", "bad-request");
			this->sendStanza(error);
		}
	}

	void Node::handlePresence(Stanza* stanza) {
		if(not this->presence_handler.empty()) {
			this->presence_handler(stanza);
		} else {
			delete stanza;
		}
	}

	void Node::handleStanza(Stanza* stanza) {
		if(stanza->type() == "presence") {
			this->handlePresence(stanza);
		} else if(stanza->type() == "iq") {
			this->handleIq(stanza);
		} else if(stanza->type() == "message") {
			this->handleMessage(stanza);
		} else {
			Stanza* error = Stanza::createErrorStanza(stanza,
					"modify", "bad-request");
			this->sendStanza(error);
		}
	}

	void Node::sendIq(Stanza* stanza, const StanzaHandler& on_result, const TimeoutHandler& on_timeout) {
		int id = this->iq_ids.acquireID();
		stanza->id() = Util::to_string(id);
		this->iq_tracks.insert(make_pair(id, IQTrack(stanza->to(), on_result, on_timeout)));
		this->sendStanza(stanza);
	}

}
