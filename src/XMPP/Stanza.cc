#include <iostream>

#include "Stanza.hh"

#include "../Util/utils.hh"

using namespace std;
using namespace XML;

namespace XMPP {

	Stanza::Stanza(const std::string& type) : StanzaBase(type) { }

	Stanza::Stanza(Moved<StanzaBase> stanza_base) : StanzaBase(stanza_base) { }

	Stanza::Stanza(const Stanza& stanza) :
		StanzaBase(stanza) {
		foreach(it, stanza.children())
			this->children().push_back(it->clone());
	}

	Stanza::Stanza(Moved<Stanza> stanza) :
		StanzaBase(move<StanzaBase>(*stanza)) {
		swap(this->children(), stanza->children());
	}

	Stanza::~Stanza() { }

	static bool is_cdata(const XML::Item& item) {
		return typeid(item) == typeid(XML::CData);
	}
	Stanza::Stanza(Tag* tag) : StanzaBase(tag->name()) {
		this->subtype().swap(tag->attributes()["type"]);
		this->to() = Jid(tag->attributes()["to"]);
		this->from() = Jid(tag->attributes()["from"]);
		this->lang().swap(tag->attributes()["xml:lang"]);
		this->id().swap(tag->attributes()["id"]);
		tag->children().erase_if(is_cdata);
		this->children().transfer(this->children().end(), tag->children());
		delete tag;
	}

	Stanza::Stanza(Moved<Tag> tag) : StanzaBase(tag->name()) {
		this->subtype().swap(tag->attributes()["type"]);
		this->to() = Jid(tag->attributes()["to"]);
		this->from() = Jid(tag->attributes()["from"]);
		this->lang().swap(tag->attributes()["xml:lang"]);
		this->id().swap(tag->attributes()["id"]);
		tag->children().erase_if(is_cdata);
		this->children().transfer(this->children().end(), tag->children());
	}

	Tag* Stanza::createTag(Stanza* stanza) {
		Tag* tag = new Tag(stanza->type());
		if(not stanza->subtype().empty())
			tag->attributes()["type"].swap(stanza->subtype());
		if(not stanza->lang().empty())
			tag->attributes()["xml:lang"].swap(stanza->lang());
		if(not stanza->from().empty())
			tag->attributes()["from"] = stanza->from().full();
		if(not stanza->to().empty())
			tag->attributes()["to"] = stanza->to().full();
		if(not stanza->id().empty())
			tag->attributes()["id"].swap(stanza->id());
		tag->children().transfer(tag->children().end(), stanza->children());
		delete stanza;
		return tag;
	}

	string Stanza::xml() const {
		string tmp;
		tmp += "<" + this->type();
		if(not this->from().empty())
			tmp += " from='" + this->from().full() + "'";
		if(not this->to().empty())
			tmp += " to='" + this->to().full() + "'";
		if(not this->subtype().empty())
			tmp += " type='" + this->subtype() + "'";
		if(not this->lang().empty())
			tmp += " xml:lang='" + this->lang() + "'";
		if(not this->id().empty())
			tmp += " id='" + this->id() + "'";
		tmp += ">";
        foreach(it, this->children())
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
        foreach(it, this->children())
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
