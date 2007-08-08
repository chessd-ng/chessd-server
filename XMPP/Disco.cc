#include "Disco.hh"

#include <iostream>

using namespace std;
using namespace XML;

namespace XMPP {

	DiscoItem::DiscoItem(const std::string& name,
			const Jid& jid,
			const std::string& node) :
		name(name),
		jid(jid),
		node(node) { }

	bool DiscoItem::operator<(const DiscoItem& item) const {
		if(this->jid < item.jid)
			return true;
		else if(this->jid > item.jid)
			return false;
		else if(this->node < item.node)
			return true;
		else
			return false;
	}

	DiscoNode::DiscoNode( const std::string& name,
			const std::string& category,
			const std::string& type) :
		_name(name),
		_category(category),
		_type(type) { }

	DiscoNode::~DiscoNode() { }

	Disco::Disco(const StanzaSender& sender) : stanza_sender(sender) { }

	Disco::~Disco() {
		DiscoNodeMap::iterator it;
		for(it = this->nodes().begin(); it != this->nodes().end(); ++it)
			delete it->second;
	}

	void Disco::handleIqInfo(Stanza* stanza) {
		Jid jid = stanza->to();
		string node;
		AttributeMap::const_iterator attr_it = stanza->children().front()->attributes().find("node");
		if(attr_it ==  stanza->children().front()->attributes().end())
			node = "";
		else
			node = attr_it->second;
		DiscoNodeMap::const_iterator it = this->_nodes.find(make_pair(jid, node));
		if(it != this->_nodes.end()) {
			Tag* query = stanza->children().front();
			stanza->to().swap(stanza->from());
			stanza->subtype() = "result";
			const DiscoNode *node  = it->second;
			Tag* identity = new Tag("identity");
			identity->attributes()["category"] = node->category();
			identity->attributes()["type"] = node->type();
			identity->attributes()["name"] = node->name();
			query->children().push_back(identity);
			StringSet::const_iterator fit = node->features().begin();
			for(; fit != node->features().end(); ++fit) {
				Tag* feature = new Tag("feature");
				feature->attributes()["var"] = *fit;
				query->children().push_back(feature);
			}
			this->stanza_sender(stanza);
		} else {
			Stanza* error = Stanza::createErrorStanza(stanza,
					"cancel", "item-not-found");
			this->stanza_sender(error);
		}
	}

	void Disco::handleIqItems(Stanza* stanza) {
		Jid jid = stanza->to();
		AttributeMap::const_iterator attr_it = stanza->children().front()->attributes().find("node");
		string node;
		if(attr_it ==  stanza->children().front()->attributes().end())
			node = "";
		else
			node = attr_it->second;
		DiscoNodeMap::const_iterator it = this->_nodes.find(make_pair(jid, node));
		if(it != this->_nodes.end()) {
			Tag* query = stanza->children().front();
			stanza->to().swap(stanza->from());
			stanza->subtype() = "result";
			const DiscoNode *node  = it->second;
			DiscoItemSet::const_iterator iit = node->items().begin();
			for(; iit != node->items().end(); ++iit) {
				Tag* item = new Tag("item");
				item->attributes()["name"] = iit->name;
				item->attributes()["jid"] = iit->jid.full();
				if(not iit->node.empty())
					item->attributes()["node"] = iit->node;
				query->children().push_back(item);
			}
			this->stanza_sender(stanza);
		} else {
			Stanza* error = Stanza::createErrorStanza(stanza, "cancel", "item-not-found");
			this->stanza_sender(error);
		}
	}

}
