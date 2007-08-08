#ifndef USERMANAGER_HH
#define USERMANAGER_HH

#include "Threads/Dispatcher.hh"
#include "XMPP/Jid.hh"

#include <boost/function.hpp>

#include <string>

/*! \brief The UserManager manages the user's status.
 *
 * It keeps the users's status, wheter the user is available to play
 * or not. It provides a notification service whenever a user status chenges.
 */

enum UserStatus {
	Available,
	Unavailable
};


typedef boost::function<void (UserStatus new_status)> UserStatusHandle;

class UserManager : public Threads::Dispatcher {
	public:


		/*! \brief Constructor.
		 *
		 * \param data_path is the path to the data files.
		 */
		UserManager(const std::string& data_path);

		/*! \brief Destructor.
		 *
		 * The destructor will call the close method if
		 * it is still active.
		 */
		~UserManager();

		/*! \brief Connects the component to the server.
		 *
		 * \param host_address is the address to the host.
		 * \param port is the port to be used.
		 * \param secret is the server's password.
		 * \return Returns true on success, false otherwise.
		 */
		bool connect(const std::string& host_address, int port, const std::string& secret);

		/*! \brief Closes the connection to the server.
		 *
		 * The connection to the server is closed, all online users is set
		 * to offline and the corresponding notifications are delivered.
		 */
		void close();

		/*! \brief Add a monitor on a user status.
		 *
		 * The handle is called when a user status is chenged. A namespace is used
		 * to differ repetead monitors. It is not allowed to add a monitor for the
		 * same user in the same namespace.
		 * \param user is the user's JID.
		 * \param handle is the function to be called.
		 * \param ns is a namespace for the monitor.
		 * \return None.
		 */
		void addMonitor(const XMPP::Jid& user, const UserStatusHandle& handle, int ns);

		/*! \brief Erase an existing monitor
		 *
		 * The monitor is removed and is no longer called on status chenge.
		 * \param user is the user's JID.
		 * \param ns is a namespace where the monitor was added in.
		 * \return None.
		 */
		void eraseMonitor(const XMPP:Jid& user, int ns);
	private:

};

#endif
