#ifndef CORE_HH
#define CORE_HH

#include "Threads/Dispatcher.hh"

#include "XML/Xml.hh"

#include "MatchManager.hh"
#include "GameManager.hh"
#include "RatingComponent.hh"

#include "DatabaseManager.hh"

#include <string>
#include <set>

#include "Game.hh"
#include "Rating.hh"

/*! \brief This class is the implementation of the server core.
 *
 *  It is responsible to interconnect all of the elements of
 *  the server. */

typedef std::map<std::string, Rating> UserRatings;

typedef boost::function<void (UserRatings*)> RatingCallback;


class Core {
	public:


		~Core();

		/*! \brief Connects to the XMPP server.
		 *
		 * \param config holds all configurable information.
		 */
		void connect();

		static Core& singleton() { return *Core::_singleton; }

		static void init(const XML::Tag& config); 

		static void destroy();

		void startGame(Game* game);
		void endGame(int game_id, GameResult* result);
		void adjournGame(int game_id, GameResult* result);
		void cancelGame(int game_id);

	private:

		void _startGame(Game* game);
		void _endGame(int game_id, GameResult* result);
		void _adjournGame(int game_id, GameResult* result);
		void _cancelGame(int game_id);

		Core(const XML::Tag& config);

		void handleError(const std::string& error);

        DatabaseManager database_manager;

		MatchManager match_manager;
		GameManager game_manager;
		RatingComponent rating_component;


		static Core* _singleton;

		Threads::Dispatcher dispatcher;

		Util::IDSet game_ids;

		std::map<XMPP::Jid, std::map<std::string, Rating> > ratings;
};

#endif
