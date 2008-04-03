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
#include "RatingComponent.hh"
#include "Util/utils.hh"
#include "Util/Date.hh"

#include "Exception.hh"
#include "XMPP/Exception.hh"

#include "HistoryProcess.hh"

#include "I18n.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

#define XMLNS_CHESSD_INFO "http://c3sl.ufpr.br/chessd#info"
#define XMLNS_CHESSD_GAME_SEARCH "http://c3sl.ufpr.br/chessd#search_game"
#define XMLNS_CHESSD_GAME_FETCH "http://c3sl.ufpr.br/chessd#fetch_game"


RatingComponent::RatingComponent(
        const XML::Tag& config,
        const XMPP::ErrorHandler& error_handler,
        DatabaseManager& database) :
    ComponentBase(config, "Rating Component"),
    error_handler(error_handler),
    database(database)
{

    /* Set features */
    this->root_node.disco().features().insert(XMLNS_CHESSD_INFO);

    /* Set iqs */
    this->root_node.setIqHandler(boost::bind(&RatingComponent::handleRating, this, _1),
            XMLNS_CHESSD_INFO);
    this->root_node.setIqHandler(boost::bind(&RatingComponent::handleSearchGame, this, _1),
            XMLNS_CHESSD_GAME_SEARCH);
    this->root_node.setIqHandler(boost::bind(&RatingComponent::handleFetchGame, this, _1),
            XMLNS_CHESSD_GAME_FETCH);
}

RatingComponent::~RatingComponent() {
}

void RatingComponent::handleRating(const Stanza& stanza) {
    try {
        const Tag& query = stanza.query();

        /* check if the format is correct */
        foreach(tag, query.tags()) {
            if(tag->name() == "rating") {
                if(not tag->hasAttribute("jid"))
                    throw XMPP::bad_request("Invalid format");
            } else if(tag->name() == "type") {
                if(not tag->hasAttribute("jid"))
                    throw XMPP::bad_request("Invalid format");
            } else {
                throw XMPP::bad_request("Invalid format");
            }
        }

        /* execute the transaction */
        this->database.queueTransaction(boost::bind(&RatingComponent::fetchRating, this, stanza, _1));
    } catch (const XML::xml_error& error) {
        throw XMPP::bad_request("Invalid format");
    }
}

void RatingComponent::handleSearchGame(const Stanza& stanza) {
    try {
        const Tag& query = stanza.query();

        /* check if the format is correct */
        const Tag& search_tag = query.findChild("search");
        foreach(child, search_tag.tags()) {
            if(child->name() == "player") {
                if(not child->hasAttribute("jid"))
                    throw XMPP::bad_request("Invalid format");
            } else {
                throw XMPP::bad_request("Invalid format");
            }
        }
        /* execute the transaction */
        this->database.queueTransaction(boost::bind(&RatingComponent::searchGame, this, stanza, _1));
    } catch (const XML::xml_error& error) {
        throw XMPP::bad_request("Invalid format");
    }
}

void RatingComponent::handleFetchGame(const Stanza& stanza) {
    try {
        /* execute the transaction */
        this->database.queueTransaction(boost::bind(&RatingComponent::fetchGame, this, stanza, _1));
    } catch (const XML::xml_error& error) {
        throw XMPP::bad_request("Invalid format");
    }
}

void RatingComponent::searchGame(const Stanza& stanza, DatabaseInterface& database) {
    XML::TagGenerator generator;
    std::vector<string> players;
    int max_results = 50;
    int offset = 0;

    /* Parse request */
    const Tag& query = stanza.findChild("query");
    const Tag& search_tag = query.findChild("search");
    if(search_tag.hasAttribute("offset")) {
        offset = Util::parse_string<int>(search_tag.getAttribute("offset"));
    }
    if(search_tag.hasAttribute("results")) {
        max_results = min(50, Util::parse_string<int>(search_tag.getAttribute("results")));
    }
    foreach(field, search_tag.tags()) {
        if(field->name() == "player") {
            players.push_back(field->getAttribute("jid"));
        } else {
            /* for later use */
        }
    }
    
    /* Search in the database */
    std::vector<PersistentGame> games = database.searchGames(players, offset, max_results);

    /* Create result */
    generator.openTag("iq");
    generator.addAttribute("to", stanza.from().full());
    generator.addAttribute("from", stanza.to().full());
    generator.addAttribute("id", stanza.id());
    generator.addAttribute("type", "result");
    generator.openTag("query");
    generator.addAttribute("xmlns", XMLNS_CHESSD_GAME_SEARCH);

    foreach(game, games) {
        generator.openTag("game");
        generator.addAttribute("id", Util::to_string(game->id));
        generator.addAttribute("category", game->category);
        generator.addAttribute("time_stamp", Util::ptime_to_xmpp_date_time(game->time_stamp));
        generator.openTag("result");
        generator.addCData(i18n.getText(game->result,stanza.lang()));
        foreach(player, game->players) {
            generator.openTag("player");
            generator.addAttribute("jid", player->jid.partial());
            generator.addAttribute("role", player->role);
            generator.addAttribute("score", player->score);
            generator.closeTag();
        }
        generator.closeTag();
    }

    /* Send result */
    this->sendStanza(new XMPP::Stanza(generator.getTag()));
}

void RatingComponent::fetchGame(const Stanza& stanza, DatabaseInterface& database) {
    try {
        try {
            XML::TagGenerator generator;

            /* Parse request */
            const Tag& query = stanza.findChild("query");
            const Tag& game_tag = query.findChild("game");
            int game_id = Util::parse_string<int>(game_tag.getAttribute("id"));
            std::string game_history = database.getGameHistory(game_id);

            /* Create result */
            generator.openTag("iq");
            generator.addAttribute("to", stanza.from().full());
            generator.addAttribute("from", stanza.to().full());
            generator.addAttribute("id", stanza.id());
            generator.addAttribute("type", "result");
            generator.openTag("query");
            generator.addAttribute("xmlns", XMLNS_CHESSD_GAME_FETCH);
            generator.addChild(HistoryProcess::generate(XML::parseXmlString(game_history)));

            /* send the result to the player */
            this->sendStanza(new XMPP::Stanza(generator.getTag()));
        } catch(const XML::xml_error& error) {
            throw XMPP::bad_request("Invalid format");
        } catch(const game_not_found&) {
            throw XMPP::bad_request("Game not found");
        }
    } catch (const XMPP::xmpp_exception& error) {
        this->sendStanza(error.getErrorStanza(new Stanza(stanza)));
    }
}

void RatingComponent::fetchRating(const Stanza& stanza, DatabaseInterface& database) {
    XML::TagGenerator generator;
    const Tag& query = stanza.findChild("query");
    std::string category, jid;

    /* create message */
    generator.openTag("iq");
    generator.addAttribute("to", stanza.from().full());
    generator.addAttribute("from", stanza.to().full());
    generator.addAttribute("id", stanza.id());
    generator.addAttribute("type", "result");
    generator.openTag("query");
    generator.addAttribute("xmlns", XMLNS_CHESSD_INFO);

    /* for each requested info, consult the database */
    foreach(tag, query.tags()) {
        if(tag->name() == "rating") {
            /* parse tag */
            category = tag->hasAttribute("category") ? tag->getAttribute("category") : "";
            jid = tag->getAttribute("jid");

            /* consult the database */
            std::vector<std::pair<std::string, PersistentRating> > ratings = database.getRatings(
                        jid, category);

            /* add result to the message */
            foreach(rating, ratings) {
                generator.openTag("rating");
                generator.addAttribute("jid", jid);
                generator.addAttribute("category", rating->first);
                generator.addAttribute("rating", Util::to_string(rating->second.rating));
                generator.addAttribute("wins", Util::to_string(rating->second.wins));
                generator.addAttribute("draws", Util::to_string(rating->second.draws));
                generator.addAttribute("losses", Util::to_string(rating->second.defeats));
                generator.addAttribute("max_rating", Util::to_string(rating->second.max_rating));
                generator.addAttribute("max_timestamp", Util::ptime_to_xmpp_date_time(rating->second.max_timestamp));
                generator.closeTag();
            }
        } else if(tag->name() == "type") {
            /* parse message */
            jid = tag->getAttribute("jid");

            /* consult the database */
            std::string type = database.getUserType(jid);

            /* create the tag */
            generator.openTag("type");
            generator.addAttribute("jid", jid);
            generator.addAttribute("type", type);
            generator.closeTag();
        }
    }
    generator.closeTag();
    generator.closeTag();
    this->sendStanza(new XMPP::Stanza(generator.getTag()));
}

void RatingComponent::onError(const std::string& error) {
    this->error_handler(error);
}

void RatingComponent::onClose() {
}
