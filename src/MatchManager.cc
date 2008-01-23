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


using namespace std;
using namespace XML;
using namespace XMPP;

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
    this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#match");

    /* Set match iqs */
    this->root_node.setIqHandler(boost::bind(&MatchManager::handleMatch, this, _1),
            "http://c3sl.ufpr.br/chessd#match");

    /* FIXME */
    /* this should not be here */
    this->insertMatchRule(new MatchRuleStandard);

    MatchProtocol::init("protocol");
}

void MatchManager::insertMatchRule(MatchRule* rule) {
    std::string category = rule->getCategory();
    this->rules.insert(category, rule);
}

MatchManager::~MatchManager() {
}

void MatchManager::handleMatch(Stanza* _stanza) {
    std::auto_ptr<Stanza> stanza(_stanza);
    std::auto_ptr<Query> query;
    try {
        query = std::auto_ptr<Query>(new Query(move(*stanza)));
    } catch (const char* msg) {
        this->sendStanza(
                Stanza::createErrorStanza(
                    stanza.release(),
                    "cancel",
                    "bad-request",
                    msg));
        return;
    }
    if(query->action() == "offer") {
        this->handleMatchOffer(query.release());
    } else if(query->action() == "accept") {
        this->handleMatchAccept(query.release());
    } else if(query->action() == "decline") {
        this->handleMatchDecline(query.release());
    } else {
        this->sendStanza(
                Stanza::createErrorStanza(
                    Query::createStanza(move(*query)),
                    "cancel",
                    "bad-request"));
    }
}

void MatchManager::handleMatchOffer(Query* _query) {
    auto_ptr<Match> match;
    auto_ptr<Query> query(_query);
    Jid requester = query->from();
    try {
        /* parse message */
        XML::Tag& offer = query->children().tags().front();
        if(not offer.hasAttribute("category"))
            throw "Invalid message";
        const std::string& category = offer.getAttribute("category");
        if(not Util::has_key(this->rules, category))
            throw "Invalid category";
        match = auto_ptr<Match>(this->rules.find(category)->second->checkOffer(offer, this->teams));
        bool valid = false;
        foreach(player, match->players()) {
            if(not this->roster.isUserAvailable(*player))
                throw "User is not available";
            if(*player == query->from())
                valid = true;
        }
        if(not valid)
            throw "Invalid request";

        /* check if there are no repeated users in the match */
        foreach(it1, match->players()) {
            foreach_it(it2, Util::next(it1), match->players().end()) {
                if(it1->parcialCompare(*it2))
                    throw "Repeated players on the match";
            }
        }

        TagGenerator generator;
        int id = this->match_db.insertMatch(match.release());
        Stanza* stanza = new Stanza("iq");
        stanza->subtype() = "result";
        stanza->id() = query->id();
        stanza->to() = query->from();
        stanza->from() = query->to();

        generator.openTag("query");
        generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#match");
        generator.addAttribute("action", "offer");
        generator.openTag("match");
        generator.addAttribute("id", Util::to_string(id));

        stanza->children().push_back(generator.getTag());

        this->match_db.acceptMatch(id, requester);

        this->sendStanza(stanza);

        this->notifyMatchOffer(id, requester);

    } catch (const char* msg) {
        this->sendStanza(
                Stanza::createErrorStanza(
                    Query::createStanza(move(*query)),
                    "cancel",
                    "bad-request",
                    msg));
    }
}

void MatchManager::notifyMatchOffer(int id, const Jid& requester) {
    const Match& match = this->match_db.getMatch(id);
    Stanza stanza("iq");
    stanza.subtype() = "set";
    Tag* tag = match.notification();
    tag->setAttribute("id", Util::to_string(id));
    stanza.children().push_back(tag);
    foreach(player, match.players()) {
        if(*player != requester) {
            stanza.to() = *player;
            this->root_node.sendIq(new XMPP::Stanza(stanza));
        }
    }
}

void MatchManager::handleMatchAccept(Query* _query) {
    std::auto_ptr<Query> query(_query);
    try {
        cout << "incoming accept" << endl;
        if(query->children().tags().begin() ==  query->children().tags().end())
            throw "Invalid message";
        XML::Tag& match = query->children().tags().front();
        if(match.name() != "match" or not match.hasAttribute("id"))
            throw "Invalid message";
        int id = Util::parse_string<int>(match.getAttribute("id"));
        this->match_db.acceptMatch(id, query->from());
        this->sendStanza(
                Stanza::createIQResult(
                    Query::createStanza(move(*query))));
        if(this->match_db.isDone(id)) {
            this->closeMatch(id, true);
        }
    } catch (const char* msg) {
        this->sendStanza(
                Stanza::createErrorStanza(
                    Query::createStanza(move(*query)),
                    "cancel",
                    "bad-request",
                    msg));
    }
}

void MatchManager::handleMatchDecline(Query* _query) {
    std::auto_ptr<Query> query(_query);
    try {
        XML::Tag& match = query->children().tags().front();
        if(match.name() != "match" or not match.hasAttribute("id"))
            throw "Invalid message";
        int id = Util::parse_string<int>(match.getAttribute("id"));
        if(not this->match_db.hasPlayer(id, query->from()))
            throw "Invalid id";
        this->sendStanza(
                Stanza::createIQResult(
                    Query::createStanza(move(*query))));
        this->closeMatch(id, false);
    } catch (const char* msg) {
        this->sendStanza(
                Stanza::createErrorStanza(
                    Query::createStanza(move(*query)),
                    "cancel",
                    "bad-request",
                    msg));
    }
}

void MatchManager::closeMatch(int id, bool accepted) {
    Match* match = this->match_db.closeMatch(id);
    if(accepted) {
        this->core_interface.startGame(match->createGame());
    }
    this->notifyMatchResult(match, id, accepted);
}

void MatchManager::notifyMatchResult(Match* match, int id, bool accepted) {
    Stanza stanza("iq");
    stanza.subtype() = "set";
    stanza.children().push_back(MatchProtocol::notifyMatchResult(*match, id, accepted));
    foreach(player, match->players()) {
        stanza.to() = *player;
        this->root_node.sendIq(new XMPP::Stanza(stanza));
    }
    delete match;
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
