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

#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include "GameRoom.hh"

#include <map>
#include <set>
#include <memory>
#include <boost/ptr_container/ptr_map.hpp>
#include "XMPP/Component.hh"
#include "XMPP/RootNode.hh"
#include "XMPP/Disco.hh"
#include "XMPP/Roster.hh"
#include "Threads/Dispatcher.hh"
#include "ComponentBase.hh"
#include "Util/Timer.hh"
#include "Util/IDSet.hh"

#include "DatabaseManager.hh"

typedef boost::function<void (const XMPP::Jid& gmae_room)> OnGameStart;

/*! \brief Manage all games in the server plus control the game component. */
class GameManager : public ComponentBase {
	public:
		/*! \brief Constructor.
		 *
		 * \param config is the configuration for this component.
		 */
		GameManager(const XML::Tag& config, DatabaseManager& database_manager, const XMPP::ErrorHandler& handle_error);

		/*! \brief Destructor.
		 *
		 * Closes server connection if available.
		 */
		~GameManager();

		/*! \brief Create a game
		 *
		 * \param game is the game to be inserted.
		 * \param on_game_start is a notifier for the game jid.
		 * This is a tunnel the real one.
		 */
		void createGame(Game* game, const OnGameStart& on_game_start = OnGameStart());

    private:

		/*! \brief Create a game. The real one.
		 *
		 * This one is not thread safe.
		 */
		void _createGame(Game* game, const OnGameStart& on_game_start = OnGameStart());

		/*! \brief Close a game room.
         *
         * This is a tunnel.
         * */
		void closeGameRoom(int room_id);

		/*! \brief close a game room.
         *
         * This is the real one.
         * */
		void _closeGameRoom(int room_id);

        /*! \brief handle an error */
		void handleError(const std::string& error);

        /*! \brief Receive a close notification */
        void onClose();

        /*! \brief Receive an error notification */
        void onError(const std::string& msg);

		std::string node_name;

		boost::ptr_map<int, GameRoom> game_rooms;

        DatabaseManager& database_manager;

		XMPP::ErrorHandler handle_error;

		Util::IDSet room_ids;

};

#endif
