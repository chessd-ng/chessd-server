/*
 *   Copyright (c) 2007-2008 C3SL.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

#ifndef USERMANAGER_HH
#define USERMANAGER_HH

#include "Threads/Dispatcher.hh"
#include "XMPP/Jid.hh"
#include "XMPP/Component.hh"

#include "ComponentListener.hh"

#include "CoreInterface.hh"

#include <boost/function.hpp>

#include <string>
#include <map>

/*! \brief All user status.
 */

enum UserStatus {
	Available,
	Unavailable
};


typedef boost::function<void (UserStatus new_status)> UserStatusHandle;

/*! \brief The UserManager manages the user's status.
 *
 * It keeps the users's status, wheter the user is available to play
 * or not. It provides a notification service whenever a user status chenges.
 */

class UserManager : public Threads::Dispatcher {
	public:


		/*! \brief Constructor.
		 *
		 * \param data_path is the path to the data files.
		 * \param interfcae is the interface to the core.
		 */
		UserManager(const std::string& data_path, CoreInterface* interface);

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
		 * \param ns is a namespace for the monitor. Used to differ monitors from different threads.
		 * \return None.
		 */
		void addMonitor(const XMPP::Jid& user, const UserStatusHandle& handle, int ns);

		/*! \brief Erase an existing monitor
		 *
		 * The monitor is removed and is no longer called on status change.
		 * \param user is the user's JID.
		 * \param ns is a namespace where the monitor was added in.
		 * \return None.
		 */
		void eraseMonitor(const XMPP::Jid& user, int ns);
	private:
		
		/*! \brief Store all active monitors. */
		std::map<XMPP::Jid, std::map<int, UserStatusHandle> > monitors;

		/*! \brief Store all online user's status. */
		std::map<XMPP::Jid, UserStatus> user_status;

		/*! \brief The XMPP Component. */
		XMPP::Component component;

		/*! \brief The component listener.
		 *
		 * It is important to receive the messages in a separate thread.
		 */
		ComponentListener listener;

		/*! \brief The XMPP root Node */
		XMPP::Node root_node;

		/*! \brief The XMPP Disco */
		XMPP::Disco disco;

		/*! \brief An interface to the core */
		CoreInterface* core_interface;

		/*! \brief Path to data files */
		string data_path;

		/*! \brief Connection handler */
		void handleConnection(int status);

		/*! \brief Handle a presence and deliver notifications */
		void handlePresence(Stanza* stanza);
};

#endif
