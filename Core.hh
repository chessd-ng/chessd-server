#ifndef CORE_HH
#define CORE_HH

#include "Dispatcher.hh"

#include <string>
#include <set>

/*! \brief This class is the implementation of the server core.
 *
 *  It is responsible to interconnect all of the elements of
 *  the server. */

class Core : public Dispatcher {
	public:

		Core();

		~Core();

		/*! \brief Connects to the XMPP server.
		 *  \param config holds all configurable information.
		 */
		bool connect(const std::set<std::string, std::set<std::string, std::string> >& config);

	private:
};

#endif
