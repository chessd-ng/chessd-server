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

#ifndef MATCHMANAGER_HH
#define MATCHMANAGER_HH

#include <map>
#include <set>
#include <memory>
#include <boost/ptr_container/ptr_map.hpp>
#include "CoreInterface.hh"
#include "XMPP/Component.hh"
#include "XMPP/RootNode.hh"
#include "XMPP/Disco.hh"
#include "XMPP/Roster.hh"
#include "Threads/Dispatcher.hh"
#include "Util/Timer.hh"

#include "Match.hh"
#include "MatchRule.hh"
#include "MatchDatabase.hh"

#include "Query.hh"

#include "ComponentBase.hh"

class MatchManager : public ComponentBase {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		MatchManager(const XML::Tag& config, const XMPP::ErrorHandler& handleError);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		~MatchManager();

		/*! \brief Insert a match rule
		 *
		 * The rule's ownership is passed to this class.
		 * This must be called befre connect.
		 * \param rule is the MatchRule to be inserted.
		 */
		void insertMatchRule(MatchRule* rule);

	private:

		/* several handlers for the incoming events */

		/*! \brief handle an incoming match offer */
		void handleOffer(XMPP::Stanza* query);
		/*! \brief handle an incoming match acceptance */
		void handleAccept(XMPP::Stanza* query);
		/*! \brief handle an incoming match declinance */
		void handleDecline(XMPP::Stanza* query);

		void notifyOffer(int id, const XMPP::Jid& requester);

		void notifyResult(const Match& match, int id, bool accepted);

        void closeMatch(int id, bool accepted);

		void notifyUserStatus(const XMPP::Jid& jid, bool available);

		void _handleError(const std::string& error);

		void handleStanza(XMPP::Stanza* stanza);

        void onClose();

        void onError(const std::string& error);

		/*! \brief A XMPP roster */
		XMPP::Roster roster;

		/*! \brief Team database */
		TeamDatabase teams;

		typedef boost::ptr_map<std::string, MatchRule>  RuleMap;
		/*! \brief Registered rules */
		RuleMap rules;

		/*! \brief Pending offers */
		MatchDatabase match_db;

		Util::IDSet match_ids;

		XMPP::ErrorHandler handleError;
};

#endif
