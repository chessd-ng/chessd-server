#ifndef CORE_INTERFACE_HH
#define CORE_INTERFACE_HH

#include <boost/function.hpp>
#include "XMPP/Jid.hh"
#include "Game.hh"

typedef boost::function<void ()> ShutdownHandler;

class Core;

class CoreInterface {
	public:
		/*! \brief destroys the interface */
		~CoreInterface();

		CoreInterface();



		/*! \brief Request the Core to start a game.
		 *
		 * The Core is responsible for spawning the game,
		 * Returns true in case of success, false otherwise
		 */
		void startGame(Game* game);

		void endGame(int game_id, GameResult* result);
		void adjournGame(int game_id, GameResult* result);
		void cancelGame(int game_id);

		/*! \brief Get an interface to the database.
		 *
		 * This interface
		 * is not thread safe so there should be one interface
		 * for each thread, the caller takes the ownership of
		 * the returned object
		 */
		// DatabaseInterface* getDatabaseInterface();



	private:

		void shutdown();

		Core& core;
};

#endif
