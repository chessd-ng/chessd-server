#ifndef CORE_HH
#define CORE_HH

#include "Threads/Dispatcher.hh"

#include "XML/Xml.hh"

#include "MatchManager.hh"

#include <string>
#include <set>

/*! \brief This class is the implementation of the server core.
 *
 *  It is responsible to interconnect all of the elements of
 *  the server. */


class Core {
	public:

		Core(const XML::Tag* config);

		~Core();

		/*! \brief Connects to the XMPP server.
		 *
		 * \param config holds all configurable information.
		 */
		bool connect();

	private:
		MatchManager match_manager;
};

#endif
