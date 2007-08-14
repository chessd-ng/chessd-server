#include "Roster.hh"

using namespace std;

namespace XMPP {
	Roster::Roster(const StanzaHandler& handler) :
		stanza_sender(stanza_sender) { }

	Roster::~Roster() { }

	void Roster::handlePresence(Stanza* stanza) {
		if(stanza->subtype().empty()) {
			this->setUserStatus(stanza->from(), Available);
		} else if(stanza->subtype() == "unavailable") {
			this->setUserStatus(stanza->from(), Unavailable);
		} else {
			stanza = Stanza::createErrorStanza(stanza, "modify", "bad-request");
			this->stanza_sender(stanza);
		}
	}

	void Roster::setUserStatus(const Jid& user, UserStatus status) {
		std::map<Jid, UserInfo>::iterator it = this->users.find(user);
		if(it == this->users.end()) {
			this->users.insert(make_pair(user, UserInfo(status)));
		} else {
			it->second.status = status;
			if(not it->second.handler.empty())
				it->second.handler(status);
		}
	}

	void Roster::insertUserMonitor(const Jid& user, const UserStatusHandler& handler) {
		std::map<Jid, UserInfo>::iterator it = this->users.find(user);
		if(it == this->users.end()) {
			this->users.insert(make_pair(user,UserInfo(handler)));
			handler(Unavailable);
		} else {
			it->second.handler = handler;
			it->second.handler(it->second.status);
		}
	}

	void Roster::removeUserMonitor(const Jid& user) {
		this->users.find(user)->second.handler.clear();
	}

	UserStatus Roster::getUserStatus(const Jid& user) const {
		std::map<Jid, UserInfo>::const_iterator it = this->users.find(user);
		if(it == this->users.end()) {
			return Unavailable;
		} else {
			return it->second.status;
		}
	}
};
