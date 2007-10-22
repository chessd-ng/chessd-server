#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include "GameRoom.hh"

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
#include "ComponentWrapper.hh"
#include "Util/Timer.hh"
#include "Util/IDSet.hh"


class GameManager {
	public:
		/*! \brief Constructor
		 *
		 * \param config is the configuration for this component.
		 */
		GameManager(const XML::Tag& config, const XMPP::ErrorHandler& handle_error);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		~GameManager();

		/*! \brief Connect to the server.
		 *
		 * throws an exception on error
		 */
		void connect();

		/*! \brief Insert a game
		 *
		 * This is thread safe.
		 * \param game_id is the game's id
		 * \param game is the game to be inserted
		 */
		void insertGame(int game_id, Game* game);

		/*! \brief Closes the conenction to the server */
		void close();

	private:

		void _close();

		/*! \brief insertGame helper
		 *
		 * This one is not thread safe
		 */
		void _insertGame(int game_id, Game* game);

		/*! \brief close a game room */
		void closeGameRoom(int room_id);

		void _closeGameRoom(int room_id);

		/*! \brief handle an incoming game iq */
		void handleGame(XMPP::Stanza* stanza);

		void handleStanza(XMPP::Stanza* stanza);

		void handleError(const std::string& error);

		/*! \brief We run in a separated thread as a dispatcher */
		Threads::Dispatcher dispatcher;

		/*! \brief A XMPP component */
		ComponentWrapper component;
		
		/*! \brief A XMPP node*/
		XMPP::RootNode root_node;

		std::string node_name;

		/*! \brief Interface to the core */
		CoreInterface core_interface;

		/*! \brief A XMPP roster */
		//XMPP::Roster roster;

		/*! \brief Pending offers */
		//GameDatabase game_db;

		/*! \brief The XMPP server address */
		std::string server_address;

		/*! \brief The server port */
		int server_port;

		/*! \brief The server password */
		std::string server_password;

		boost::ptr_map<int, GameRoom> game_rooms;

		XMPP::ErrorHandler handle_error;

		/*! \brief Is it running? */
		bool running;

		Util::IDSet room_ids;

};

#endif
