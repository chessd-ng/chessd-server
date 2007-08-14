#ifndef CORE_INTERFACE_HH
#define CORE_INTERFACE_HH

#include <boost/function.hpp>
#include "XMPP/Jid.hh"

typedef boost::function<void ()> ShutdownHandler;

class Core;

class CoreInterface {
	public:
		/*! \brief destroys the interface */
		~CoreInterface();

		/*! \brief Set a function to be called on server shutdown
		 *
		 * \param handler is the function.
		 * */
		void setShutdownHandler(const ShutdownHandler& handler);

		/*! \brief Request the Core to create a game.
		 *
		 * The Core is responsible for spawning the game,
		 * Returns true in case of success, false otherwise
		 */
		//bool createGame(const GameInfo& info);

		/*! \brief Ger an interface to the database.
		 *
		 * This interface
		 * is not thread safe so there should be one interface
		 * for each thread, the caller takes the ownership of
		 * the returned object
		 */
		// DatabaseInterface* getDatabaseInterface();

	private:

		friend class Core;

		CoreInterface();

		void shutdown();
};

#endif
