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

#include "GameRoom.hh"
#include "Util/utils.hh"
#include <boost/bind.hpp>
#include <boost/generator_iterator.hpp>

#include "XMPP/Exception.hh"
#include "GameException.hh"

#define XMLNS_GAME                  "http://c3sl.ufpr.br/chessd#game"
#define XMLNS_GAME_MOVE             "http://c3sl.ufpr.br/chessd#game#move"
#define XMLNS_GAME_RESIGN           "http://c3sl.ufpr.br/chessd#game#resign"
#define XMLNS_GAME_DRAW             "http://c3sl.ufpr.br/chessd#game#draw"
#define XMLNS_GAME_DRAW_DECLINE     "http://c3sl.ufpr.br/chessd#game#draw-decline"
#define XMLNS_GAME_CANCEL           "http://c3sl.ufpr.br/chessd#game#cancel"
#define XMLNS_GAME_CANCEL_DECLINE   "http://c3sl.ufpr.br/chessd#game#cancel-decline"
#define XMLNS_GAME_ADJOURN          "http://c3sl.ufpr.br/chessd#game#adjourn"
#define XMLNS_GAME_ADJOURN_DECLINE  "http://c3sl.ufpr.br/chessd#game#adjourn-decline"
#define XMLNS_GAME_START            "http://c3sl.ufpr.br/chessd#game#start"
#define XMLNS_GAME_STATE            "http://c3sl.ufpr.br/chessd#game#state"
#define XMLNS_GAME_END              "http://c3sl.ufpr.br/chessd#game#end"
#define XMLNS_GAME_MOVE             "http://c3sl.ufpr.br/chessd#game#move"
#define XMLNS_GAME_CANCELED         "http://c3sl.ufpr.br/chessd#game#canceled"

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
        const GameRoomHandlers& handlers) :
    Muc(room_jid, game->title(), handlers.send_stanza),
    game(game),
    database_manager(database_manager),
    handlers(handlers),
    game_active(true),
    start_time(Util::Timer::Now())
{

    /* Set features */
    this->disco().features().insert(XMLNS_GAME);

    /* Set game iqs */
    this->setIqHandler(boost::bind(&GameRoom::handleMove, this, _1),
            XMLNS_GAME_MOVE);
    this->setIqHandler(boost::bind(&GameRoom::handleResign, this, _1),
            XMLNS_GAME_RESIGN);
    this->setIqHandler(boost::bind(&GameRoom::handleDrawAccept, this, _1),
            XMLNS_GAME_DRAW);
    this->setIqHandler(boost::bind(&GameRoom::handleDrawDecline, this, _1),
            XMLNS_GAME_DRAW_DECLINE);
    this->setIqHandler(boost::bind(&GameRoom::handleCancelAccept, this, _1),
            XMLNS_GAME_CANCEL);
    this->setIqHandler(boost::bind(&GameRoom::handleCancelDecline, this, _1),
            XMLNS_GAME_CANCEL_DECLINE);
    this->setIqHandler(boost::bind(&GameRoom::handleAdjournAccept, this, _1),
            XMLNS_GAME_ADJOURN);
    this->setIqHandler(boost::bind(&GameRoom::handleAdjournDecline, this, _1),
            XMLNS_GAME_ADJOURN_DECLINE);

    foreach(team, game->teams()) {
        foreach(player, *team) {
            this->draw_agreement.insert(*player);
            this->cancel_agreement.insert(*player);
            this->all_players.insert(*player);
        }
    }

}

GameRoom::~GameRoom() { }

void GameRoom::checkGameIQ(const XMPP::Jid& from) {
    if(not this->isOccupant(from))
        throw XMPP::not_acceptable("Only occupants are allowed to send queries to the game");
    if(not Util::has_key(this->all_players, from))
        throw XMPP::not_acceptable("Only players can do that");
    if(not this->game_active)
        throw XMPP::not_acceptable("Game is not active");
}

void GameRoom::handleMove(const XMPP::Stanza& stanza) {
    this->checkGameIQ(stanza.from());
    try {
        const XML::Tag& move = stanza.query().findChild("move");
        std::string move_string = move.getAttribute("long");

        this->game->move(stanza.from(), move_string, Util::Timer::Now() - this->start_time);
        this->sendStanza(stanza.createIQResult());
        this->notifyMove(move_string);
        if(this->game->done()) {
            this->endGame();
        }
    } catch (const game_exception& error) {
        throw xmpp_invalid_move(error.what());
    }
}

void GameRoom::notifyState(const XMPP::Jid& user) {
    XML::TagGenerator generator;
    XMPP::Stanza* stanza = new XMPP::Stanza("iq");

    stanza->to() = user;
    stanza->from() = this->room_jid;
    stanza->subtype() = "set";
    generator.openTag("query");
    generator.addAttribute("xmlns", XMLNS_GAME_STATE);
    generator.addChild(this->gameState());
    stanza->children().push_back(generator.getTag());
    this->sendIq(stanza);
}

void GameRoom::handleResign(const XMPP::Stanza& stanza) {
    this->checkGameIQ(stanza.from());

    this->game->resign(stanza.from());
    this->sendStanza(stanza.createIQResult());

    this->endGame();
}

void GameRoom::handleDrawAccept(const XMPP::Stanza& stanza) {
    this->checkGameIQ(stanza.from());

    this->draw_agreement.agreed(stanza.from());

    this->sendStanza(stanza.createIQResult());

    if(this->draw_agreement.left_count() == 0) {
        this->game->draw();
        this->endGame();
    } else if(this->draw_agreement.agreed_count() == 1) {
        this->notifyRequest(REQUEST_DRAW, stanza.from());
    }
}

void GameRoom::handleDrawDecline(const XMPP::Stanza& stanza) {
    this->checkGameIQ(stanza.from());

    this->draw_agreement.clear();
    this->sendStanza(stanza.createIQResult());
    this->notifyRequest(REQUEST_DRAW_DECLINE, stanza.from());
}

void GameRoom::handleCancelAccept(const XMPP::Stanza& stanza) {
    this->checkGameIQ(stanza.from());

    this->cancel_agreement.agreed(stanza.from());
    this->sendStanza(stanza.createIQResult());
    if(this->cancel_agreement.left_count()==0) {
        this->cancelGame();
    }
    if(this->cancel_agreement.agreed_count() == 1) {
        this->notifyRequest(REQUEST_CANCEL, stanza.from());
    }
}

void GameRoom::handleCancelDecline(const XMPP::Stanza& stanza) {
    this->checkGameIQ(stanza.from());

    this->cancel_agreement.clear();
    this->sendStanza(stanza.createIQResult());
    this->notifyRequest(REQUEST_CANCEL_DECLINE, stanza.from());
}

void GameRoom::handleAdjournAccept(const XMPP::Stanza& stanza) {
    this->checkGameIQ(stanza.from());

    this->adjourn_agreement.agreed(stanza.from());
    this->sendStanza(stanza.createIQResult());
    if(this->adjourn_agreement.left_count()==0) {
        this->adjournGame();
    }
    if(this->adjourn_agreement.agreed_count() == 1) {
        this->notifyRequest(REQUEST_CANCEL, stanza.from());
    }
}

void GameRoom::handleAdjournDecline(const XMPP::Stanza& stanza) {
    this->checkGameIQ(stanza.from());

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

XMPP::Stanza* createResultStanza(const GameResult& result) {
    XML::TagGenerator tag_generator;
    XMPP::Stanza* stanza = new XMPP::Stanza("iq");
    stanza->subtype() = "set";
    tag_generator.openTag("query");
    tag_generator.addAttribute("xmlns", XMLNS_GAME_END);
    tag_generator.openTag("reason");
    tag_generator.addCData(result.end_reason());
    tag_generator.closeTag();
    foreach(team_result, result.results()) {
        foreach(player, team_result->first) {
            tag_generator.openTag("player");
            tag_generator.addAttribute("jid", player->full());
            tag_generator.addAttribute("result", result_table[team_result->second]);
            tag_generator.closeTag();
        }
    }
    stanza->children().push_back(tag_generator.getTag());
    return stanza;
}

void GameRoom::notifyResult(const GameResult& result) {
    XMPP::Stanza* stanza = createResultStanza(result);
    this->broadcastIq(stanza);
}

XMPP::Stanza* createMoveStanza(XML::Tag* state, const std::string& long_move) {
    XML::TagGenerator tag_generator;
    XMPP::Stanza* stanza = new XMPP::Stanza("iq");
    stanza->subtype() = "set";
    tag_generator.openTag("query");
    tag_generator.addAttribute("xmlns", XMLNS_GAME_MOVE);
    tag_generator.openTag("move");
    tag_generator.addAttribute("long", long_move);
    tag_generator.closeTag();
    tag_generator.addChild(state);
    stanza->children().push_back(tag_generator.getTag());
    return stanza;
}

XML::Tag* GameRoom::gameState() {
    return this->game->state(Util::Timer::Now() - this->start_time);
}

void GameRoom::notifyMove(const std::string& long_move) {
    XMPP::Stanza* stanza = createMoveStanza(this->gameState(), long_move);
    this->broadcastIq(stanza);
}

void storeResult(GameResult* result, DatabaseInterface& database) {
    RatingDatabase& rating_database = database.rating_database;
    GameDatabase& game_database = database.game_database;
    std::string category = result->category();
    PersistentGame game;

    std::map<Player, Rating> tmp;
    foreach(player, result->players()) {
        std::string name = player->parcial();
        Rating rating = rating_database.getRatingForUpdate(name, category);
        tmp.insert(std::make_pair(*player, rating));
        game.players.push_back(name);
    }
    result->updateRating(tmp);
    foreach(it, tmp) {
        rating_database.setRating(it->first.parcial(), category, it->second);
    }

    game.category = category;
    game.result = result->end_reason();
    game.time_stamp = time(NULL);

    XML::Tag* history = result->history();
    game.history = history->xml();
    delete history;

    game_database.insertGame(game);
    delete result;
}

void GameRoom::endGame() {
    std::auto_ptr<GameResult> result(this->game->result());
    this->game_active = false;
    this->notifyResult(*result);
    //this->database_manager.queueTransaction(boost::bind(storeResult.release(), result, _1));
}

void GameRoom::adjournGame() {
    this->game->adjourn();
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
    this->broadcastIq(createCanceledStanza());
}

void GameRoom::notifyUserStatus(const XMPP::Jid& jid, const std::string& nick, bool available) {
    if(not available and not this->game_active and this->occupants().size() == 0) {
        this->handlers.close_game();
    }
    if(available) {
        this->notifyState(jid);
    }
}
