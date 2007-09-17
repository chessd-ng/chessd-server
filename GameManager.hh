#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include <map>
#include <set>
#include <memory>
#include <boost/ptr_container/ptr_map.hpp>
#include "CoreInterface.hh"
#include "XMPP/Component.hh"
#include "XMPP/Node.hh"
#include "XMPP/Disco.hh"
#include "XMPP/Roster.hh"
#include "Threads/Dispatcher.hh"
#include "ComponentListener.hh"
#include "Util/Timer.hh"

#include "Game.hh"

enum GameStatus {
	GAME_RUNNING,
	GAME_CANCELED,
	GAME_ADJOURNED,
	GAME_ENDED
};

typedef boost::function<void (GameResult* result)> GameResultCallback;

class GameRoom {
	public:
		GameRoom(Game* game, int game_id, const GameResultCallback& result_callback);

		~GameRoom();

		void handleStanza(Stanza* stanza);

	private:
		enum GameRequest {
			REQUEST_DRAW,
			REQUEST_CANCEL,
			REQUEST_ADJOURN
		};
		/*! \brief handle an incoming game iq */
		void handleGame(XMPP::Stanza* stanza);

		void handleGameMove(XMPP:Stanza* stanza);
		void handleGameResign(XMPP:Stanza* stanza);
		void handleGameDraw(XMPP:Stanza* stanza);
		void handleGameCancel(XMPP:Stanza* stanza);
		void handleGameAdjourn(XMPP:Stanza* stanza);

		void notifyRequest(GameRequest request);

		void notifyResult();

		XMPP::Node node;
		//XMPP::Roster roster;
		XMPP::Muc muc;

		std::auto_ptr<Game> game;
		int game_id;

		CoreInterface core;
		//GameProtocol protocol;

		GameResultCallback result_callback;
};

class GameManager {
	public:
		/*! \brief Constructor
		 *
		 * \param config is the configuration for this component.
		 */
		GameManager(const XML::Tag& config);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		~GameManager();

		/*! \brief Connect to the server.
		 *
		 * \return Returns true on success, false otherwise.
		 */
		bool connect();

		/*! \brief Insert a game
		 *
		 * This is thread safe.
		 * \param game is the game to be inserted
		 * \param game_id is the game's id
		 */
		void insertGame(Game* game, int game_id);

		/*! \brief Closes the conenction to the server */
		void close();

	private:

		/*! \brief insertGame helper
		 *
		 * This one is not thread safe
		 */
		void _insertGame(Game* game, int game_id);

		/*! \brief handle the result of a game */
		void handleGame(GameResult* result, int game_id);

		/*! \brief handle an incoming game iq */
		void handleGame(XMPP::Stanza* stanza);

		/*! \brief We run in a separated thread as a dispatcher */
		Threads::Dispatcher dispatcher;

		/*! \brief Is it running? */
		bool running;

		/*! \brief Interface to the core */
		CoreInterface core_interface;

		/*! \brief A XMPP component */
		XMPP::Component component;
		
		std::string node_name;

		/*! \brief A component listener */
		ComponentListener listener;

		/*! \brief A XMPP node*/
		XMPP::Node root_node;

		/*! \brief A XMPP roster */
		XMPP::Roster roster;

		/*! \brief Pending offers */
		GameDatabase game_db;

		/*! \brief The XMPP server address */
		std::string server_address;


		/*! \brief The server port */
		int server_port;

		/*! \brief The server password */
		std::string server_password;

		GameProtocol protocol;

		boost::ptr_map<int, GameRoom> game_rooms;

		boost::function<void (Game*, int)> insert_game_tunnel;
};

#endif
