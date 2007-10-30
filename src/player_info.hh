#ifndef PLAYER_INFO_HH
#define PLAYER_INFO_HH

#include <string>
#include "jid.hh"

class PlayerInfo {
	public:
		PlayerInfo() { }
		PlayerInfo(const JID& jid, const std::string& role) :
			_jid(jid), _role(role) { }


		const JID& jid() const { return this->_jid; }
		JID& jid() { return this->_jid; }

		const std::string& role() const { return this->_role; }
		std::string& role() { return this->_role; }
	private:
		JID _jid;
		std::string _role;
};

#endif
