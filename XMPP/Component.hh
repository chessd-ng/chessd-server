#ifndef XMPPCOMPONENT_HH
#define XMPPCOMPONENT_HH

#include <string>
#include <map>
#include "Stream.hh"
#include "Stanza.hh"
#include "handlers.hh"

namespace XMPP {

	/*! \brief An implementation of a XMPP Component */
	class Component {
		public:
			/*! \brief Constructor */
			Component(const std::string& node_name);

			/*! \brief Destructor */
			~Component();

			/*! \brief Connect to the server.
			 *
			 * \param host is the host address.
			 * \param port is the port to be used.
			 * \param password is the server's password.
			 * \return Returns true on success, false otherwise.
			 */
			bool connect(const std::string& host,
					int port,
					const std::string& password);

			/*! \brief Closes the connection to the server
			 */
			void close();

			/*! \brief Set a stanza handler for the given node
			 *
			 * Every incoming stanza adressed to node@ something
			 * will be forwarded to the handler.
			 * \param handler is a function that handles the stanza.
			 * \param node is the node name.
			 */
			void setStanzaHandler(const StanzaHandler& handler, const std::string& node);

			/*! \brief Remove a stanza handler.
			 *
			 * The stanza handler for the given node is removed
			 * \param node id then node name.
			 */
			void removeStanzaHandler(const std::string& node);

			/*! \brief Set a handler for the root node.
			 *
			 * Every message addressed to a jid without @ will
			 * be forward to this handler.
			 * \param handler is a funtion that handles the stanzas.
			 */
			void setRootStanzaHandler(const StanzaHandler& handler);

			/*! \brief Send a stanza to the server.
			 *
			 * The stanza ownership is passed to this class.
			 * \param stanza is the stanza to be sent.
			 */
			void send(Stanza* stanza);

			/*! \brief Deliver an incoming stanza.
			 *
			 * Wait at most timeout for an incoming stanza.
			 * \param timeout is the maximun time to wait, -1 means infinite.
			 */
			void deliverStanza(int timeout = -1);

		private:

			typedef std::map<std::string, StanzaHandler> HandlerMap;

			Stream stream;

			HandlerMap node_handlers;

			StanzaHandler root_handler;

			ConnectionHandler conn_handler;

			/*std::string password, host;
			int post;*/

			int status;

			bool auth(const std::string& password);

			std::string node_name;
	};

}

#endif
