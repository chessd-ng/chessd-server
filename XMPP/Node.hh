#ifndef XMPPNODE_HH
#define XMPPNODE_HH

#include <map>
#include "Jid.hh"
#include "handlers.hh"


namespace XMPP {

	class Node {
		public:
			Node(const Jid& jid, const StanzaSender& sender);
			~Node();

			void setMessageHandler(const StanzaHandler& handler,
					const std::string& subtype);
			void removeMessageHandler(const std::string& subtype);

			void setIqHandler(const StanzaHandler& handler, const std::string& xmlns);
			void removeIqHandler(const std::string& xmlns);

			void setPresenceHandler(const StanzaHandler& handler);
			void removePresenceHandler();

			void handleStanza(Stanza* stanza);

			const Jid& jid() const { return this->_jid; }

		private:
			Jid _jid;

			void notifyIq(Stanza* stanza);
			void notifyMessage(Stanza* stanza);
			void notifyPresence(Stanza* stanza);

			typedef std::map<std::string, StanzaHandler> HandlerMap;
			HandlerMap message_handlers;
			HandlerMap iq_handlers;
			StanzaHandler presence_handler;
			StanzaHandler stanza_sender;
	};

}

#endif
