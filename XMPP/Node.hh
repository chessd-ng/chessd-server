#ifndef XMPPNODE_HH
#define XMPPNODE_HH

#include <map>
#include "handlers.hh"
#include "Disco.hh"


namespace XMPP {

	class Node {
		public:
			/*! \brief Constructor
			 *
			 * \param sender is a function to be used to send a stanza.
			 * \param category is the node category according to disco spec.
			 * \param type if the node type.
			 * \param name is the node name.
			 */
			Node(const StanzaSender& sender,
					const std::string& category = "",
					const std::string& type = "",
					const std::string& name = "");
			~Node();

			/*! \brief Set a handler for message stanzas.
			 *
			 * A message of type subtype is sent to the handler.
			 * \param handler is a function.
			 * \param subtype is the message's type.
			 */
			void setMessageHandler(const StanzaHandler& handler,
					const std::string& subtype);

			/*! \brief Remove a message handler
			 *
			 * Removes the handler for the given type
			 * \param subtype is the message's type.
			 * */
			void removeMessageHandler(const std::string& subtype);

			/*! \brief Set a handler for iqs of the given xmlns.
			 *
			 * All the iq tanzas that has the given xmlns is forwarded to the handler.
			 * \param handler is a function.
			 * \param xmlns is the stanza's xmlns.
			 */
			void setIqHandler(const StanzaHandler& handler, const std::string& xmlns);

			/*! \brief Remove a iq handler
			 *
			 * \param xmlns is the stanza's xmlns.
			 */
			void removeIqHandler(const std::string& xmlns);

			/*! \brief Set a handler for presence stanzas
			 *
			 * All presence stanzas are forwarded to the given handler.
			 * /param handler is a function.
			 */
			void setPresenceHandler(const StanzaHandler& handler);
			/*! \brief Remove the presence handler. */
			void removePresenceHandler();

			/*! \brief Send a stanza to this  node.
			 *
			 * \param stanza is the incoming stanza.
			 */
			void handleStanza(Stanza* stanza);

			Disco& disco() { return this->_disco; }
			const Disco& disco() const { return this->_disco; }

		private:

			void notifyIq(Stanza* stanza);
			void notifyMessage(Stanza* stanza);
			void notifyPresence(Stanza* stanza);

			typedef std::map<std::string, StanzaHandler> HandlerMap;
			HandlerMap message_handlers;
			HandlerMap iq_handlers;
			StanzaHandler presence_handler;
			StanzaHandler stanza_sender;

			Disco _disco;
	};

}

#endif
