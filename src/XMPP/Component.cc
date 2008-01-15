#include "Component.hh"
#include "Jid.hh"

#include <iksemel.h>
#include <iostream>
#include <memory>

using namespace XML;

enum STATUS {
    NOT_CONNECTED,
    WAITING_HANDSHAKE,
    CONNECTED
};

namespace XMPP {

    Component::Component(const std::string& component_name) :
        stream("jabber:component:accept"),
		component_name(component_name) { }

	Component::~Component() { }

	Stanza* Component::recvStanza(int max_time) {
        Tag* tag = this->stream.recvTag(max_time);
        if(tag != 0) {
            return new Stanza(tag);
        } else {
            return 0;
        }
	}

	void Component::sendStanza(Stanza* stanza) {
		/*if(stanza->from().empty())
			stanza->from() = this->component_name;
		Tag *tag = stanza->tag();*/
		this->stream.sendTag(Stanza::createTag(stanza));
	}

    void Component::connect(const std::string& host, int port, const std::string& password ) {
        std::auto_ptr<Tag> tag;

        this->stream.connect(host, port);

        tag = std::auto_ptr<Tag>(this->stream.recvTag(-1));

        if (tag->name() != "stream:stream") {
            throw "Athentication error: Invalid protocol";
        } else if(not tag->hasAttribute("id")) {
            throw "Athentication error: Invalid protocol";
        } else {
            std::string data = tag->getAttribute("id") + password;
            char hash[128];
            iks_sha(data.c_str(), hash);
            Tag* tag_tmp = new Tag("handshake");
            tag_tmp->children().push_back(new CData(hash));
            this->stream.sendTag(tag_tmp);
        }

        tag = std::auto_ptr<Tag>(this->stream.recvTag(-1));

        if(tag->name() != "handshake") {
            throw "Athentication error: Permission Denied";
        }
    }

    void Component::close() {
        this->stream.close();
    }
    
}
