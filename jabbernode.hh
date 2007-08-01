#ifndef JABBERNODE_HH
#define JABBERNODE_HH

#include <map>
#include <sigc++/signal.h>
#include "jid.hh"
#include "handlers.hh"


class JabberNode : public sigc::trackable {
	public:
		JabberNode(const JID& jid, const StanzaSender& sender);
		~JabberNode();

		void setMessageHandler(const StanzaHandler& handler,
				const std::string& subtype);
		void removeMessageHandler(const std::string& subtype);

		void setIqHandler(const StanzaHandler& handler, const std::string& xmlns);
		void removeIqHandler(const std::string& xmlns);

		void setPresenceHandler(const StanzaHandler& handler);
		void removePresenceHandler();

		void handleStanza(Stanza* stanza);

		const JID& jid() const { return this->_jid; }

	private:
		JID _jid;

		void notifyIq(Stanza* stanza);
		void notifyMessage(Stanza* stanza);
		void notifyPresence(Stanza* stanza);
		
		typedef std::map<std::string, StanzaHandler> HandlerMap;
		HandlerMap message_handlers;
		HandlerMap iq_handlers;
		StanzaHandler presence_handler;
		StanzaHandler stanza_sender;
};

#endif
