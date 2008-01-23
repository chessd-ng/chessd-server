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
