#ifndef CORE_INTERFACE_HH
#define CORE_INTERFACE_HH

#include <sigc++/sigc++.h>
#include "jid.hh"
#include "game_result.hh"
#include "game_info.hh"

typedef sigc::slot<void, JID, UserState> UserMonitor;

class Core;

class CoreInterface {
	public:
		/* destroys the interface */
		~CoreInterface();

		/* set a callback to be called in case of server shutdown */
		void setShutdownHandler(const ShutdownHandler& handler);

		/* register w watch for a user state, handler is called
		 * whenever the user's state changes */
		void setUserMonitor(const UserMonitor& handler);

		/* add and remove users to be monitored */
		void addMonitor(const JID& jid);
		void removeMonitor(const JID& jid);

		/* return current user state, this is not safe, this
		 * should only be used if you want an immediate state,
		 * the state can change before the function returns,
		 * so you get an obsolete state, you may need to set
		 * a UserMonitor to reeive changes */
		UserState getUserState(const JID& jid);

		/* request the Core to create a game
		 * the Core is responsible for spawning the game,
		 * returns 0 in case of succes, or the error code
		 * in case of an error */
		int createGame(const GameInfo& info);

		/* returns an interface to the database, this interface
		 * is not thread safe so there should be one interface
		 * for each thread, the caller is responsible for
		 * destroying the returned object */
		DatabaseInterface* getDatabaseInterface();

	private:

		friend class Core;

		CoreInterface();

		void shutdown();
};

#endif
