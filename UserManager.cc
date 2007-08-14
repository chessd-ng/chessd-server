#include "UserManager.hh"

using namespace std;
using namespace XMPP;
using namespace Threads;

UserManager::UserManager(const std::string& data_path, CoreInterface* core_interface) :
	component(),
	listener(component),
	root_node(boost::bind(&Component::send, &this->component, _1)),
	disco(boost::bind(&Component::send, &this->component, _1)),
	core_interface(core_interface),
	data_path(data_path)
{
	/* Set root_node to receive stanzas from the component */
	this->component.setRootStanzaHandler(boost:bind(&Node::handleStanza, &this->node, _1));
	/* Set disco's iq handlers */
	this->root_node.setIqHandler(boost::bind(&Disco::handleIqInfo, &this->disco, _1),
			"http://jabber.org/protocol/disco#info");
	this->root_node.setIqHandler(boost::bind(&Disco::handleIqInfo, &this->disco, _1),
			"http://jabber.org/protocol/disco#info");

	/* Set the disco to report the root node */
	DiscoNode disco_node("User Manager", "chess_server", "user");
	disco_node.features().insert("presence");

}
