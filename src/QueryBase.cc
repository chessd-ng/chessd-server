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

#include "QueryBase.hh"
#include <algorithm>

using namespace std;

QueryValidateFormat::QueryValidateFormat(const XMPP::Stanza& stanza) {
	if(stanza.type() != "iq")
		throw("Not an iq stanza");
	if(stanza.children().tags().begin() == stanza.children().tags().end())
		throw("Wrong iq format");
    const XML::Tag& tag = stanza.children().tags().front();
	if(tag.name() != "query")
		throw("Wrong query format");
	if(not tag.hasAttribute("xmlns"))
		throw("Wrong query format");
	if(not tag.hasAttribute("action"))
		throw("Wrong query format");
}

QueryBase::QueryBase(const XMPP::Stanza& stanza) :
	/* validate format first */
	QueryValidateFormat(stanza),
	StanzaBase(stanza),
	_ns(stanza.children().tags().front().getAttribute("xmlns")),
	_action(stanza.children().tags().front().getAttribute("action")) { }

QueryBase::QueryBase(Moved<XMPP::Stanza> stanza) :
	/* validate format first */
	QueryValidateFormat(*stanza),
	StanzaBase(move<StanzaBase>(*stanza)) {
		swap(_ns, stanza->children().tags().front().getAttribute("xmlns"));
		swap(_action, stanza->children().tags().front().getAttribute("action"));
	}

QueryBase::QueryBase(const QueryBase& query_base) :
	StanzaBase(query_base),
	_ns(query_base.ns()),
	_action(query_base.action()) { }

QueryBase::QueryBase(Moved<QueryBase> query_base) :
	StanzaBase(move<StanzaBase>(*query_base)) {
		swap(_ns, query_base->ns());
		swap(_action, query_base->action());
	}
