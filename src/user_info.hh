#ifndef USER_INFO_HH
#define USER_INFO_HH

#include <string>
#include "jid.hh"


class UserInfo {
	public:
		const JID& jid() const { return this->_jid; }
		JID& jid() { return this->_jid; }
	private:
		JID _jid;

};

#endif
