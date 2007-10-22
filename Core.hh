#ifndef CORE_HH
#define CORE_HH

#include "Threads/Dispatcher.hh"

#include "XML/Xml.hh"

#include "MatchManager.hh"
#include "GameManager.hh"

#include <string>
#include <set>

/*! \brief This class is the implementation of the server core.
 *
 *  It is responsible to interconnect all of the elements of
 *  the server. */


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

	private:
		Core(const XML::Tag& config);

		void handleError(const std::string& error);

		MatchManager match_manager;
		GameManager game_manager;

		static Core* _singleton;
};

#endif
