#include "Roster.hh"

#include <iostream>

using namespace std;

namespace XMPP {
	Roster::Roster(const StanzaHandler& stanza_sender, const ChangeStatusHandler& status_handler) :
		stanza_sender(stanza_sender),
		status_handler(status_handler) { }

	Roster::~Roster() { }

	void Roster::handlePresence(Stanza* stanza) {
		if(stanza->subtype().empty()) {
			this->setUserStatus(stanza->from(), true);
			delete stanza;
		} else if(stanza->subtype() == "unavailable") {
			this->setUserStatus(stanza->from(), false);
			delete stanza;
		} else {
			stanza = Stanza::createErrorStanza(stanza, "modify", "bad-request");
			this->stanza_sender(stanza);
		}
	}

	void Roster::setUserStatus(const Jid& user, bool available) {
		this->status_handler(user, available);
		std::map<Jid, UserInfo>::iterator it = this->users.find(user);
		if(available)
			std::cout << user.full() << " is Available" << endl;
		else
			std::cout << user.full() << " is Unavailable" << endl;
		if(it == this->users.end()) {
			this->users.insert(make_pair(user, UserInfo(available)));
		} else {
			it->second.available = available;
			if(not it->second.handler.empty())
				it->second.handler(available);
		}
	}

	void Roster::insertUserMonitor(const Jid& user, const UserStatusHandler& handler) {
		std::map<Jid, UserInfo>::iterator it = this->users.find(user);
		if(it == this->users.end()) {
			this->users.insert(make_pair(user,UserInfo(handler)));
			handler(false);
		} else {
			it->second.handler = handler;
			it->second.handler(it->second.available);
		}
	}

	void Roster::removeUserMonitor(const Jid& user) {
		this->users.find(user)->second.handler.clear();
	}

	bool Roster::isUserAvailable(const Jid& user) const {
		std::map<Jid, UserInfo>::const_iterator it = this->users.find(user);
		if(it == this->users.end()) {
			return false;
		} else {
			return it->second.available;
		}
	}
};
