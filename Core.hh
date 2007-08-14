#ifndef CORE_HH
#define CORE_HH

#include "Threads/Dispatcher.hh"

#include <string>
#include <set>

/*! \brief This class is the implementation of the server core.
 *
 *  It is responsible to interconnect all of the elements of
 *  the server. */

typedef std::set<std::string, std::string> Properties;
typedef std::set<std::string, Properties> Config;

class Core : public Threads::Dispatcher {
	public:

		Core(const Config& config);

		~Core();

		/*! \brief Connects to the XMPP server.
		 *
		 * \param config holds all configurable information.
		 */
		bool connect();

	private:
};

#endif
