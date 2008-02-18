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
		_nick(nick), _affiliation(affiliation), _role(role), _jid(jid) { }

    Muc::Muc(const Jid& jid,
            const std::string& room_title,
            const StanzaHandler send_stanza) :
        Node(send_stanza, jid, room_title, "muc", "muc")
    {
        this->disco().features().insert("presence");
        this->setPresenceHandler(boost::bind(&Muc::handlePresence, this, _1));

        this->disco().features().insert(XMLNS_MUC);
        this->setMessageHandler(boost::bind(&Muc::handleGroupChat, this, _1), "groupchat");
    }

    Muc::~Muc() { }

	void Muc::handlePresence(const Stanza& stanza) throw() {
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
                    this->disco().items().insert(
                            new DiscoItem(nick,
                                Jid(this->jid().node(),
                                    this->jid().domain(),
                                    nick)));
                    Stanza* stanza = this->createPresenceStanza(*this->users().find_jid(user_jid));
                    this->broadcast(stanza);
                    this->notifyUserStatus(user_jid, nick, true);
				}
			}
		}
	}

	void Muc::broadcast(Stanza* stanza) {
		foreach(it, this->users()) {
			Stanza* tmp = new Stanza(*stanza);
			tmp->to() = it->jid();
			this->sendStanza(tmp);
		}
		delete stanza;
	}

	void Muc::broadcastIq(Stanza* stanza, const ConstStanzaHandler& on_result,
			const TimeoutHandler& on_timeout) {
		foreach(it, this->users()) {
			Stanza* tmp = new Stanza(*stanza);
			tmp->to() = it->jid();
			this->sendIq(tmp, on_result, on_timeout);
		}
		delete stanza;
	}

	void Muc::presentUsers(const Jid& jid) {
		foreach(it, this->users()) {
			Stanza* stanza = this->createPresenceStanza(*it);
			stanza->to() = jid;
			this->sendStanza(stanza);
		}
	}

	Stanza* Muc::createPresenceStanza(const MucUser& user) {
		Stanza* stanza = new Stanza("presence");
		stanza->from() = this->jid();
		stanza->from().resource() = user.nick();
		TagGenerator generator;
		generator.openTag("x");
		generator.addAttribute("xmlns", XMLNS_MUC_USER);
		generator.openTag("item");
		generator.addAttribute("affiliation", user.affiliation());
		generator.addAttribute("role", user.role());
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
			Jid jid = this->jid();
			jid.resource() = it->nick();
			this->disco().items().erase(jid);
			this->users().erase(it);
            this->notifyUserStatus(user_jid, jid.resource(), false);
		}
	}

	void Muc::handleGroupChat(const Stanza& stanza) {
		MucUserSet::iterator it = this->users().find_jid(stanza.from());
		if(it != this->users().end()) {
            Stanza* resp = new Stanza(stanza);
			swap(resp->to(), resp->from());
			resp->from().resource() = it->nick();
			this->broadcast(resp);
		} else {
            throw not_acceptable("Only room occupants may send messages to the room");
		}
	}

    void Muc::handleStanza(Stanza* _stanza) throw () {
        std::auto_ptr<Stanza> stanza(_stanza);
        try {
            if(stanza->type() != "presence" and not stanza->to().resource().empty()) {
                MucUserSet::iterator dest, source;
                source = this->users().find_jid(stanza->from());
                if(source == this->users().end()) {
                    throw not_acceptable("Only room occupants may send messages to the room");
                }
                dest = this->users().find_nick(stanza->to().resource());
                if(dest == this->users().end()) {
                    throw item_not_found("Occupant is not present on the room");
                }
                stanza->to() = dest->jid();
                stanza->from() = Jid(this->jid().node(), this->jid().domain(), source->nick());

                this->sendStanza(stanza.release());
            } else {
                Node::handleStanza(stanza.release());
            }
        } catch (const xmpp_exception& exception) {
            this->sendStanza(exception.getErrorStanza(stanza.release()));
        }
    }

}
