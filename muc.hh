#ifndef MUC_HH
#define MUC_HH

#include <vector>
#include <string>
#include <sigc++/sigc++.h>
#include "disco.hh"
#include "handlers.hh"

struct MucUser {
	MucUser(const std::string& nick, const std::string& affiliation,
			const std::string& role, const JID& jid);
	std::string nick, affiliation, role;
	JID jid;
};

typedef std::vector<MucUser> UserList;

class Muc : public sigc::trackable {
	public:
		Muc(const StanzaSender& sender, DiscoNode* node, const JID& jid);
		~Muc();

		void handlePresence(Stanza* stanza);

		void handleGroupChat(Stanza* stanza);

		void broadcast(Stanza* stanza);

		void presentUsers(const JID& jid);

		const UserList& users() const { return this->_users; }

	private:

		StanzaSender stanza_sender;

		DiscoNode* disco_node;

		JID jid;

		UserList _users;

		void addUser(Stanza* stanza);

		void removeUser(Stanza* stanza);

		Stanza* createPresenceStanza(const MucUser& user);

		UserList& users() { return this->_users; }
};

#endif
