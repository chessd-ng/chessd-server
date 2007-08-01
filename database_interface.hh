#ifndef DATABSE_INTERFACE_HH
#define DATABSE_INTERFACE_HH

#include <sigc++/sigc++.h>
#include "user_info.hh"
#include "player_info.hh"
#include "game_history.hh"
#include "jid.hh"

class DatabaseInterface {
	public:
		/* destroy the interface */
		~DatabaseInterface();

		/* fetch or update user's general information */
		bool queryUserInfo(const JID& jid, UserInfo& info);
		void updateUserInfo(const JID& jid, const UserInfo& info);

		/* fetch or update user's status of a given game variant */
		bool queryUserRating(const JID& jid,
				const std::string variant, UserRating& status);
		void updateUserRating(const JID& jid,
				const std::string variant, const UserRating& status);

		/* store or retrieve a game history */
		void storeGame(const GameHistory* game);

	private:
		DatabaseInterface();
};

#endif
