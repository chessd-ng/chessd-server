#include <iostream>
#include "stanza.hh"

using namespace std;
using namespace XML;

Stanza::Stanza(const std::string& type) : _type(type) { }

Stanza::~Stanza() {
	TagList::iterator it;
	for(it = this->children().begin(); it != this->children().end(); ++it)
		delete *it;
}

Stanza::Stanza(Tag* tag) {
	this->type().swap(tag->name());
	this->subtype().swap(tag->attributes()["type"]);
	this->to() = JID(tag->attributes()["to"]);
	this->from() = JID(tag->attributes()["from"]);
	this->lang().swap(tag->attributes()["xml:lang"]);
	this->id().swap(tag->attributes()["id"]);
	/* this will destroy the original tag */
	/* remove non tag children */
	ChildrenList::iterator it;
	for(it = tag->children().begin(); it != tag->children().end(); ++it) {
		Tag* tag = dynamic_cast<Tag*>(*it);
		if(not tag) {
			delete *it;
		} else {
			this->children().push_back(tag);
		}
	}
	/* XXX */
	tag->children().clear();
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
	for(it = this->children().begin(); it != this->children().end(); ++it)
		tag->children().push_back(*it);
	this->children().clear();
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
		(*it)->xml(tmp);
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
		ret->children().push_back((Tag*)(*it)->clone());
	return ret;
}

