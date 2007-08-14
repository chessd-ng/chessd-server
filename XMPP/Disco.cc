#include "Disco.hh"

#include <iostream>

using namespace std;
using namespace XML;

namespace XMPP {

	DiscoItem::DiscoItem(const std::string& name,
			const Jid& jid) :
		name(name),
		jid(jid) { }

	bool DiscoItem::operator<(const DiscoItem& item) const {
		return this->jid < item.jid;
	}


	Disco::Disco(const StanzaSender& sender,
			const std::string& name,
			const std::string& category,
			const std::string& type) :
		stanza_sender(sender),
		_name(name),
		_category(category),
		_type(type) { }

	Disco::~Disco() {
	}

	void Disco::handleIqInfo(Stanza* stanza) {
		Tag* query = stanza->children().front();
		swap(stanza->to(),stanza->from());
		stanza->subtype() = "result";
		Tag* identity = new Tag("identity");
		identity->setAttribute("category", this->category());
		identity->setAttribute("type", this->type());
		identity->setAttribute("name", this->name());
		query->children().push_back(identity);
		StringSet::const_iterator fit = this->features().begin();
		for(; fit != this->features().end(); ++fit) {
			Tag* feature = new Tag("feature");
			feature->setAttribute("var", *fit);
			query->children().push_back(feature);
		}
		this->stanza_sender(stanza);
	}

	void Disco::handleIqItems(Stanza* stanza) {
		Tag* query = stanza->children().front();
		swap(stanza->to(),stanza->from());
		stanza->subtype() = "result";
		DiscoItemSet::const_iterator iit = this->items().begin();
		for(; iit != this->items().end(); ++iit) {
			Tag* item = new Tag("item");
			item->setAttribute("name", iit->name);
			item->setAttribute("jid", iit->jid.full());
			query->children().push_back(item);
		}
		this->stanza_sender(stanza);
	}
}
