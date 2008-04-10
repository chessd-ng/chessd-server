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

static const char game_end_type_table[][64] = {
    "normal",
    "canceled",
    "adjourned"
};

class xmpp_invalid_move : public XMPP::xmpp_exception {
    public:
        xmpp_invalid_move(const std::string& what) : xmpp_exception(what) { }

        virtual XMPP::Stanza* getErrorStanza(XMPP::Stanza* stanza) const {
            return XMPP::Stanza::createErrorStanza
                (stanza, "modify", "invalid-move", this->what(), XMLNS_GAME);
        }
};

class xmpp_game_over : public XMPP::xmpp_exception {
    public:
        xmpp_game_over(const std::string& what) : xmpp_exception(what) { }

        virtual XMPP::Stanza* getErrorStanza(XMPP::Stanza* stanza) const {
            return XMPP::Stanza::createErrorStanza
                (stanza, "modify", "game-over", this->what(), XMLNS_GAME);
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
    move_count(0)
{

    /* Set features */
    this->disco().features().insert(XMLNS_GAME);

    /* Set extended info */
    XML::TagGenerator generator;
    generator.openTag("game");
    generator.addAttribute("category", game->category());
    this->disco().setExtendedInfo(generator.getTag());

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
    this->setIqHandler(boost::bind(&GameRoom::handleGameIq, this, _1),
            XMLNS_GAME_STATE);

    /* init agreements */
    foreach(player, game->players()) {
        this->draw_agreement.insert(*player);
        this->cancel_agreement.insert(*player);
        this->adjourn_agreement.insert(*player);
        this->all_players.insert(*player);
    }

    /* set time check */
    this->dispatcher.schedule(boost::bind(&GameRoom::checkTime, this), Util::Timer::now() + 20 * Util::Seconds);

}

GameRoom::~GameRoom() { }

void GameRoom::checkTime() {
    /* cancel games for inactivity */
    if(this->game_active and this->move_count <= 1 and this->currentTime() > (5 * Util::Minutes)) {
        /* FIXME */
        this->endGame(END_TYPE_CANCELED);
    }
    
    /* check whether the time is over */
    if(this->game_active and this->game->done(this->currentTime())) {
        this->endGame(END_TYPE_NORMAL);
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
    std::auto_ptr<XMPP::Stanza> result(this->createStateStanza());
    result->to() = stanza.from();
    result->id() = stanza.id();
    result->subtype() = "result";
    this->sendStanza(result.release());
}


void GameRoom::handleGameIq(const XMPP::Stanza& stanza) {
    const std::string& xmlns = stanza.findChild("query").getAttribute("xmlns");

    /* is a room occupant? */
    if(not this->isOccupant(stanza.from()))
        throw XMPP::not_acceptable("Only occupants are allowed to send queries to the game");
    
    /* is a state iq? */
    if(xmlns == XMLNS_GAME_STATE) {
        this->handleState(stanza);
        return;
    }

    /* is the sender a player and the game is active? */
    if(not Util::has_key(this->all_players, stanza.from()))
        throw XMPP::not_acceptable("Only players can do that");
    if(not this->game_active)
        throw XMPP::not_acceptable("Game is not active");

    /* check xmlns and call the proper handler */
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

    /* check if the game is over */
    if(this->game->done(this->currentTime())) {
        this->endGame(END_TYPE_NORMAL);
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
        /* parse message */
        const XML::Tag& move = stanza.query().findChild("move");
        std::string move_string = move.getAttribute("long");
        
        /* make the move */
        std::auto_ptr<XML::Tag> move_notification(this->game->move(stanza.from(), move_string, this->currentTime()));
        this->move_count ++;

        /* send the iq result */
        this->sendStanza(stanza.createIQResult());

        /* notify the players */
        this->notifyMove(move_notification.release());
    } catch (const time_over& error) {
        this->endGame(END_TYPE_NORMAL);
        throw xmpp_game_over("Time over");
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

    /* check if all players agreed on a draw */
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

    /* check if all players agreed on canceling the game */
    if(this->cancel_agreement.left_count()==0) {
        this->endGame(END_TYPE_CANCELED);
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

    /* check whether all players agreed on adjourning the game */
    if(this->adjourn_agreement.left_count()==0) {
        this->endGame(END_TYPE_ADJOURNED);
    } else if(this->adjourn_agreement.agreed_count() == 1) {
        this->notifyRequest(REQUEST_ADJOURN, stanza.from());
    }
}

void GameRoom::handleAdjournDecline(const XMPP::Stanza& stanza) {
    this->adjourn_agreement.clear();
    this->sendStanza(stanza.createIQResult());
    this->notifyRequest(REQUEST_ADJOURN_DECLINE, stanza.from());
}

void GameRoom::notifyRequest(GameRequest request, const XMPP::Jid& requester) {
    /* create message */
    XMPP::Stanza stanza("iq");
    XML::Tag* query = new XML::Tag("query");
    stanza.subtype()="set";
    query->setAttribute("xmlns", xmlns_table[request]);
    stanza.children().push_back(query);
    /* send message to occupants */
    foreach(player, this->all_players) {
        if(*player != requester) {
            stanza.to() = *player;
            this->sendIq(new XMPP::Stanza(stanza));
        }
    }
}

XMPP::Stanza* GameRoom::createResultStanza(const std::string& lang) {

    XML::TagGenerator tag_generator;
    std::auto_ptr<XMPP::Stanza> stanza(new XMPP::Stanza("iq"));
    stanza->subtype() = "set";
    stanza->lang() = lang;
    tag_generator.openTag("query");
    tag_generator.addAttribute("xmlns", XMLNS_GAME_END);
    tag_generator.openTag("reason");
    tag_generator.addCData(i18n.getText(this->result_reason, lang));
    tag_generator.addAttribute("type", Util::to_string(game_end_type_table[this->end_type]));
    tag_generator.closeTag();

    /* results are available only if the game ended normally */
    if(this->end_type == END_TYPE_NORMAL) {
        foreach(player, this->players_result) {
            tag_generator.openTag("player");
            tag_generator.addAttribute("jid", player->jid.full());
            tag_generator.addAttribute("score", player->score);
            tag_generator.addAttribute("role", player->role);
            tag_generator.closeTag();
        }
    }
    stanza->children().push_back(tag_generator.getTag());
    return stanza.release();
}

XMPP::Stanza* GameRoom::createMoveStanza(XML::Tag* move_tag) {
    XML::TagGenerator tag_generator;
    XMPP::Stanza* stanza = new XMPP::Stanza("iq");
    stanza->subtype() = "set";
    tag_generator.openTag("query");
    tag_generator.addAttribute("xmlns", XMLNS_GAME_MOVE);
    tag_generator.addChild(move_tag);
    tag_generator.addChild(this->game->state(this->currentTime()));
    stanza->children().push_back(tag_generator.getTag());
    return stanza;
}

void GameRoom::notifyMove(XML::Tag* move_tag) {
    /* create message */
    std::auto_ptr<XMPP::Stanza> stanza(createMoveStanza(move_tag));
    /* send to all occupants */
    this->broadcastIq(*stanza);
}

void storeResult(GameResult* result, DatabaseInterface& database) {
    database.insertGameResult(*result);
    delete result;
}

void storeAdjourned(AdjournedGame* adj_game, DatabaseInterface& database) {
    PersistentAdjournedGame game;
    std::auto_ptr<XML::Tag> history(adj_game->history());

    /* set game values */
    game.category = adj_game->category();
    game.players = adj_game->players();
    game.history = history->xml();
    game.time_stamp = boost::posix_time::second_clock::local_time();
    
    /* insert game to the database */
    database.insertAdjournedGame(game);

    delete adj_game;
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

void GameRoom::endGame(GameEndType type) {
    /* set game status */
    this->game_active = false;
    this->end_type = type;

    /* hide the game from other users */
    this->handlers.hide_game();

    if(type == END_TYPE_NORMAL) {
        /* take the game result */
        std::auto_ptr<GameResult> result(this->game->result());

        /* set result */
        this->setResult(*result);

        /* store game in the database */
        this->database_manager.queueTransaction(boost::bind(storeResult, result.release(), _1));
    } else if(type == END_TYPE_CANCELED) {
        /* set result */
        this->result_reason = "The game was canceled";
    } else if(type == END_TYPE_ADJOURNED) {
        /* get the adjourned game */
        std::auto_ptr<AdjournedGame> adj_game(this->game->adjourn(this->currentTime()));

        /* set result */
        this->result_reason = "The game was adjourned";

        /* store game */
        this->database_manager.queueTransaction(boost::bind(storeAdjourned, adj_game.release(), _1));
    }

    /* notify result */
    this->broadcastResultStanza();
}

void GameRoom::notifyResult(const XMPP::Jid& user) {
    std::auto_ptr<XMPP::Stanza> stanza(this->createResultStanza(this->occupants().find_jid(user)->lang()));
    stanza->to() = user;
    this->sendIq(stanza.release());
}

void GameRoom::notifyUserStatus(const XMPP::Jid& jid, const std::string& nick, bool available) {
    if(available) {
        /* send to the user the game status */
        this->notifyState(jid);
        if(not this->game_active) {
            this->notifyResult(jid);
        }
    }
}
