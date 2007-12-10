#ifndef QUERYBASE_HH
#define QUERYBASE_HH

#include <string>
#include "XMPP/Jid.hh"
#include "XMPP/Stanza.hh"
#include "XMPP/StanzaBase.hh"

struct QueryValidateFormat {
    QueryValidateFormat(const XMPP::Stanza& stanza);
    QueryValidateFormat() { }
};

class QueryBase : public QueryValidateFormat, public XMPP::StanzaBase {
	public:
		QueryBase(const QueryBase& stanza);
		QueryBase(Moved<QueryBase> stanza);

		QueryBase(const XMPP::Stanza& stanza);
		explicit QueryBase(Moved<XMPP::Stanza> stanza);

		const std::string& ns() const {return this->_ns;}
		std::string& ns() {return this->_ns;}
		const std::string& action() const {return this->_action;}
		std::string& action() {return this->_action;}

	private:

		std::string _ns;
		std::string _action;

};

#endif
