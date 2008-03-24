/*
 *   Copyright (c) 2007-2008 C3SL.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

#include <memory>
#include "AdminComponent.hh"
#include "Util/utils.hh"
#include "Util/Date.hh"

#include "Exception.hh"
#include "XMPP/Exception.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

#define XMLNS_CHESSD_ADMIN "http://c3sl.ufpr.br/chessd#admin"

AdminComponent::AdminComponent(
        const XML::Tag& config,
        const XMPP::ErrorHandler& error_handler,
        DatabaseManager& database) :
    ComponentBase(config, "Admin Component"),
    error_handler(error_handler),
    database(database),
    server_name(config.getAttribute("server_name"))
{

    /* Set features */
    this->root_node.disco().features().insert(XMLNS_CHESSD_ADMIN);

    /* Set iqs */
    this->root_node.setIqHandler(boost::bind(&AdminComponent::handleAdmin, this, _1),
            XMLNS_CHESSD_ADMIN);
}

AdminComponent::~AdminComponent() {
}

void AdminComponent::handleAdmin(const Stanza& stanza) {
    try {
        const Tag& query = stanza.query();

        /* check if the format is correct */
        foreach(tag, query.tags()) {
            if(tag->name() == "kick") {
                if(not tag->hasAttribute("jid"))
                    throw XMPP::bad_request("Invalid format");
            } else {
                throw XMPP::bad_request("invalid format");
            }
        }

        /* We need to know the user type before proceeding */
        this->database.queueTransaction(boost::bind(&AdminComponent::fetchUserType, this, stanza, _1));
    } catch (const XML::xml_error& error) {
        throw XMPP::bad_request("Invalid format");
    }
}

void AdminComponent::fetchUserType(const Stanza& stanza, DatabaseInterface& database) {
    std::string user = stanza.from().partial();
    std::string type = database.rating_database.getUserType(user);
    this->dispatcher.queue(boost::bind(&AdminComponent::execAdminCommand, this, stanza, type));
}

void AdminComponent::kickUser(const XMPP::Jid& user) {
    std::auto_ptr<XMPP::Stanza> message(new XMPP::Stanza("iq"));
    XML::TagGenerator generator;

    message->subtype() = "set";
    message->to() = this->server_name;
    
    generator.openTag("command");
    generator.addAttribute("xmlns", "http://jabber.org/protocol/commands");
    generator.addAttribute("node", "http://jabber.org/protocol/admin#end-user-session");
    generator.openTag("x");
    generator.addAttribute("xmlns", "jabber:x:data");
    generator.addAttribute("type", "submit");
    generator.openTag("field");
    generator.addAttribute("jid-single", "accountjid");
    generator.addAttribute("var", "accountjid");
    generator.openTag("value");
    generator.addCData(user.partial());

    message->children().push_back(generator.getTag());

    this->root_node.sendIq(message.release());
}

void AdminComponent::execAdminCommand(const Stanza& stanza, const std::string& user_type) {
    try {
        /* check privilegies */
        if(user_type != "admin") {
            throw XMPP::not_acceptable("You don't have admin privilegies");
        }
        try {
            /* parse message */
            const Tag& query = stanza.query();
            foreach(tag, query.tags()) {
                if(tag->name() == "kick") {
                    this->kickUser(XMPP::Jid(tag->getAttribute("jid")));
                }
            }
        } catch (const XML::xml_error& error) {
            throw XMPP::bad_request("Invalid format");
        }
    } catch(const XMPP::xmpp_exception& error) {
        this->sendStanza(error.getErrorStanza(new Stanza(stanza)));
    }
}

void AdminComponent::onError(const std::string& error) {
    this->error_handler(error);
}

void AdminComponent::onClose() {
}
