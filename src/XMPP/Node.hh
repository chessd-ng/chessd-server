#ifndef XMPPNODE_HH
#define XMPPNODE_HH

#include <map>
#include "handlers.hh"
#include "Disco.hh"
#include "Util/IDSet.hh"


namespace XMPP {

	class Node {
		public:
			/*! \brief Constructor
			 *
			 * \param send_stanza is a function to be used to send a stanza.
			 * \param jid is the node's jid.
			 * \param name is the node name.
			 * \param category is the node category according to disco spec.
			 * \param type if the node type.
			 */
			Node(const StanzaHandler& send_stanza,
					const Jid& jid,
					const std::string& name,
					const std::string& category,
					const std::string& type);

			virtual ~Node();

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

			/*! \brief Send an Iq of type get or set.
			 *
			 * Choose an id for the iq and send it.
			 * When the result is received, it is passed to
			 * on_result. When timeout occurs on_timeout is called.
			 *
			 * \param stazna is the iq.
			 * \param on_result is the result callback.
			 * \param on_timeout is the timeout callback.
			 * */
			void sendIq(Stanza* stanza, const StanzaHandler& on_result = StanzaHandler(),
					const TimeoutHandler& on_timeout = TimeoutHandler());

			void sendStanza(Stanza* stanza);


		private:

			void handleIq(Stanza* stanza);
			void handleMessage(Stanza* stanza);
			void handlePresence(Stanza* stanza);

			typedef std::map<std::string, StanzaHandler> HandlerMap;
			HandlerMap message_handlers;
			HandlerMap iq_handlers;
			StanzaHandler presence_handler;
			StanzaHandler send_stanza;

			Disco _disco;

			Util::IDSet iq_ids;

			struct IQTrack {
				Jid jid;
				StanzaHandler on_result;
				TimeoutHandler on_timeout;
				IQTrack(const Jid& jid,
						const StanzaHandler& on_result,
						const TimeoutHandler& on_timeout) :
					jid(jid),
					on_result(on_result),
					on_timeout(on_timeout) { }
			};

			std::map<int, IQTrack> iq_tracks;

			Jid jid;
	};

}

#endif
