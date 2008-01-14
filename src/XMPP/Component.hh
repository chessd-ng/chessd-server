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
			/*! \brief Constructor 
             *
             * \param component_name is the jid of the component.
             * */
			Component(const std::string& component_name);

			/*! \brief Destructor */
			~Component();

            /* \brief Send a stanza to the jabber server. */
			void sendStanza(Stanza* stanza);

            /* \brief Receive an incoming stanza.
             *
             * \param max_time is the maximum time to wait. -1 is infinite
             * */
			Stanza* recvStanza(int max_time);

            /* \brief Connect to the server. */
            void connect(const std::string& host, int port, const std::string& password);

            /* \brief Close the connection to the server. */
            void close();

		private:

            Stream stream;

			std::string component_name;

            StanzaHandler handleStanza;

	};

}

#endif
