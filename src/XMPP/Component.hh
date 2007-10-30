#ifndef XMPPCOMPONENT_HH
#define XMPPCOMPONENT_HH

#include <string>
#include <map>
#include "Stream.hh"
#include "Stanza.hh"
#include "handlers.hh"

namespace XMPP {


	struct ComponentHandlers {
		TagHandler sendTag;
		StanzaHandler handleStanza;
		ConnectionHandler connected;
		ErrorHandler handleError;
		ComponentHandlers(
				const TagHandler& sendTag,
				const StanzaHandler& handleStanza,
				const ConnectionHandler& connected,
				const ErrorHandler& handleError) :
			sendTag(sendTag),
			handleStanza(handleStanza),
			connected(connected),
			handleError(handleError) { }
	};

	/*! \brief An implementation of a XMPP Component */
	class Component {
		public:
			/*! \brief Constructor */
			Component(const std::string& component_name,
					ComponentHandlers handlers);

			/*! \brief Destructor */
			~Component();

			void sendStanza(Stanza* stanza);

			/*! \brief Handle an incoming XML Tag.
			 */
			void handleTag(XML::Tag* tag);

			void connect(const std::string& password);

		private:

			std::string component_name;

			ComponentHandlers handlers;

			typedef std::map<std::string, StanzaHandler> HandlerMap;

			HandlerMap node_handlers;

			/*! \brief Authenticate stream
			 *
			 * */
			void auth(XML::Tag* tag);

			std::string password;

			/* already athenticated? */
			int not_authed;

	};

}

#endif
