#ifndef XMPPMUC_HH
#define XMPPMUC_HH

#include "Disco.hh"
#include "handlers.hh"

#include <vector>
#include <string>


namespace XMPP {

	struct MucUser {
		MucUser(const std::string& nick, const std::string& affiliation,
				const std::string& role, const Jid& jid);
		std::string nick, affiliation, role;
		Jid jid;
	};

	typedef std::vector<MucUser> UserList;

	class Muc {
		public:
			Muc(const StanzaSender& sender, DiscoNode* node, const Jid& jid);
			~Muc();

			void handlePresence(Stanza* stanza);

			void handleGroupChat(Stanza* stanza);

			void broadcast(Stanza* stanza);

			void presentUsers(const Jid& jid);

			const UserList& users() const { return this->_users; }

		private:

			StanzaSender stanza_sender;

			DiscoNode* disco_node;

			Jid jid;

			UserList _users;

			void addUser(Stanza* stanza);

			void removeUser(Stanza* stanza);

			Stanza* createPresenceStanza(const MucUser& user);

			UserList& users() { return this->_users; }
	};

}

#endif
