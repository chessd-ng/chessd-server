#include "Component.hh"
#include "Jid.hh"

#include <iksemel.h>
#include <iostream>
#include <memory>

using namespace XML;

namespace XMPP {

	Component::Component(const std::string& component_name, 
			ComponentHandlers handlers) :
		component_name(component_name),
		handlers(handlers),
		not_authed(1) { }

	Component::~Component() { }

	void Component::handleTag(Tag* tag) {
		if(not not_authed) {
			this->handlers.handleStanza(new Stanza(tag));
		} else {
			try {
				this->auth(tag);
			} catch (const char* error) {
				this->handlers.handleError(error);
			}
		}
	}

	void Component::sendStanza(Stanza* stanza) {
		/*if(stanza->from().empty())
			stanza->from() = this->component_name;
		Tag *tag = stanza->tag();*/
		this->handlers.sendTag(stanza->tag());
	}

	void Component::auth(XML::Tag* _tag) {
		std::auto_ptr<Tag> tag(_tag);
		if(not_authed == 1) {
			if (tag->name() != "stream:stream") {
				throw "Athentication error: Invalid protocol";
			} else if(not tag->hasAttribute("id")) {
				throw "Athentication error: Invalid protocol";
			} else {
				std::string data = tag->getAttribute("id") + this->password;
				char hash[128];
				iks_sha(data.c_str(), hash);
				Tag* tag_tmp = new Tag("handshake");
				tag_tmp->children().push_back(new CData(hash));
				this->handlers.sendTag(tag_tmp);
				this->not_authed = 2;
			}
		} else if(not_authed == 2) {
			if(tag->name() == "handshake") {
				this->not_authed = 0;
				this->handlers.connected();
			} else {
				throw "Athentication error: Permission Denied";
			}
		}
	}

	void Component::connect(const std::string& password) {
		this->password = password;
		this->not_authed = 1;
	}
}
