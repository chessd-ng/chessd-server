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

#ifndef MATCHPROTOCOL_HH
#define MATCHPROTOCOL_HH

#include "XML/Xml.hh"

#include "Match.hh"

#include <memory>

#define XMLNS_MATCH         "http://c3sl.ufpr.br/chessd#match"
#define XMLNS_MATCH_OFFER   "http://c3sl.ufpr.br/chessd#match#offer"
#define XMLNS_MATCH_ACCEPT  "http://c3sl.ufpr.br/chessd#match#accept"
#define XMLNS_MATCH_DECLINE "http://c3sl.ufpr.br/chessd#match#decline"

/*! \brief The matchManagers's XMPP protocol. */
class MatchProtocol {
	public:
		/*! \brief Parse the incoming stanza.
		 *
		 * Throw an exception on error.
		 * \return A MatchOffer instance.
		 */
		//static MatchOffer* parseMatchOffer(XML::Tag& query);

		/*! Generate the answer query */
		static XML::Tag* notifyMatchOffer(const Match& match, int id);

		/*! \brief Parse a match iq
		 *
		 * \return Returns the requested action
		 * */
		static std::string parseMatch(XML::Tag& query);

		static int parseMatchAccept(XML::Tag& query);

		static int parseMatchDecline(XML::Tag& query);

		static XML::Tag* notifyMatchResult(const Match& match, int id, bool accepted);

		/* \brief Initializer
		 *
		 * Loads all XML description of the messages.
		 * \param path is the path to the xml description files
		 */
		static void init(const std::string& path);

	private:
		/* \brief Constructor */
		MatchProtocol() { }

		/*! \brief Destructir */
		~MatchProtocol() { }

		static XML::Description match_offer_desc;
		static XML::Description match_desc;
		static XML::Description match_accept_desc;
		static XML::Description match_decline_desc;
};

#endif
