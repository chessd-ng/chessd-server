#ifndef XMPPROSTER_HH
#define XMPPROSTER_HH

/*! \brief A XMPP roster implementation.
 */

#include "Jid.hh"
#include "Stanza.hh"
#include "handlers.hh"

#include <map>



namespace XMPP {

	enum UserStatus {
		Available,
		Unavailable
	};

	typedef boost::function<void (UserStatus new_status)> UserStatusHandler;

	class Roster {
		public:

			/*! \brief Constructor 
			 *
			 * \param stanza_sender is the function used to send stanzas
			 * */
			Roster(const StanzaSender& stanza_sender);

			/*! \brief Destructor */
			~Roster();

			/*! \brief Handle a presence stanza */
			void handlePresence(Stanza* stanza);

			/*! \brief Insert a user status monitor
			 *
			 * Insert a handler to be called on user status change. Current
			 * user status is immediately notified.
			 * \param user is the user's jid.
			 * \param handler is the handler function.
			 */
			void insertUserMonitor(const Jid& user, const UserStatusHandler& handler);

			/*! \brief Remove a user monitor.
			 *
			 * \param user is the user's jid.
			 */
			void removeUserMonitor(const Jid& user);

			/*! \brief Get the user status
			 *
			 * \param user is the user's jid.
			 * \return Returns the user status.
			 */
			UserStatus getUserStatus(const Jid& user) const;
		private:
			struct UserInfo {
				UserStatus status;
				UserStatusHandler handler;
				UserInfo() : status(Unavailable) { }
				UserInfo(UserStatus status) : status(status) { }
				UserInfo(UserStatusHandler handler) :
					status(Unavailable),
					handler(handler) { }
			};
			std::map<Jid, UserInfo> users;

			void setUserStatus(const Jid& user, UserStatus status);

			StanzaSender stanza_sender;
	};
}

#endif
