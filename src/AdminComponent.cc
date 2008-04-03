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

        /* check if the sender is an admin */
        if(this->admins.find(stanza.from()) == this->admins.end()) {
            throw XMPP::not_acceptable("You don't have admin privilegies");
        }

        /* We need to know the user type before proceeding */
        /* parse message */
        foreach(tag, query.tags()) {
            XMPP::Jid target(tag->getAttribute("jid"));
            if(tag->name() == "kick") {
                this->kickUser(target);
            } else if(tag->name() == "ban") {
                this->banUser(target);
            } else if(tag->name() == "unban") {
                this->unbanUser(target);
            } else {
                throw XMPP::bad_request("Invalid format");
            }
        }

        this->sendStanza(stanza.createIQResult());
    } catch (const XML::xml_error& error) {
        throw XMPP::bad_request("Invalid format");
    }
}

void AdminComponent::kickUser(const XMPP::PartialJid& user) {
    std::auto_ptr<XMPP::Stanza> message(new XMPP::Stanza("iq"));
    XML::TagGenerator generator;

    /* Create command message with the user name */
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
    generator.addCData(user.full());

    message->children().push_back(generator.getTag());

    /* Send the message */
    this->root_node.sendIq(message.release());
}

void AdminComponent::banUser(const XMPP::PartialJid& user) {
    this->kickUser(user);
    this->baneds.insert(user);
    this->updateAcl();
}

void AdminComponent::unbanUser(const XMPP::PartialJid& user) {
    this->baneds.erase(user);
    this->updateAcl();
}

void AdminComponent::onError(const std::string& error) {
    this->error_handler(error);
}

void AdminComponent::onClose() {
}

void AdminComponent::setAccessRules() {
    std::auto_ptr<XMPP::Stanza> message(new XMPP::Stanza("iq"));
    XML::TagGenerator generator;

    /* Create the command message */
    message->subtype() = "set";
    message->to() = this->server_name;
    
    generator.openTag("command");
    generator.addAttribute("xmlns", "http://jabber.org/protocol/commands");
    generator.addAttribute("node", "config/access");
    generator.openTag("x");
    generator.addAttribute("xmlns", "jabber:x:data");
    generator.addAttribute("type", "submit");
    generator.openTag("field");
    generator.addAttribute("type", "text-multi");
    generator.addAttribute("var", "access");
    generator.openTag("value");
    generator.addCData("[{access,muc_admin,[{allow,chessd_admin}]}].");

    message->children().push_back(generator.getTag());

    /* Send the message */
    this->root_node.sendIq(message.release());
}

void AdminComponent::onConnect() {
    /* Set ejabberd's access rules */
    this->setAccessRules();

    /* Load admin list from the database */
    this->database.queueTransaction(boost::bind(&AdminComponent::loadAdmins, this, _1));
}

void AdminComponent::loadAdmins(DatabaseInterface& database) {
    /* read from database */
    std::vector<std::string> resp = database.getAdmins();
    std::set<XMPP::PartialJid> admins;

    /* convert types */
    foreach(admin, resp) {
        admins.insert(XMPP::PartialJid(*admin));
    }

    /* update the list */
    this->dispatcher.queue(boost::bind(&AdminComponent::setAdmins, this, admins));
}

void AdminComponent::setAdmins(const std::set<XMPP::PartialJid>& admins) {
    /* set local list */
    this->admins = admins;

    /* update ejabberd's acl */
    this->updateAcl();
}

void AdminComponent::updateAcl() {
    std::auto_ptr<XMPP::Stanza> message(new XMPP::Stanza("iq"));
    XML::TagGenerator generator;

    /* Create the command message */
    message->subtype() = "set";
    message->to() = this->server_name;
    
    generator.openTag("command");
    generator.addAttribute("xmlns", "http://jabber.org/protocol/commands");
    generator.addAttribute("node", "config/acls");
    generator.openTag("x");
    generator.addAttribute("xmlns", "jabber:x:data");
    generator.addAttribute("type", "submit");
    generator.openTag("field");
    generator.addAttribute("type", "text-multi");
    generator.addAttribute("var", "acls");
    generator.openTag("value");

    /* build acl string */
    std::string acl = "[";
    /* put the admins in chessd_admin group */
    foreach(admin, this->admins) {
        acl += "{acl,chessd_admin,{user,\"" + admin->node() + "\",\"" + admin->domain() + "\"}},";
    }
    /* put baned users in the blocked group */
    foreach(ban, this->baneds) {
        acl += "{acl,blocked,{user,\"" + ban->node() + "\",\"" + ban->domain() + "\"}},";
    }
    /* remove extra comma */
    if(acl.size() > 1) {
        acl.erase(acl.end() - 1);
    }
    acl += "].";

    generator.addCData(acl);

    message->children().push_back(generator.getTag());

    /* Send the message */
    this->root_node.sendIq(message.release());
}
