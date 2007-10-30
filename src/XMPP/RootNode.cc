#include "RootNode.hh"
#include <boost/bind.hpp>

using namespace XML;
using namespace std;

namespace XMPP {

	RootNode::RootNode(const StanzaHandler& send_stanza,
			const XMPP::Jid& jid,
			const std::string& name,
			const std::string& category,
			const std::string& type) :
		Node(send_stanza, jid, name, category, type) {
			this->node_handlers.insert(make_pair("",boost::bind(&Node::handleStanza, this, _1)));
		}

	RootNode::~RootNode() { }

	void RootNode::handleStanza(Stanza* stanza) {
		HandlerMap::const_iterator it;
		it = this->node_handlers.find(stanza->to().node());
		if(it != this->node_handlers.end())
			it->second(stanza);
		else {
			this->sendStanza(Stanza::createErrorStanza(stanza, "cancel", "item-not-found"));
		}
	}

	void RootNode::setStanzaHandler(const string& node, const StanzaHandler& handler) {
		this->node_handlers.insert(make_pair(node,handler));
	}

	void RootNode::removeStanzaHandler(const string& node) {
		this->node_handlers.erase(node);
	}
}
