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
#include "ComponentBase.hh"
#include "Util/Timer.hh"
#include "Util/IDSet.hh"


class GameManager : public ComponentBase {
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

		/*! \brief Insert a game
		 *
		 * This is thread safe.
		 * \param game_id is the game's id
		 * \param game is the game to be inserted
		 */
		void insertGame(int game_id, Game* game);

	private:

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

        void onClose();

        void onError(const std::string& msg);

		std::string node_name;

		boost::ptr_map<int, GameRoom> game_rooms;

		XMPP::ErrorHandler handle_error;

		Util::IDSet room_ids;

};

#endif
