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
#include "CoreInterface.hh"
#include "Game.hh"
#include "Agreement.hh"

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

class GameRoom {
	public:
		GameRoom(int game_id, Game* game, const XMPP::Jid& room_name,
				const GameRoomHandlers& handlers);

		~GameRoom();

		void handleStanza(XMPP::Stanza* stanza);

	private:
		enum GameRequest {
			REQUEST_DRAW,
			REQUEST_CANCEL,
			REQUEST_ADJOURN
		};
		/*! \brief handle an incoming game iq */
		void handleGame(XMPP::Stanza* stanza);

		void handleMove(XMPP::Stanza* stanza);
		void handleResign(XMPP::Stanza* stanza);
		void handleDrawAccept(XMPP::Stanza* stanza);
		void handleDrawDecline(XMPP::Stanza* stanza);
		void handleCancelAccept(XMPP::Stanza* stanza);
		void handleCancelDecline(XMPP::Stanza* stanza);
		void handleAdjournAccept(XMPP::Stanza* stanza);
		void handleAdjournDecline(XMPP::Stanza* stanza);

        void notifyPlayers();

		void notifyRequest(GameRequest request, const XMPP::Jid& requester);

		void notifyResult(const GameResult& result);

		void notifyState(const XMPP::Jid& jid);
		void notifyMove(const std::string& long_move);

		void cancelGame();
		void adjournGame();

		void check_end_game();

        void reportUser(const XMPP::Jid& jid, const std::string& nick, bool available);

		int game_id;

		std::auto_ptr<Game> game;


		//GameProtocol protocol;

		XMPP::Jid room_jid;

		GameRoomHandlers handlers;

		XMPP::Node node;
		//XMPP::Roster roster;
		XMPP::Muc muc;

		Agreement draw_agreement;
		Agreement cancel_agreement;
		Agreement adjourn_agreement;

		std::set<XMPP::Jid> all_players;

		bool game_active;

        std::auto_ptr<XML::Tag> game_state;

		CoreInterface core;
};

#endif
