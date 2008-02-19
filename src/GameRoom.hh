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

#ifndef GAMEROOM_HH
#define GAMEROOM_HH

#include "XMPP/Muc.hh"
#include "XMPP/Node.hh"
#include "XMPP/handlers.hh"
#include "Game.hh"
#include "Agreement.hh"
#include "DatabaseManager.hh"
#include "Threads/Dispatcher.hh"

enum GameStatus {
	GAME_RUNNING,
	GAME_ENDED
};


struct GameRoomHandlers {
	typedef boost::function<void ()> CloseGame;
	XMPP::StanzaHandler send_stanza;
	CloseGame close_game;
	GameRoomHandlers(const XMPP::StanzaHandler& send_stanza, const CloseGame& close_game):
		send_stanza(send_stanza),
		close_game(close_game) { }
};

/*! \brief A game room. */
class GameRoom : public XMPP::Muc {
	public:
        /*! \brief Constructor.
         *
         * \param game is the game being played.
         * \param room_name is the room's jid.
         * \param database_manager is an referene to the database.
         * \param handlers are the functions needed by the room
         */
        GameRoom(
                Game* game,
                const XMPP::Jid& room_name,
                DatabaseManager& database_manager,
                Threads::Dispatcher& dispatcher,
                const GameRoomHandlers& handlers
                );

        /*! \brief Destructor. */
		~GameRoom();

	private:
		enum GameRequest {
			REQUEST_DRAW,
			REQUEST_CANCEL,
			REQUEST_ADJOURN,
			REQUEST_DRAW_DECLINE,
			REQUEST_CANCEL_DECLINE,
			REQUEST_ADJOURN_DECLINE
		};

		/*! \brief Handle an incoming game iq */
		void handleGame(XMPP::Stanza* stanza);

		/*! \brief Handle a move stana. */
		void handleMove(const XMPP::Stanza& stanza);

		/*! \brief Handle a resign stana. */
		void handleResign(const XMPP::Stanza& stanza);

		/*! \brief Handle a draw stana. */
		void handleDrawAccept(const XMPP::Stanza& stanza);

		/*! \brief Handle a draw-decline stana. */
		void handleDrawDecline(const XMPP::Stanza& stanza);

		/*! \brief Handle a cancel stanza. */
		void handleCancelAccept(const XMPP::Stanza& stanza);

		/*! \brief Handle a cancel-decline stanza. */
		void handleCancelDecline(const XMPP::Stanza& stanza);

		/*! \brief Handle a adjourn stanza. */
		void handleAdjournAccept(const XMPP::Stanza& stanza);

		/*! \brief Handle a adjourn-decline stanza. */
		void handleAdjournDecline(const XMPP::Stanza& stanza);

        /*! \brief Notify a request to the users. */
		void notifyRequest(GameRequest request, const XMPP::Jid& requester);

        /*! \brief Send the user the current game state. */
		void notifyState(const XMPP::Jid& jid);

        /*! \brief Send the user the game result. */
        void notifyResult(const XMPP::Jid& user);

        /*! \brief Notify a move to the users. */
		void notifyMove(const std::string& long_move);

        /*! \brief Cancel the game. */
		void cancelGame();

        /*! \brief Adjourn the game. */
		void adjournGame();

        /*! \brief End the game. */
        void endGame();

        /*! \brief Receive a notification of a user in the muc. */
        void notifyUserStatus(const XMPP::Jid& jid, const std::string& nick, bool available);

        /*! \brief Handle a game iq. */
        void handleGameIq(const XMPP::Stanza& from);

        /*! \brief check timeouts */
        void checkTime();

        XML::Tag* gameState();

		std::auto_ptr<Game> game;

        std::auto_ptr<XMPP::Stanza> result_stanza;

        std::auto_ptr<XML::Tag> game_state;

		XMPP::Jid room_jid;

        DatabaseManager& database_manager;

        Threads::Dispatcher& dispatcher;

		GameRoomHandlers handlers;

		Agreement draw_agreement;

		Agreement cancel_agreement;

		Agreement adjourn_agreement;

		std::set<XMPP::Jid> all_players;

		bool game_active;

        Util::Time start_time;


        int move_count;
};

#endif
