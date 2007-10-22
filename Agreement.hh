#ifndef AGREEMENT_HH
#define AGREEMENT_HH

#include "XMPP/Jid.hh"
#include <map>

class Agreement {
	public:
		Agreement();

		void insert(const XMPP::Jid& jid);

		void agreed(const XMPP::Jid& jid);

		int left_count() const { return this->agreement.size() - this->_agreed_count; }
		int agreed_count() const { return this->_agreed_count; }

		void clear();

		int size() const { return this->agreement.size(); }
	private:
		typedef std::map<XMPP::Jid, bool> AgreementMap;

		AgreementMap agreement;

		int _agreed_count, _left_count;

};

#endif
