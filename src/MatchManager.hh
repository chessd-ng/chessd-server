/*
 *   Copyright (c) 2007-2008 C3SL.
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
#include "XMPP/Component.hh"
#include "XMPP/RootNode.hh"
#include "XMPP/Disco.hh"
#include "XMPP/Roster.hh"
#include "Threads/Dispatcher.hh"
#include "Util/Timer.hh"

#include "Match.hh"
#include "MatchRule.hh"
#include "MatchDatabase.hh"

#include "ComponentBase.hh"
#include "GameManager.hh"

/*! \brief This class manages all the matchs int the server. */
class MatchManager : public ComponentBase {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		MatchManager(const XML::Tag& config, GameManager& game_manager, const XMPP::ErrorHandler& handleError);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		~MatchManager();

		/*! \brief Insert a match rule
		 *
		 * This must be called befre connect.
		 * \param rule is the MatchRule to be inserted.
		 */
		void insertMatchRule(MatchRule* rule);

	private:

		/*! \brief Handle an incoming match offer. */
		void handleOffer(const XMPP::Stanza& query);

		/*! \brief Handle an incoming match acceptance. */
		void handleAccept(const XMPP::Stanza& query);

		/*! \brief Handle an incoming match declinance. */
		void handleDecline(const XMPP::Stanza& query);

		/*! \brief Notify the users of a match offer. */
		void notifyOffer(int id, const XMPP::Jid& requester);

		/*! \brief Notify the users of a match result. */
		void notifyResult(const Match& match, int id, bool accepted);

		/*! \brief Close a match. */
        void closeMatch(int id, bool accepted);

		/*! \brief Receive a notification of the user availability. */
		void notifyUserStatus(const XMPP::Jid& jid, bool available);

        /*! \brief Receive a close notification. */
        void onClose();

        /*! \brief Receive a error notification. */
        void onError(const std::string& error);

        /*! \brief Receive a game start notification.
         *
         * This is a tunnel to the real one.
         * */
        void notifyGameStart(int match_id, Match* match, const XMPP::Jid& jid);

        /*! \brief Receive a game start notification.
         *
         * This is the real one. This is required by the protocol.
         * The game room must be in the match result.
         * */
        void _notifyGameStart(int match_id, Match* match, const XMPP::Jid& jid);

        /*! \brief Send the iq result of a match offer */
        void sendOfferResult(const XMPP::Jid& to, const std::string& iq_id, int match_id);

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

        GameManager& game_manager;

		XMPP::ErrorHandler handleError;
};

#endif
