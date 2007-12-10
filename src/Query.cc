#include "Query.hh"
#include "Util/utils.hh"
#include <algorithm>

using namespace std;

Query::Query(Moved<XMPP::Stanza> stanza) :
	QueryBase(move<XMPP::Stanza>(stanza)) {
	swap(this->children(), stanza->children().tags().front().children());
}

Query::Query(Moved<Query> query) :
	QueryBase(move<QueryBase>(query)) {
	swap(this->children(), query->children());
}

Query::Query(const Query& query) :
	QueryBase(query) {
	foreach(child, query.children()) {
		this->children().push_back(child->clone());
	}
}

XMPP::Stanza* Query::createStanza(Moved<Query> query) {
	XMPP::Stanza* stanza = new XMPP::Stanza(move<StanzaBase>(*query));
	XML::Tag* tag = new XML::Tag("query");
	swap(tag->attributes()["xmlns"], query->ns());
	swap(tag->attributes()["action"], query->action());
	stanza->children().push_back(tag);
	return stanza;
}
