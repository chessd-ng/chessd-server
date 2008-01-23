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
