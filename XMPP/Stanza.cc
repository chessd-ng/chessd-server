#include <iostream>

#include "Stanza.hh"

#include "../Util/utils.hh"

using namespace std;
using namespace XML;

namespace XMPP {

	Stanza::Stanza(const std::string& type) : _type(type) { }

	Stanza::~Stanza() { }

	static bool is_cdata(const XML::Item& item) {
		return typeid(item) == typeid(XML::CData);
	}
	Stanza::Stanza(Tag* tag) {
		this->type().swap(tag->name());
		this->subtype().swap(tag->attributes()["type"]);
		this->to() = Jid(tag->attributes()["to"]);
		this->from() = Jid(tag->attributes()["from"]);
		this->lang().swap(tag->attributes()["xml:lang"]);
		this->id().swap(tag->attributes()["id"]);
		tag->children().erase_if(is_cdata);
		this->children().transfer(this->children().end(), tag->children());
		delete tag;
	}

	Tag* Stanza::tag() {
		Tag* tag = new Tag(this->type());
		if(not this->subtype().empty())
			tag->attributes()["type"].swap(this->subtype());
		if(not this->lang().empty())
			tag->attributes()["xml:lang"].swap(this->lang());
		if(not this->from().empty())
			tag->attributes()["from"] = this->from().full();
		if(not this->to().empty())
			tag->attributes()["to"] = this->to().full();
		if(not this->id().empty())
			tag->attributes()["id"].swap(this->id());
		TagList::iterator it;
		tag->children().transfer(tag->children().end(), this->children());
		delete this;
		return tag;
	}

	string Stanza::xml() const {
		string tmp;
		tmp += "<";
		tmp += this->type() + " ";
		if(not this->from().empty())
			tmp += "from='" + this->from().full() + "'";
		if(not this->to().empty())
			tmp += "to='" + this->to().full() + "'";
		if(not this->subtype().empty())
			tmp += "type='" + this->subtype() + "'";
		if(not this->lang().empty())
			tmp += "xml:lang='" + this->lang() + "'";
		if(not this->id().empty())
			tmp += "id='" + this->id() + "'";
		tmp += ">";
		TagList::const_iterator it;
		for(it = this->children().begin(); it != this->children().end(); ++it)
			it->xml(tmp);
		tmp += "</" + this->type() + ">";
		return tmp;
	}

	// TODO replace error and condition string for enums ?
	Stanza* Stanza::createErrorStanza(Stanza* stanza,
			const std::string& error,
			const std::string& condition,
			const std::string& text) {
		stanza->to().swap(stanza->from());
		stanza->subtype() = "error";
		Tag* deferr = new Tag(condition);
		deferr->attributes()["xmlns"] = "urn:ietf:params:xml:ns:xmpp-stanzas";
		Tag* errort = new Tag("error");
		errort->attributes()["type"] = error;
		errort->children().push_back(deferr);
		if(not text.empty()) {
			Tag* ttext = new Tag("text");
			ttext->attributes()["xmlns"] = "urn:ietf:params:xml:ns:xmpp-stanzas";
			errort->children().push_back(ttext);
			errort->children().push_back(new CData(text));
		}
		stanza->children().push_back(errort);
		return stanza;
	}

	Stanza* Stanza::clone() const {
		Stanza* ret = new Stanza(this->type());
		ret->subtype() = this->subtype();
		ret->id() = this->id();
		ret->from() = this->from();
		ret->to() = this->to();
		ret->lang() = this->lang();
		TagList::const_iterator it;
		for(it = this->children().begin(); it != this->children().end(); ++it)
			ret->children().push_back((Tag*)it->clone());
		return ret;
	}

	void Stanza::clearChildren() {
		this->children().clear();
	}

	Stanza* Stanza::createIQResult(Stanza* stanza) {
		swap(stanza->from(), stanza->to());
		stanza->subtype() = "result";
		stanza->clearChildren();
		return stanza;
	}
}
