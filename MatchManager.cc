#include <iostream>
#include "MatchManager.hh"
#include "Threads/Task.hh"
#include "Util/utils.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

void match_log_traffic(const std::string& data, bool incoming) {
	cout << "-----------------------------" << endl;
	cout << "MatchManager: " <<
		(incoming ? "Incoming!" : "Outcoming!") << endl;
	cout << "-----------" << endl;
	cout << data << endl;
	cout << "-----------------------------" << endl;
}

MatchManager::MatchManager(CoreInterface* interface, const XML::Tag* config) :
	running(false),
	core_interface(core_interface),
	component(),
	listener(this->component),
	root_node(boost::bind(&Component::send, &this->component, _1), "Match Manager", "service", "game"),
	server_address(config->getAttribute("server_address")),
	server_port(Util::str2int(config->getAttribute("server_port"))),
	server_password(config->getAttribute("server_password"))
{
	/* Set root_node to receive stanzas from the component */
	XMPP::StanzaHandler f = boost::bind(&Node::handleStanza, &this->root_node, _1);
	this->component.setRootStanzaHandler(this->dispatcher.createTunnel(f));

	/* Set features */
	this->root_node.disco().features().insert("presence");
	this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#match");
}

MatchManager::~MatchManager() {
	if(this->running == true)
		this->close();
}

bool MatchManager::connect() {
	if(not this->component.connect(this->server_address, this->server_port, this->server_password)) {
		return false; // TODO throw an exception
	} else {
		this->running = true;
		this->dispatcher.start();
		this->listener.start();
		return true;
	}
}

void MatchManager::close() {
	this->listener.stop();
	this->dispatcher.stop();
	this->component.close();
	this->running = false;
}

void MatchManager::handleMatchOffer(Stanza* stanza) {
}

void MatchManager::handleMatchAccept(Stanza* stanza) {
}

void MatchManager::handleMatchDecline(Stanza* stanza) {
}
