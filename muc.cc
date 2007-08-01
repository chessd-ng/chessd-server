#include <iostream>
#include <algorithm>
#include "muc.hh"
#include "utils.hh"

using namespace std;
using namespace XML;

MucUser::MucUser(const std::string& nick, const std::string& affiliation,
			const std::string& role, const JID& jid) :
	nick(nick), affiliation(affiliation), role(role), jid(jid) { }

Muc::Muc(const StanzaSender& sender, DiscoNode* node, const JID& jid) : stanza_sender(sender),
	disco_node(node), jid(jid) {
	this->disco_node->features().insert("http://jabber.org/protocol/muc");
}

Muc::~Muc() { }

void Muc::handlePresence(Stanza* stanza) {
	if(stanza->subtype().empty()) {
		this->addUser(stanza);
	} else if(stanza->subtype() == "unavailable") {
		this->removeUser(stanza);
	} else {
		stanza = Stanza::createErrorStanza(stanza, "modify",
				"bad-request");
		this->stanza_sender(stanza);
	}
}

void Muc::addUser(Stanza* stanza) {
	string nick = stanza->to().resource();
	if(nick.empty()) {
		stanza = Stanza::createErrorStanza(stanza, "modify",
				"jid-malformed");
		this->stanza_sender(stanza);
	} else  {
		UserList::iterator itj = find_if(this->users().begin(),
				this->users().end(),
				CompareMember(&MucUser::jid, stanza->from()));
		UserList::iterator itn = find_if(this->users().begin(),
				this->users().end(),
				CompareMember(&MucUser::nick, nick));
		if(itj == this->users().end()) {
			if(itn != this->users().end()) {
				stanza = Stanza::createErrorStanza(stanza, "modify",
						"conflict");
				this->stanza_sender(stanza);
			} else  {
				this->presentUsers(stanza->from());
				this->users().push_back(MucUser(nick, "member",
							"participant", stanza->from()));
				this->disco_node->items().insert(DiscoItem(nick, stanza->to()));
				delete stanza;
				stanza = this->createPresenceStanza(this->users().back());
				this->broadcast(stanza);
			}
		} else {
			delete stanza;
		}
	}
}

void Muc::broadcast(Stanza* stanza) {
	UserList::const_iterator it;
	for(it = this->users().begin(); it != this->users().end(); ++it) {
		Stanza* tmp = stanza->clone();
		tmp->to() = it->jid;
		this->stanza_sender(tmp);
	}
	delete stanza;
}

void Muc::presentUsers(const JID& jid) {
	UserList::const_iterator it;
	for(it = this->users().begin(); it != this->users().end(); ++it) {
		Stanza* stanza = this->createPresenceStanza(*it);
		stanza->to() = jid;
		this->stanza_sender(stanza);
	}
}

Stanza* Muc::createPresenceStanza(const MucUser& user) {
	Stanza* stanza = new Stanza("presence");
	stanza->from() = this->jid;
	stanza->from().resource() = user.nick;
	TagGenerator generator;
	generator.openTag("x");
	generator.addAttribute("xmlns", "http://jabber.org/protocol/muc#user");
	generator.openTag("item");
	generator.addAttribute("affiliation", user.affiliation);
	generator.addAttribute("role", user.role);
	generator.closeTag();
	stanza->children().push_back(generator.closeTag());
	return stanza;
}

void Muc::removeUser(Stanza* stanza) {
	UserList::iterator it = find_if(this->users().begin(),
			this->users().end(),
			CompareMember(&MucUser::jid, stanza->from()));
	if(it != this->users().end()) {
		Tag* status = 0;
		if(not stanza->children().empty() and
				stanza->children().front()->name() == "status") {
			status = stanza->children().front();
			stanza->children().erase(stanza->children().begin());
		}
		delete stanza;
		stanza = this->createPresenceStanza(*it);
		stanza->subtype() = "unavailable";
		this->broadcast(stanza);
		JID jid = this->jid;
		jid.resource() = it->nick;
		this->disco_node->items().erase(DiscoItem(it->nick, jid));
		this->users().erase(it);
	} else {
		stanza = Stanza::createErrorStanza(stanza, "cancel",
				"not-acceptable");
		this->stanza_sender(stanza);
	}
}

void Muc::handleGroupChat(Stanza* stanza) {
	UserList::const_iterator it = find_if(this->users().begin(),
			this->users().end(),
			CompareMember(&MucUser::jid, stanza->from()));
	if(it != this->users().end()) {
		stanza->from().swap(stanza->to());
		stanza->from().resource() = it->nick;
		this->broadcast(stanza);
	} else {
		stanza = Stanza::createErrorStanza(stanza, "cancel", "not-acceptable");
		this->stanza_sender(stanza);
	}
}
