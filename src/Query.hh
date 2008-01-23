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
