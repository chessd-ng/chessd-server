#ifndef CORE_INTERFACE_HH
#define CORE_INTERFACE_HH

#include <boost/function.hpp>
#include "XMPP/Jid.hh"
#include "Game.hh"

typedef std::map<std::string, Rating> UserRatings;

typedef boost::function<void ()> ShutdownHandler;
typedef boost::function<void (UserRatings*)> RatingCallback;

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

	private:

		void shutdown();

};

#endif
