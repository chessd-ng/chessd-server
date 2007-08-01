#ifndef COMPONENT_HH
#define COMPONENT_HH

#include <string>
#include <map>
#include <sigc++/signal.h>
#include "stream.hh"
#include "stanza.hh"
#include "handlers.hh"

class Component : public sigc::trackable {
	public:
		Component(const TagSender& tag_sender, const std::string& password);

		~Component();

		void auth(const std::string& password);

		void handleTag(XML::Tag* tag);

		void setConnectionHandler(const ConnectionHandler& handler);

		void setStanzaHandler(const StanzaHandler& handler, const std::string& node);

		void removeStanzaHandler(const std::string& node);

		void setRootStanzaHandler(const StanzaHandler& handler);

		void send(Stanza* stanza);

	private:

		typedef std::map<std::string, StanzaHandler> HandlerMap;

		HandlerMap node_handlers;

		TagSender tag_sender;

		StanzaHandler root_handler;

		ConnectionHandler conn_handler;

		std::string password;

		int status;
};

#endif
