#ifndef HANDLERS_HH
#define HANDLERS_HH

#include "xml.hh"
#include "stanza.hh"
#include <sigc++/sigc++.h>

typedef sigc::slot<void, XML::Tag*> TagSender;
typedef sigc::slot<void, Stanza*> StanzaSender;
typedef sigc::slot<void, Stanza*> StanzaHandler;
typedef sigc::slot<void, int> ConnectionHandler;
typedef sigc::slot<void, XML::Tag*> TagHandler;
typedef sigc::slot<void, std::string&, bool> LogHandler;
typedef sigc::slot<void> ShutdownHandler;


#endif
