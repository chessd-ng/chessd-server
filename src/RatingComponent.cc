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
#include "RatingComponent.hh"
#include "Util/utils.hh"

#include "Exception.hh"
#include "XMPP/Exception.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

RatingComponent::RatingComponent(
        const XML::Tag& config,
        const XMPP::ErrorHandler& error_handler,
        DatabaseManager& database) :
    ComponentBase(config, "Rating Component"),
    error_handler(error_handler),
    database(database)
{

    /* Set features */
    this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#info");

    /* Set rating iqs */
    this->root_node.setIqHandler(boost::bind(&RatingComponent::handleRating, this, _1),
            "http://c3sl.ufpr.br/chessd#info");

}

RatingComponent::~RatingComponent() {
}

void RatingComponent::handleRating(const Stanza& stanza) {
    try {
        const Tag& query = stanza.query();

        /* check if the format is correct */
        foreach(tag, query.tags()) {
            if(tag->name() == "rating") {
                if(not tag->hasAttribute("category") or not tag->hasAttribute("jid"))
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

void RatingComponent::fetchRating(const Stanza& stanza, DatabaseInterface& database) {
    RatingDatabase& rating_database = database.rating_database;
    XML::TagGenerator generator;
    const Tag& query = stanza.findChild("query");
    generator.openTag("iq");
    generator.addAttribute("to", stanza.from().full());
    generator.addAttribute("from", stanza.to().full());
    generator.addAttribute("id", stanza.id());
    generator.addAttribute("type", "result");
    generator.openTag("query");
    generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#rating");
    foreach(tag, query.tags()) {
        if(tag->name() == "rating") {
            Rating rating = rating_database.getRating
                (tag->getAttribute("jid"), tag->getAttribute("category"));
            generator.openTag("rating");
            generator.addAttribute("jid", tag->getAttribute("jid"));
            generator.addAttribute("category", tag->getAttribute("category"));
            generator.addAttribute("rating", Util::to_string(rating.rating()));
            generator.addAttribute("wins", Util::to_string(rating.wins()));
            generator.addAttribute("draws", Util::to_string(rating.draws()));
            generator.addAttribute("losses", Util::to_string(rating.losses()));
            generator.closeTag();
        } else if(tag->name() == "type") {
            generator.openTag("type");
            generator.addAttribute("jid", tag->getAttribute("jid"));
            generator.addAttribute("type", "user");
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
