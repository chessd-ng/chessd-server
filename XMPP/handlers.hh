#ifndef XMPPHANDLERS_HH
#define XMPPHANDLERS_HH

#include "../XML/Xml.hh"

#include "Stanza.hh"

#include <boost/function.hpp>

namespace XMPP {
	typedef boost::function<void (XML::Tag*)> TagSender;
	typedef boost::function<void (Stanza*)> StanzaSender;
	typedef boost::function<void (Stanza*)> StanzaHandler;
	typedef boost::function<void (int)> ConnectionHandler;
	typedef boost::function<void (XML::Tag*)> TagHandler;
	typedef boost::function<void (std::string&, bool)> LogHandler;
	typedef boost::function<void ()> ShutdownHandler;
	typedef boost::function<void ()> TimeoutHandler;
}


#endif
