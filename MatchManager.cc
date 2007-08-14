#include <iostream>
#include "MatchManager.hh"
#include "Threads/Task.hh"
#include "Util/utils.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

static void match_log_traffic(const std::string& data, bool incoming) {
	cout << "-----------------------------" << endl;
	cout << "MatchManager: " <<
		(incoming ? "Incoming!" : "Outcoming!") << endl;
	cout << "-----------" << endl;
	cout << data << endl;
	cout << "-----------------------------" << endl;
}

MatchManager::MatchManager(CoreInterface* interface, const JID& jid) :
	core_interface(core_interface),
	component(),
	listener(this->component),
	node(boost::bind(&Component::send, &this->component, _1), "service", "game", "Match Manager"),
{
	/* Set root_node to receive stanzas from the component */
	this->component.setRootStanzaHandler(
			this->createTunnel(boost:bind(&Node::handleStanza, &this->node, _1)));

	/* Set features */
	this->root_node.disco().features().insert("presence");
	this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#match");
}

bool MatchManager::connect(const std::string& host,
		int port, const std::string& password) {
	if(not this->stream.connect(host, port, password)) {
		return false;
	} else {
		this->listener.start();
		this->start();
		return true;
	}
}

/* this handles a stanza offering a match
 *   <query xmlns=.../match>
 *     <match category=category_name />
 *     <!-- list of players involved --!>
 *     <player jid= time= inc= color= />
 *     <player jid= time= inc= color= />
 *   </query>
 * </iq>
 *
 * <iq type='result' from='jid requester' id>
 *   <query xmlns=.../match>
 *     <match category=category_name match_id= />
 *     <!-- list of involved teams --!>
 *     <team>
 *       <player jid= time= inc= color= />
 *       <player jid= time= inc= color= />
 *     </team>
 *     <team>
 *       <player jid= time= inc= color= />
 *       <player jid= time= inc= color= />
 *     </team>
 *   </query>
 * </iq>
 */
void MatchManager::handleMatchOffer(Stanza* stanza) {
}

void MatchManager::handleMatchAccept(Stanza* stanza) {
}

void MatchManager::handleMatchDecline(Stanza* stanza) {
}
