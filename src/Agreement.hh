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

/*! \brief A helper class to manage any kind of agreement betwen users */

class Agreement {
	public:
		Agreement();

        /*! \brief Insert a user and set its state to not agreed */
		void insert(const XMPP::Jid& jid);

        /*! \brief Set a user state to agreed*/
		void agreed(const XMPP::Jid& jid);

        /*! \brief Returns how many users has not agreed */
		int left_count() const { return this->agreement.size() - this->_agreed_count; }

        /*! \brief Returns how many users has agreed */
		int agreed_count() const { return this->_agreed_count; }

        /*! \brief Reset the class to initial state */
		void clear();

        /*! \brief Returns then number of users in he agreement */
		int size() const { return this->agreement.size(); }
	private:
		typedef std::map<XMPP::Jid, bool> AgreementMap;

		AgreementMap agreement;

		int _agreed_count, _left_count;

};

#endif
