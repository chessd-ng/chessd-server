/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

#include "Node.hh"
#include "Util/utils.hh"
#include "Exception.hh"
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

    void Node::handleIq(const Stanza& stanza) {
        try {
            if(not stanza.children().empty() and not stanza.id().empty()) {
                if(stanza.subtype() == "set" or stanza.subtype() == "get") {
                    HandlerMap::const_iterator it;
                    const string& ns = stanza.children().tags().front().getAttribute("xmlns");
                    it = this->iq_handlers.find(ns);
                    if(it == this->iq_handlers.end()) {
                        throw feature_not_implemented("Unknown xmlns");
                    } else {
                        /* TODO pass a reference instead ? */
                        it->second(new Stanza(stanza));
                    }
                } else if((stanza.subtype() == "result" or stanza.subtype() == "error") and
                        Util::isNumber(stanza.id())) {
                    int id = Util::parse_string<int>(stanza.id());
                    if(not Util::has_key(this->iq_tracks, id))
                        return;
                    const IQTrack& iq_track = this->iq_tracks.find(id)->second;
                    if(iq_track.jid != stanza.from())
                        return;
                    if(not iq_track.on_result.empty()) {
                        /* TODO pass a reference instead ? */
                        iq_track.on_result(new Stanza(stanza));
                    }
                    this->iq_ids.releaseID(id);
                    this->iq_tracks.erase(id);
                } else {
                    throw invalid_format("Invalid iq type");
                }
            } else {
                throw invalid_format("No id");
            }
        } catch(const XML::xml_error& error) {
            throw invalid_format("XML child or attribute missing");
        }
    }

	void Node::handleMessage(const Stanza& stanza) {
		if(not stanza.subtype().empty()) {
			HandlerMap::const_iterator it;
			it = this->message_handlers.find(stanza.subtype());
			if(it == this->message_handlers.end()) {
                throw feature_not_implemented("");
			} else {
				it->second(new Stanza(stanza));
			}
		} else {
            throw invalid_format("Missing message subtype");
		}
	}

	void Node::handlePresence(const Stanza& stanza) {
		if(not this->presence_handler.empty()) {
			this->presence_handler(new Stanza(stanza));
		}
	}

	void Node::handleStanza(Stanza* _stanza) {
        std::auto_ptr<Stanza> stanza(_stanza);
        try {
            if(stanza->type() == "presence") {
                this->handlePresence(*stanza);
            } else if(stanza->type() == "iq") {
                this->handleIq(*stanza);
            } else if(stanza->type() == "message") {
                this->handleMessage(*stanza);
            } else {
                throw invalid_format("Invalid stanza type");
            }
        } catch(const feature_not_implemented& error) {
			Stanza* resp = Stanza::createErrorStanza
				(stanza.release(), "cancel", "feature-not-implemented");
			this->sendStanza(resp);
        } catch(const invalid_format& error) {
			Stanza* resp = Stanza::createErrorStanza
				(stanza.release(), "modify", "bad-request", "Invalid format");
			this->sendStanza(resp);
		} catch(const xmpp_exception& error) {
			Stanza* resp = Stanza::createErrorStanza
				(stanza.release(), "cancel", "bad-request", error.what());
			this->sendStanza(resp);
		}
	}

	void Node::sendIq(Stanza* stanza, const StanzaHandler& on_result, const TimeoutHandler& on_timeout) {
		int id = this->iq_ids.acquireID();
		stanza->id() = Util::to_string(id);
		this->iq_tracks.insert(make_pair(id, IQTrack(stanza->to(), on_result, on_timeout)));
		this->sendStanza(stanza);
	}

}
