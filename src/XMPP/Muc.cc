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

#include <iostream>
#include <algorithm>
#include <boost/bind.hpp>

#include "Muc.hh"
#include "Util/utils.hh"

#include "Exception.hh"

using namespace std;
using namespace XML;

#define XMLNS_MUC       "http://jabber.org/protocol/muc"
#define XMLNS_MUC_USER  "http://jabber.org/protocol/muc#user"

namespace XMPP {

	MucUser::MucUser(const std::string& nick, const std::string& affiliation,
			const std::string& role, const Jid& jid) :
		nick(nick), affiliation(affiliation), role(role), jid(jid) { }

    Muc::Muc(Node& node, const Jid& jid, const OccupantMonitor& monitor) :
        node(node),
        jid(jid),
        monitor(monitor)
    {
        this->node.disco().features().insert("presence");
        this->node.setPresenceHandler(boost::bind(&Muc::handlePresence, this, _1));

        this->node.disco().features().insert(XMLNS_MUC);
        this->node.setMessageHandler(boost::bind(&Muc::handleGroupChat, this, _1), "groupchat");
    }

    Muc::~Muc() { }

	void Muc::handlePresence(const Stanza& stanza) {
		if(stanza.subtype().empty()) {
			this->addUser(stanza.to().resource(), stanza.from());
		} else if(stanza.subtype() == "unavailable") {
			this->removeUser(stanza.from(), "");
		} else {
            throw bad_request("Invalid presence type");
		}
	}

	void Muc::addUser(const std::string& nick, const Jid& user_jid) {
		if(nick.empty()) {
            throw bad_request("Invalid nick");
		} else  {
			MucUserSet::iterator itj = this->users().find_jid(user_jid);
			MucUserSet::iterator itn = this->users().find_nick(nick);
			if(itj == this->users().end()) {
				if(itn != this->users().end()) {
                    throw bad_request("This nick is already in use");
				} else  {
					this->presentUsers(user_jid);
					this->users().insert(new MucUser(nick, "member",
								"participant", user_jid));
					this->node.disco().items().insert(
							new DiscoItem(nick, Jid(this->jid.node(), jid.domain(), nick)));
					Stanza* stanza = this->createPresenceStanza(*this->users().find_jid(user_jid));
					this->broadcast(stanza);
                    if(not this->monitor.empty()) {
                        this->monitor(user_jid, nick, true);
                    }
				}
			}
		}
	}

	void Muc::broadcast(Stanza* stanza) {
		foreach(it, this->users()) {
			Stanza* tmp = new Stanza(*stanza);
			tmp->to() = it->jid;
			this->node.sendStanza(tmp);
		}
		delete stanza;
	}

	void Muc::broadcastIq(Stanza* stanza, const ConstStanzaHandler& on_result,
			const TimeoutHandler& on_timeout) {
		foreach(it, this->users()) {
			Stanza* tmp = new Stanza(*stanza);
			tmp->to() = it->jid;
			this->node.sendIq(tmp, on_result, on_timeout);
		}
		delete stanza;
	}

	void Muc::presentUsers(const Jid& jid) {
		foreach(it, this->users()) {
			Stanza* stanza = this->createPresenceStanza(*it);
			stanza->to() = jid;
			this->node.sendStanza(stanza);
		}
	}

	Stanza* Muc::createPresenceStanza(const MucUser& user) {
		Stanza* stanza = new Stanza("presence");
		stanza->from() = this->jid;
		stanza->from().resource() = user.nick;
		TagGenerator generator;
		generator.openTag("x");
		generator.addAttribute("xmlns", XMLNS_MUC_USER);
		generator.openTag("item");
		generator.addAttribute("affiliation", user.affiliation);
		generator.addAttribute("role", user.role);
		generator.closeTag();
		stanza->children().push_back(generator.getTag());
		return stanza;
	}

	void Muc::removeUser(const Jid& user_jid, const std::string& status) {
		MucUserSet::iterator it = this->users().find_jid(user_jid);
		if(it != this->users().end()) {
			Stanza* stanza = this->createPresenceStanza(*it);
			stanza->subtype() = "unavailable";
			this->broadcast(stanza);
			Jid jid = this->jid;
			jid.resource() = it->nick;
			this->node.disco().items().erase(jid);
			this->users().erase(it);
            if(not this->monitor.empty()) {
                this->monitor(user_jid, jid.resource(), false);
            }
		}
	}

	void Muc::handleGroupChat(const Stanza& stanza) {
		MucUserSet::iterator it = this->users().find_jid(stanza.from());
		if(it != this->users().end()) {
            Stanza* resp = new Stanza(stanza);
			swap(resp->to(), resp->from());
			resp->from().resource() = it->nick;
			this->broadcast(resp);
		} else {
            throw not_acceptable("Only room occupants may send message sto the room");
		}
	}

}
