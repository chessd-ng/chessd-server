#include "Disco.hh"
#include "Util/utils.hh"

#include <iostream>

using namespace std;
using namespace XML;

namespace XMPP {

	DiscoItem::DiscoItem(const std::string& name,
			const Jid& jid) :
		_name(name),
		_jid(jid) { }

	Disco::Disco(const StanzaHandler& sender,
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
		Tag& query = stanza->children().tags().front();
		swap(stanza->to(),stanza->from());
		stanza->subtype() = "result";
		Tag* identity = new Tag("identity");
		identity->setAttribute("category", this->category());
		identity->setAttribute("type", this->type());
		identity->setAttribute("name", this->name());
		query.children().push_back(identity);
		StringSet::const_iterator fit = this->features().begin();
		foreach(feature, this->features()) {
			Tag* tag_feature = new Tag("feature");
			tag_feature->setAttribute("var", *feature);
			query.children().push_back(tag_feature);
		}
		this->stanza_sender(stanza);
	}

	void Disco::handleIqItems(Stanza* stanza) {
		Tag& query = stanza->children().tags().front();
		swap(stanza->to(),stanza->from());
		stanza->subtype() = "result";
		foreach(item, this->items()) {
			Tag* tag_item = new Tag("item");
			tag_item->setAttribute("name", item->name());
			tag_item->setAttribute("jid", item->jid().full());
			query.children().push_back(tag_item);
		}
		this->stanza_sender(stanza);
	}
}
