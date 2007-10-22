#ifndef GAMEROOM_HH
#define GAMEROOM_HH

#include "XMPP/Muc.hh"
#include "XMPP/Node.hh"
#include "XMPP/handlers.hh"
#include "CoreInterface.hh"
#include "GameProtocol.hh"
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

		void handleGameMove(XMPP::Stanza* stanza);
		void handleGameResign(XMPP::Stanza* stanza);
		void handleGameDrawAccept(XMPP::Stanza* stanza);
		void handleGameDrawDecline(XMPP::Stanza* stanza);
		void handleGameCancelAccept(XMPP::Stanza* stanza);
		void handleGameCancelDecline(XMPP::Stanza* stanza);
		void handleGameAdjournAccept(XMPP::Stanza* stanza);
		void handleGameAdjournDecline(XMPP::Stanza* stanza);

		void notifyRequest(GameRequest request, const XMPP::Jid& requester);

		void notifyResult(const GameResult& result);

		void notifyGameState();
		void notifyMove(const std::string& long_move);

		void cancelGame();
		void adjournGame();

		void check_end_game();

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

		// current game state
		std::auto_ptr<XML::Tag> game_state;

		bool game_active;

		CoreInterface core;
};

#endif
