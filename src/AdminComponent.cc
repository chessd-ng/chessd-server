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
        const std::string& server_name,
        DatabaseManager& database,
        const XMPP::StanzaHandler& send_stanza) :
    ServerModule(send_stanza),
    database(database),
    server_name(server_name)
{

    /* Set features */
    //this->root_node.disco().features().insert(XMLNS_CHESSD_ADMIN);

    /* Set iqs */
    //this->root_node.setIqHandler(boost::bind(&AdminComponent::handleAdmin, this, _1),
    //        XMLNS_CHESSD_ADMIN);
}

AdminComponent::~AdminComponent() {
}

vector<string> AdminComponent::namespaces() const {
    vector<string> ret;
    ret.push_back(XMLNS_CHESSD_ADMIN);
    return ret;
}

void AdminComponent::handleIq(const Stanza& stanza) {
    try {
        const Tag& query = stanza.firstTag();

        /* check if the sender is an admin */
        if(this->admins.find(stanza.from()) == this->admins.end()) {
            throw XMPP::not_acceptable("You don't have admin privilegies");
        }

        /* We need to know the user type before proceeding */
        /* parse message */
        if(query.name() == "kick") {
            this->handleKick(stanza);
        } else if(query.name() == "ban") {
            this->handleBan(stanza);
        } else if(query.name() == "unban") {
            this->handleUnban(stanza);
        } else if(query.name() == "banned-list") {
            this->handleBannedList(stanza);
        } else {
            throw XMPP::bad_request("Invalid format");
        }

    } catch (const XML::xml_error& error) {
        throw XMPP::bad_request("Invalid format");
    }
}

void AdminComponent::handleBannedList(const Stanza& stanza) {
    std::auto_ptr<XMPP::Stanza> message(stanza.createIQResult());
    XML::TagGenerator generator;

    /* create result message with the users banned */
    generator.openTag("banned-list");
    generator.addAttribute("xmlns", XMLNS_CHESSD_ADMIN);

    foreach(user, this->banneds) {
        generator.openTag("user");
        generator.addAttribute("jid", user->first.full());
        generator.addCData(user->second);
        generator.closeTag();
    }
    
    message->children().push_back(generator.getTag());

    /* Send the message */
    this->sendIq(message.release());
}

void AdminComponent::handleKick(const Stanza& stanza) {
    const Tag& query = stanza.firstTag();

    /* get user to be kicked */
    XMPP::Jid target(query.getAttribute("jid"));

    /* get kick reason */
    const std::string& reason = query.findChild("reason").findCData().data();

    /* send kick notification */
    TagGenerator generator;
    generator.openTag("iq");
    generator.addAttribute("to", target.full());
    generator.addAttribute("type", "set");
    generator.openTag("kick");
    generator.addAttribute("xmlns", XMLNS_CHESSD_ADMIN);
    generator.openTag("reason");
    generator.addCData(reason);
    std::auto_ptr<XMPP::Stanza> message(new Stanza(generator.getTag()));
    this->sendIq(message.release());

    /* kick user */
    this->kickUser(target);

    /* send result */
    auto_ptr<Stanza> result(stanza.createIQResult());
    generator.openTag("kick");
    generator.addAttribute("xmlns", XMLNS_CHESSD_ADMIN);
    result->children().push_back(generator.getTag());
    this->sendStanza(result.release());
}

void AdminComponent::handleBan(const Stanza& stanza) {
    const Tag& query = stanza.firstTag();

    /* get user to be banned */
    XMPP::Jid target(query.getAttribute("jid"));

    /* get ban reason */
    const std::string& reason = query.findChild("reason").findCData().data();

    /* send ban notification */
    TagGenerator generator;
    generator.openTag("iq");
    generator.addAttribute("to", target.full());
    generator.addAttribute("type", "set");
    generator.openTag("ban");
    generator.addAttribute("xmlns", XMLNS_CHESSD_ADMIN);
    generator.openTag("reason");
    generator.addCData(reason);
    std::auto_ptr<XMPP::Stanza> message(new Stanza(generator.getTag()));
    this->sendIq(message.release());

    /* ban user */
    this->banUser(target, reason);

    /* send result */
    auto_ptr<Stanza> result(stanza.createIQResult());
    generator.openTag("ban");
    generator.addAttribute("xmlns", XMLNS_CHESSD_ADMIN);
    result->children().push_back(generator.getTag());
    this->sendStanza(result.release());
}

void AdminComponent::handleUnban(const Stanza& stanza) {
    const Tag& query = stanza.firstTag();
    XMPP::Jid target(query.getAttribute("jid"));
    this->unbanUser(target);

    /* send result */
    auto_ptr<Stanza> result(stanza.createIQResult());
    TagGenerator generator;
    generator.openTag("unban");
    generator.addAttribute("xmlns", XMLNS_CHESSD_ADMIN);
    result->children().push_back(generator.getTag());
    this->sendStanza(result.release());
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
    this->sendIq(message.release());
}

void AdminComponent::banUser(const XMPP::PartialJid& user,
                             const string& reason) {
    this->kickUser(user);
    this->banneds.insert(make_pair(user, reason));
    this->updateAcl();
    this->database.queueTransaction(boost::bind(&DatabaseInterface::banUser,
                _1, user.full(), reason));
}

void AdminComponent::unbanUser(const XMPP::PartialJid& user) {
    if(this->banneds.count(user) > 0) {
        this->banneds.erase(user);
        this->database.queueTransaction(boost::bind(&DatabaseInterface::unbanUser,
                    _1, user.full()));
        this->updateAcl();
    }
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
    this->sendIq(message.release());
}

void AdminComponent::onStart() {
    /* Set ejabberd's access rules */
    this->setAccessRules();

    /* Load admin list from the database */
    this->database.execTransaction(boost::bind(&AdminComponent::loadAcl, this, _1));

    /* Update the acl in the jabber server */
    this->updateAcl();
}

void AdminComponent::loadAcl(DatabaseInterface& database) {
    /* read from database */
    std::vector<std::string> admins = database.getAdmins();
    std::vector<pair<std::string, string> > banneds =
        database.searchBannedUsers();

    /* convert types */
    foreach(admin, admins) {
        this->admins.insert(XMPP::PartialJid(*admin));
    }

    foreach(banned, banneds) {
        this->banneds.insert(make_pair(XMPP::PartialJid(banned->first),
                                       banned->second));
    }
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
    /* put banned users in the blocked group */
    foreach(ban, this->banneds) {
        acl += "{acl,blocked,{user,\"" + ban->first.node() + "\",\"" + ban->first.domain() + "\"}},";
    }
    /* remove extra comma */
    if(acl.size() > 1) {
        acl.erase(acl.end() - 1);
    }
    acl += "].";

    generator.addCData(acl);

    message->children().push_back(generator.getTag());

    /* Send the message */
    this->sendIq(message.release());
}
