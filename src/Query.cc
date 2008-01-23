/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

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
    swap(tag->children(), query->children());
	stanza->children().push_back(tag);
	return stanza;
}
