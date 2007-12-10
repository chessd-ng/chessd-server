#ifndef QUERY_HH
#define QUERY_HH

#include "XMPP/ChildrenList.hh"
#include "XMPP/Stanza.hh"
#include "QueryBase.hh"


class Query : public QueryBase {
	public:
        Query();

		Query(Moved<Query> query);
		Query(const Query& query);

        /* we want these contructors not to be
         * used unless we specify it explicitly */
		explicit Query(const XMPP::Stanza& stanza);
		explicit Query(Moved<XMPP::Stanza> stanza);

		~Query() { }

		const XMPP::ChildrenList& children() const { return this->_children; }
		XMPP::ChildrenList& children() { return this->_children; }

		static XMPP::Stanza* createStanza(Moved<Query> query);

	private:
		XMPP::ChildrenList _children;

};

#endif
