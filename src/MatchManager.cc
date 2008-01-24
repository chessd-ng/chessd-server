/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
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
#include <iostream>
#include "MatchManager.hh"
#include "Threads/Task.hh"
#include "Util/utils.hh"

#include "MatchStandard.hh"

#include "XMPP/Exception.hh"
#include "Exception.hh"

#define XMLNS_MATCH         "http://c3sl.ufpr.br/chessd#match"
#define XMLNS_MATCH_OFFER   "http://c3sl.ufpr.br/chessd#match#offer"
#define XMLNS_MATCH_ACCEPT  "http://c3sl.ufpr.br/chessd#match#accept"
#define XMLNS_MATCH_DECLINE "http://c3sl.ufpr.br/chessd#match#decline"

using namespace std;
using namespace XML;
using namespace XMPP;

class match_error : public XMPP::xmpp_exception {
    public:
        match_error(const std::string& what) :
            xmpp_exception(what) { }

        virtual Stanza* getErrorStanza(Stanza* stanza) const {
            return Stanza::createErrorStanza
                (stanza, "modify", "bad-request", this->what());
        }
};



MatchManager::MatchManager(const XML::Tag& config, const XMPP::ErrorHandler& handleError) :
    ComponentBase(config, "Match Manager"),
    roster(boost::bind(&ComponentBase::sendStanza, this, _1),
            boost::bind(&MatchManager::notifyUserStatus, this, _1, _2)),
    handleError(handleError)
{

    /* Set the root_node to route presence stanzas to the roster */
    this->root_node.setPresenceHandler(boost::bind(&Roster::handlePresence, &this->roster, _1));

    /* Set features */
    this->root_node.disco().features().insert("presence");
    this->root_node.disco().features().insert(XMLNS_MATCH);

    /* Set match iqs */
    this->root_node.setIqHandler(boost::bind(&MatchManager::handleOffer, this, _1),
            XMLNS_MATCH_OFFER);
    this->root_node.setIqHandler(boost::bind(&MatchManager::handleAccept, this, _1),
            XMLNS_MATCH_ACCEPT);
    this->root_node.setIqHandler(boost::bind(&MatchManager::handleDecline, this, _1),
            XMLNS_MATCH_DECLINE);

    /* FIXME */
    /* this should not be here */
    this->insertMatchRule(new MatchRuleStandard);
}

void MatchManager::insertMatchRule(MatchRule* rule) {
    std::string category = rule->getCategory();
    this->rules.insert(category, rule);
}

MatchManager::~MatchManager() {
}

void MatchManager::handleOffer(Stanza* _stanza) {
    auto_ptr<Match> match;
    auto_ptr<Stanza> stanza(_stanza);
    try {
        Jid requester = stanza->from();
        /* parse message */
        XML::Tag& query = stanza->findChild("query");
        XML::Tag& offer = query.findChild("match");
        const std::string& category = offer.getAttribute("category");
        if(not Util::has_key(this->rules, category))
            throw match_error("This category is not available");
        match = auto_ptr<Match>(this->rules.find(category)->second->checkOffer(offer, this->teams));
        bool valid = false;
        foreach(player, match->players()) {
            if(not this->roster.isUserAvailable(*player))
                throw match_error("User is not available");
            if(*player == stanza->from())
                valid = true;
        }
        if(not valid)
            throw match_error("You must be among the players in the match");

        /* check if there are no repeated users in the match */
        foreach(it1, match->players()) {
            foreach_it(it2, Util::next(it1), match->players().end()) {
                if(it1->parcialCompare(*it2))
                    throw match_error("Users must not repeat in the match");
            }
        }

        TagGenerator generator;
        int id = this->match_db.insertMatch(match.release());
        Stanza* resp = new Stanza("iq");
        resp->subtype() = "result";
        resp->id() = stanza->id();
        resp->to() = stanza->from();
        resp->from() = stanza->to();

        generator.openTag("query");
        generator.addAttribute("xmlns", XMLNS_MATCH_OFFER);
        generator.openTag("match");
        generator.addAttribute("id", Util::to_string(id));

        resp->children().push_back(generator.getTag());

        this->match_db.acceptMatch(id, requester);

        this->sendStanza(resp);

        this->notifyOffer(id, requester);
    } catch (const XML::xml_error& error) {
        throw XMPP::bad_request(error.what());
    }

}

void MatchManager::notifyOffer(int id, const Jid& requester) {
    const Match& match = this->match_db.getMatch(id);
    TagGenerator generator;
    XML::Tag* tag = 0;
    Stanza stanza("iq");
    stanza.subtype() = "set";
    generator.openTag("query");
    generator.addAttribute("xmlns", XMLNS_MATCH_OFFER);
    tag = match.notification();
    tag->setAttribute("id", Util::to_string(id));
    generator.addChild(tag);
    stanza.children().push_back(generator.getTag());
    foreach(player, match.players()) {
        if(*player != requester) {
            stanza.to() = *player;
            this->root_node.sendIq(new XMPP::Stanza(stanza));
        }
    }
}

void MatchManager::handleAccept(Stanza* _stanza) {
    std::auto_ptr<Stanza> stanza(_stanza);
    try {
        XML::Tag& query = stanza->findChild("query");
        XML::Tag& match = query.findChild("match");
        int id = Util::parse_string<int>(match.getAttribute("id"));
        this->match_db.acceptMatch(id, stanza->from());
        this->sendStanza(Stanza::createIQResult(stanza.release()));
        if(this->match_db.isDone(id)) {
            this->closeMatch(id, true);
        }
    } catch (const XML::xml_error& error) {
        throw XMPP::bad_request("Invalid format");
    } catch (const user_error& error) {
        throw match_error(error.what());
    }
}

void MatchManager::handleDecline(Stanza* _stanza) {
    std::auto_ptr<Stanza> stanza(_stanza);
    try {
        XML::Tag& query = stanza->findChild("query");
        XML::Tag& match = query.findChild("match");
        int id = Util::parse_string<int>(match.getAttribute("id"));
        if(not this->match_db.hasPlayer(id, stanza->from()))
            throw match_error("Invalid match id");
        this->sendStanza(Stanza::createIQResult(stanza.release()));
        this->closeMatch(id, false);
    } catch (const XML::xml_error& error) {
        throw bad_request("Invalid format");
    } catch (const user_error& error) {
        throw match_error(error.what());
    }
}

void MatchManager::closeMatch(int id, bool accepted) {
    std::auto_ptr<Match> match(this->match_db.closeMatch(id));
    if(accepted) {
        this->core_interface.startGame(match->createGame());
    }
    this->notifyResult(*match, id, accepted);
}

void MatchManager::notifyResult(const Match& match, int id, bool accepted) {
	XML::TagGenerator generator;
    Stanza stanza("iq");

    stanza.subtype() = "set";

	generator.openTag("query");
	generator.addAttribute("xmlns", accepted ? XMLNS_MATCH_ACCEPT : XMLNS_MATCH_DECLINE);
	generator.openTag("match");
	generator.addAttribute("id", Util::to_string<int>(id));

    stanza.children().push_back(generator.getTag());
    foreach(player, match.players()) {
        stanza.to() = *player;
        this->root_node.sendIq(new XMPP::Stanza(stanza));
    }
}

void MatchManager::notifyUserStatus(const XMPP::Jid& jid, bool available) {
    if(not available) {
        set<int> matchs = this->match_db.getPlayerMatchs(jid);
        foreach(id, matchs) {
            this->closeMatch(*id, false);
        }
    }
}

void MatchManager::onError(const std::string& error) {
    this->handleError(error);
}

void MatchManager::onClose() {
    vector<int> matchs = this->match_db.getActiveMatchs();
    foreach(match_id, matchs) {
        this->closeMatch(*match_id, false);
    }
}
