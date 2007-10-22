#include "Agreement.hh"

#include "Util/utils.hh"

using namespace std;

Agreement::Agreement() : _agreed_count(0), _left_count(0) {
}

void Agreement::insert(const XMPP::Jid& jid) {
	this->agreement.insert(make_pair(jid, false));
}

void Agreement::agreed(const XMPP::Jid& jid) {
	AgreementMap::iterator it = this->agreement.find(jid);
	if(not it->second) {
		this->_agreed_count ++;
		it->second = true;
	}
}

void Agreement::clear() {
	foreach(it, this->agreement) {
		it->second = false;
	}
	this->_agreed_count = 0;
}
