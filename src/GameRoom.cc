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

#include "GameRoom.hh"
#include "Util/utils.hh"
#include <boost/bind.hpp>
#include <boost/generator_iterator.hpp>

#include "XMPP/Exception.hh"
#include "GameException.hh"

#include "I18n.hh"

#include <iostream>

#define XMLNS_GAME                  "http://c3sl.ufpr.br/chessd#game"
#define XMLNS_GAME_MOVE             "http://c3sl.ufpr.br/chessd#game#move"
#define XMLNS_GAME_RESIGN           "http://c3sl.ufpr.br/chessd#game#resign"
#define XMLNS_GAME_DRAW             "http://c3sl.ufpr.br/chessd#game#draw"
#define XMLNS_GAME_DRAW_DECLINE     "http://c3sl.ufpr.br/chessd#game#draw-decline"
#define XMLNS_GAME_CANCEL           "http://c3sl.ufpr.br/chessd#game#cancel"
#define XMLNS_GAME_CANCEL_DECLINE   "http://c3sl.ufpr.br/chessd#game#cancel-decline"
#define XMLNS_GAME_CANCELED         "http://c3sl.ufpr.br/chessd#game#canceled"
#define XMLNS_GAME_ADJOURN          "http://c3sl.ufpr.br/chessd#game#adjourn"
#define XMLNS_GAME_ADJOURN_DECLINE  "http://c3sl.ufpr.br/chessd#game#adjourn-decline"
#define XMLNS_GAME_START            "http://c3sl.ufpr.br/chessd#game#start"
#define XMLNS_GAME_STATE            "http://c3sl.ufpr.br/chessd#game#state"
#define XMLNS_GAME_END              "http://c3sl.ufpr.br/chessd#game#end"
#define XMLNS_GAME_MOVE             "http://c3sl.ufpr.br/chessd#game#move"

static const char xmlns_table[][64] = {
    XMLNS_GAME_DRAW,
    XMLNS_GAME_CANCEL,
    XMLNS_GAME_ADJOURN,
    XMLNS_GAME_DRAW_DECLINE,
    XMLNS_GAME_CANCEL_DECLINE,
    XMLNS_GAME_ADJOURN_DECLINE
};

static const char result_table[][32] = {
    "winner",
    "drawer",
    "loser"
};

class xmpp_invalid_move : public XMPP::xmpp_exception {
    public:
        xmpp_invalid_move(const std::string& what) : xmpp_exception(what) { }

        virtual XMPP::Stanza* getErrorStanza(XMPP::Stanza* stanza) const {
            return XMPP::Stanza::createErrorStanza
                (stanza, "modify", "invalid-move", this->what(), XMLNS_GAME);
        }
};

GameRoom::GameRoom(
        Game* game,
        const XMPP::Jid& room_jid,
        DatabaseManager& database_manager,
        Threads::Dispatcher& dispatcher,
        const GameRoomHandlers& handlers) :
    Muc(room_jid, game->title(), handlers.send_stanza),
    game(game),
    database_manager(database_manager),
    dispatcher(dispatcher),
    handlers(handlers),
    game_active(true),
    start_time(Util::Timer::now()),
    move_count(0),
    canceled(false)
{

    /* Set features */
    this->disco().features().insert(XMLNS_GAME);

    /* Set game iqs */
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_MOVE);
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_RESIGN);
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_DRAW);
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_DRAW_DECLINE);
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_CANCEL);
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_CANCEL_DECLINE);
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_ADJOURN);
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_ADJOURN_DECLINE);

    this->setIqHandler(boost::bind(&GameRoom::handleState, this, _1),
            XMLNS_GAME_STATE);

    foreach(team, game->players()) {
        this->draw_agreement.insert(player->jid);
        this->cancel_agreement.insert(player->jid);
        this->all_players.insert(player->jid);
    }

    /* set time check */
    this->dispatcher.schedule(boost::bind(&GameRoom::checkTime, this), Util::Timer::now() + 20 * Util::Seconds);

}

GameRoom::~GameRoom() { }

void GameRoom::checkTime() {
    if(this->game_active and this->move_count <= 1 and this->currentTime() > (5 * Util::Minutes)) {
        /* FIXME */
        this->cancelGame();
    }
    
    if(this->game_active and this->game->done(this->currentTime())) {
        this->endGame();
    }
    if(not this->game_active and this->occupants().size() == 0) {
        this->handlers.close_game();
    } else {
        /* set time check */
        this->dispatcher.schedule(boost::bind(&GameRoom::checkTime, this), Util::Timer::now() + 20 * Util::Seconds);
    }
}

XMPP::Stanza* GameRoom::createStateStanza() {
    std::auto_ptr<XMPP::Stanza> stanza(new XMPP::Stanza("iq"));
    XML::TagGenerator generator;

    stanza->subtype() = "set";
    generator.openTag("query");
    generator.addAttribute("xmlns", XMLNS_GAME_STATE);
    generator.addChild(this->game->state(this->currentTime()));
    generator.addChild(this->game->history());
    stanza->children().push_back(generator.getTag());
    return stanza.release();
}

void GameRoom::handleState(const XMPP::Stanza& stanza) {
    if(not this->isOccupant(stanza.from()))
        throw XMPP::not_acceptable("Only occupants are allowed to send queries to the game");

    std::auto_ptr<XMPP::Stanza> result(this->createStateStanza());
    result->to() = stanza.from();
    result->id() = stanza.id();
    result->subtype() = "result";
    this->sendStanza(result.release());
}


void GameRoom::handleGameIq(const XMPP::Stanza& stanza) {
    if(not this->isOccupant(stanza.from()))
        throw XMPP::not_acceptable("Only occupants are allowed to send queries to the game");
    if(not Util::has_key(this->all_players, stanza.from()))
        throw XMPP::not_acceptable("Only players can do that");
    if(not this->game_active)
        throw XMPP::not_acceptable("Game is not active");

    const std::string& xmlns = stanza.findChild("query").getAttribute("xmlns");

    if(xmlns == XMLNS_GAME_MOVE) {
        this->handleMove(stanza);
    } else if(xmlns == XMLNS_GAME_RESIGN) {
        this->handleResign(stanza);
    } else if(xmlns == XMLNS_GAME_DRAW) {
        this->handleDrawAccept(stanza);
    } else if(xmlns == XMLNS_GAME_DRAW_DECLINE) {
        this->handleDrawDecline(stanza);
    } else if(xmlns == XMLNS_GAME_CANCEL) {
        this->handleCancelAccept(stanza);
    } else if(xmlns == XMLNS_GAME_CANCEL_DECLINE) {
        this->handleCancelDecline(stanza);
    } else if(xmlns == XMLNS_GAME_ADJOURN) {
        this->handleAdjournAccept(stanza);
    } else if(xmlns == XMLNS_GAME_ADJOURN_DECLINE) {
        this->handleAdjournDecline(stanza);
    }

    if(this->game->done(this->currentTime())) {
        this->endGame();
    }
}

void GameRoom::notifyState(const XMPP::Jid& user) {
    std::auto_ptr<XMPP::Stanza> stanza(this->createStateStanza());
    stanza->to() = user;
    stanza->subtype() = "set";
    this->sendIq(stanza.release());
}

Util::Time GameRoom::currentTime() {
    return Util::Timer::now() - this->start_time;
}

void GameRoom::handleMove(const XMPP::Stanza& stanza) {
    try {
        const XML::Tag& move = stanza.query().findChild("move");
        std::string move_string = move.getAttribute("long");

        std::auto_ptr<XML::Tag> move_notification(this->game->move(stanza.from(), move_string, this->currentTime()));
        this->sendStanza(stanza.createIQResult());
        this->notifyMove(move_notification.release());
        this->move_count ++;
    } catch (const game_exception& error) {
        throw xmpp_invalid_move(error.what());
    }
}

void GameRoom::handleResign(const XMPP::Stanza& stanza) {
    this->game->resign(stanza.from());
    this->sendStanza(stanza.createIQResult());
}

void GameRoom::handleDrawAccept(const XMPP::Stanza& stanza) {
    this->draw_agreement.agreed(stanza.from());

    this->sendStanza(stanza.createIQResult());

    if(this->draw_agreement.left_count() == 0) {
        this->game->draw();
    } else if(this->draw_agreement.agreed_count() == 1) {
        this->notifyRequest(REQUEST_DRAW, stanza.from());
    }
}

void GameRoom::handleDrawDecline(const XMPP::Stanza& stanza) {
    this->draw_agreement.clear();
    this->sendStanza(stanza.createIQResult());
    this->notifyRequest(REQUEST_DRAW_DECLINE, stanza.from());
}

void GameRoom::handleCancelAccept(const XMPP::Stanza& stanza) {
    this->cancel_agreement.agreed(stanza.from());
    this->sendStanza(stanza.createIQResult());
    if(this->cancel_agreement.left_count()==0) {
        this->cancelGame();
    } else if(this->cancel_agreement.agreed_count() == 1) {
        this->notifyRequest(REQUEST_CANCEL, stanza.from());
    }
}

void GameRoom::handleCancelDecline(const XMPP::Stanza& stanza) {
    this->cancel_agreement.clear();
    this->sendStanza(stanza.createIQResult());
    this->notifyRequest(REQUEST_CANCEL_DECLINE, stanza.from());
}

void GameRoom::handleAdjournAccept(const XMPP::Stanza& stanza) {
    this->adjourn_agreement.agreed(stanza.from());
    this->sendStanza(stanza.createIQResult());
    if(this->adjourn_agreement.left_count()==0) {
        this->adjournGame();
    }
    if(this->adjourn_agreement.agreed_count() == 1) {
        this->notifyRequest(REQUEST_ADJOURN, stanza.from());
    }
}

void GameRoom::handleAdjournDecline(const XMPP::Stanza& stanza) {
    this->adjourn_agreement.clear();
    this->sendStanza(stanza.createIQResult());
    this->notifyRequest(REQUEST_ADJOURN_DECLINE, stanza.from());
}

void GameRoom::notifyRequest(GameRequest request, const XMPP::Jid& requester) {
    XMPP::Stanza stanza("iq");
    XML::Tag* query = new XML::Tag("query");
    stanza.subtype()="set";
    query->setAttribute("xmlns", xmlns_table[request]);
    stanza.children().push_back(query);
    foreach(player, this->all_players) {
        if(*player != requester) {
            stanza.to() = *player;
            this->sendIq(new XMPP::Stanza(stanza));
        }
    }
}

XMPP::Stanza* GameRoom::createResultStanza(const std::string& lang) {
    if(not this->canceled) {
        XML::TagGenerator tag_generator;
        std::auto_ptr<XMPP::Stanza> stanza(new XMPP::Stanza("iq"));
        stanza->subtype() = "set";
        stanza->lang() = lang;
        tag_generator.openTag("query");
        tag_generator.addAttribute("xmlns", XMLNS_GAME_END);
        tag_generator.openTag("reason");
        tag_generator.addCData(i18n.getText(this->result_reason, lang));
        tag_generator.addAttribute("code", Util::to_string(i18n.getTextCode(this->result_reason)));
        tag_generator.closeTag();
        foreach(player, this->players_result) {
            tag_generator.openTag("player");
            tag_generator.addAttribute("jid", player->jid.full());
            tag_generator.addAttribute("score", player->score);
            tag_generator.addAttribute("role", player->role);
            tag_generator.closeTag();
        }
        stanza->children().push_back(tag_generator.getTag());
        return stanza.release();
    } else {
        XML::TagGenerator tag_generator;
        std::auto_ptr<XMPP::Stanza> stanza(new XMPP::Stanza("iq"));
        stanza->subtype() = "set";
        tag_generator.openTag("query");
        tag_generator.addAttribute("xmlns", XMLNS_GAME_CANCELED);
        stanza->children().push_back(tag_generator.getTag());
        return stanza.release();
    }
}

XMPP::Stanza* GameRoom::createMoveStanza(XML::Tag* move_tag) {
    XML::TagGenerator tag_generator;
    XMPP::Stanza* stanza = new XMPP::Stanza("iq");
    stanza->subtype() = "set";
    tag_generator.openTag("query");
    tag_generator.addAttribute("xmlns", XMLNS_GAME_MOVE);
    tag_generator.addChild(move_tag);
    tag_generator.addChild(this->game->state());
    stanza->children().push_back(tag_generator.getTag());
    return stanza;
}

void GameRoom::notifyMove(XML::Tag* move_tag) {
    std::auto_ptr<XMPP::Stanza> stanza(createMoveStanza(move_tag));
    this->broadcastIq(*stanza);
}

void storeResult(GameResult* result, DatabaseInterface& database) {
    RatingDatabase& rating_database = database.rating_database;
    GameDatabase& game_database = database.game_database;
    std::string category = result->category();
    PersistentGame game;
    Rating tmp;
    PersistentRating rating;

    std::map<Player, Rating> ratings;
    std::map<Player, PersistentRating> pratings;
    foreach(player, result->players()) {
        std::string name = player->jid.partial();
        rating = rating_database.getRatingForUpdate(name, category);
        pratings.insert(std::make_pair(player->jid, rating));
        tmp.rating() = rating.rating;
        tmp.volatility() = rating.volatility;
        tmp.wins() = rating.wins;
        tmp.draws() = rating.draws;
        tmp.losses() = rating.defeats;
        ratings.insert(std::make_pair(player->jid, tmp));
    }
    result->updateRating(ratings);
    foreach(it, ratings) {
        rating.rating = it->second.rating();
        rating.wins = it->second.wins();
        rating.draws = it->second.draws();
        rating.defeats = it->second.losses();
        rating.max_rating = pratings[it->first].max_rating;
        rating.max_timestamp = pratings[it->first].max_timestamp;
        if(rating.rating > rating.max_rating) {
            rating.max_rating = rating.rating;
            rating.max_timestamp = boost::posix_time::second_clock::local_time();
        }
        rating_database.setRating(it->first.partial(), category, rating);
    }

    game.players = result->players();
    game.category = category;
    game.time_stamp = boost::posix_time::second_clock::local_time();

    XML::Tag* history = result->history();
    game.history = history->xml();
    delete history;

    game_database.insertGame(game);
    delete result;
}

void GameRoom::setResult(const GameResult& result) {
    this->result_reason = result.end_reason();
    this->players_result = result.players();
}

void GameRoom::broadcastResultStanza() {
    foreach(occupant, this->occupants()) {
        this->notifyResult(occupant->jid());
    }
}

void GameRoom::endGame() {
    std::auto_ptr<GameResult> result(this->game->result());

    this->game_active = false;

    this->setResult(*result);

    this->broadcastResultStanza();

    this->database_manager.queueTransaction(boost::bind(storeResult, result.release(), _1));
}

void GameRoom::adjournGame() {
    //this->game->adjourn();
    this->cancelGame();
    // TODO
}

XMPP::Stanza* createCanceledStanza() {
    XML::TagGenerator tag_generator;
    XMPP::Stanza* stanza = new XMPP::Stanza("iq");
    stanza->subtype() = "set";
    tag_generator.openTag("query");
    tag_generator.addAttribute("xmlns", XMLNS_GAME_CANCELED);
    stanza->children().push_back(tag_generator.getTag());
    return stanza;
}

void GameRoom::cancelGame() {
    this->game_active = false;

    this->canceled = true;
    this->result_reason = "The game was canceled";

    this->broadcastResultStanza();
}

void GameRoom::notifyResult(const XMPP::Jid& user) {
    std::auto_ptr<XMPP::Stanza> stanza(this->createResultStanza(this->occupants().find_jid(user)->lang()));
    stanza->to() = user;
    this->sendIq(stanza.release());
}

void GameRoom::notifyUserStatus(const XMPP::Jid& jid, const std::string& nick, bool available) {
    if(available) {
        this->notifyState(jid);
        if(not this->game_active) {
            this->notifyResult(jid);
        }
    }
}
