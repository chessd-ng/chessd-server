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
