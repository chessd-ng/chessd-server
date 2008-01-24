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

#define XMLNS_GAME                  "http://c3sl.ufpr.br/chessd#game"
#define XMLNS_GAME_MOVE             "http://c3sl.ufpr.br/chessd#game#move"
#define XMLNS_GAME_RESIGN           "http://c3sl.ufpr.br/chessd#game#resign"
#define XMLNS_GAME_DRAW             "http://c3sl.ufpr.br/chessd#game#draw"
#define XMLNS_GAME_DRAW_DECLINE     "http://c3sl.ufpr.br/chessd#game#draw-decline"
#define XMLNS_GAME_CANCEL           "http://c3sl.ufpr.br/chessd#game#cancel"
#define XMLNS_GAME_CANCEL_DECLINE   "http://c3sl.ufpr.br/chessd#game#cancel-decline"
#define XMLNS_GAME_ADJOURN          "http://c3sl.ufpr.br/chessd#game#adjourn"
#define XMLNS_GAME_ADJOURN_DECLINE  "http://c3sl.ufpr.br/chessd#game#adjourn-decline"

static const char action_table[][32] = {
	"draw",
	"cancel",
	"adjourn"
};

static const char result_table[][32] = {
	"winner",
	"drawer",
	"loser"
};

class invalid_move : public XMPP::xmpp_exception {
    public:
        invalid_move(const std::string& what) : xmpp_exception(what) { }

        virtual XMPP::Stanza* getErrorStanza(XMPP::Stanza* stanza) const {
            return XMPP::Stanza::createErrorStanza
                (stanza, "modify", "invalid-move", this->what(), XMLNS_GAME);
        }


};

GameRoom::GameRoom(int game_id,
		Game* game,
		const XMPP::Jid& room_jid,
		const GameRoomHandlers& handlers) :
	game_id(game_id),
	game(game),
	room_jid(room_jid),
	handlers(handlers),
	node(handlers.send_stanza, room_jid, game->title(), "game", "game"),
	muc(node, room_jid, boost::bind(&GameRoom::reportUser, this, _1, _2, _3)),
	game_active(true),
    game_state(game->state())
{

	/* Set features */
	this->node.disco().features().insert(XMLNS_GAME);

	/* Set game iqs */
    this->node.setIqHandler(boost::bind(&GameRoom::handleMove, this, _1),
            XMLNS_GAME);
    this->node.setIqHandler(boost::bind(&GameRoom::handleResign, this, _1),
            XMLNS_GAME_RESIGN);
    this->node.setIqHandler(boost::bind(&GameRoom::handleDrawAccept, this, _1),
            XMLNS_GAME_DRAW);
    this->node.setIqHandler(boost::bind(&GameRoom::handleDrawDecline, this, _1),
            XMLNS_GAME_DRAW_DECLINE);
    this->node.setIqHandler(boost::bind(&GameRoom::handleCancelAccept, this, _1),
            XMLNS_GAME_CANCEL);
    this->node.setIqHandler(boost::bind(&GameRoom::handleCancelDecline, this, _1),
            XMLNS_GAME_CANCEL_DECLINE);
    this->node.setIqHandler(boost::bind(&GameRoom::handleAdjournAccept, this, _1),
            XMLNS_GAME_ADJOURN);
    this->node.setIqHandler(boost::bind(&GameRoom::handleAdjournDecline, this, _1),
            XMLNS_GAME_ADJOURN_DECLINE);

	foreach(team, game->teams()) {
		foreach(player, *team) {
			this->draw_agreement.insert(*player);
			this->cancel_agreement.insert(*player);
			this->all_players.insert(*player);
		}
	}

    this->notifyPlayers();
}

GameRoom::~GameRoom() { }

void GameRoom::handleStanza(XMPP::Stanza* _stanza) {
    std::auto_ptr<XMPP::Stanza> stanza(_stanza);
    if(not this->muc.isOccupant(stanza->from()))
        throw XMPP::not_acceptable("Only occupants are allowed to send queries to the game");
	this->node.handleStanza(stanza.release());
}

void GameRoom::notifyPlayers() {
    XMPP::Stanza stanza("iq");
	XML::Tag* query = new XML::Tag("query");
	stanza.subtype()="set";
	query->setAttribute("xmlns", "http://c3sl.ufpr.br/chessd#game");
	query->setAttribute("action", "start");
	stanza.children().push_back(query);
    foreach(player, this->all_players) {
        stanza.to() = *player;
        this->node.sendStanza(new XMPP::Stanza(stanza));
    }
}

/*void GameRoom::handleGame(XMPP::Stanza* stanza) {
	std::string action;
	try {
		if(not this->game_active)
			throw "The game is not active";
		if(not Util::has_key(this->all_players, stanza->from()))
			throw "Only players can do that";
        if(not this->muc.isOccupant(stanza->from()))
			throw "Only occupants are allowed to send queries to the game";
		action = GameProtocol::parseQuery(*stanza->children().tags().begin());
	} catch (const char * msg) {
		stanza = XMPP::Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg);
		this->node.sendStanza(stanza);
		return;
	}
	if(action == "move") {
		this->handleGameMove(stanza);
	} else if(action == "resign") {
		this->handleGameResign(stanza);
	} else if(action == "draw") {
		this->handleGameDrawAccept(stanza);
	} else if(action == "draw-decline") {
		this->handleGameDrawDecline(stanza);
	} else if(action == "cancel") {
		this->handleGameCancelAccept(stanza);
	} else if(action == "cancel-decline") {
		this->handleGameCancelDecline(stanza);
	} else if(action == "adjourn") {
		this->handleGameAdjournAccept(stanza);
	} else if(action == "adjourn-decline") {
		this->handleGameAdjournDecline(stanza);
	} else {
		stanza = XMPP::Stanza::createErrorStanza(stanza, "modify", "bad-request");
		this->node.sendStanza(stanza);
		return;
	}
    check_end_game();
}*/

void GameRoom::check_end_game() {
    if(this->game_active) {
        GameResult* result = this->game->done();
        if(result) {
            this->game_active = false;
            this->notifyResult(*result);
            this->core.endGame(this->game_id, result);
        }
    }
}

void GameRoom::handleMove(XMPP::Stanza* _stanza) {
    std::auto_ptr<XMPP::Stanza> stanza(_stanza);
    if(not Util::has_key(this->all_players, stanza->from()))
        throw XMPP::not_acceptable("Only players can do that");
	try {
        XML::Tag& query = stanza->findChild("query");
        XML::Tag& move = query.findChild("move");
		std::string move_string = move.getAttribute("long");
		this->game->move(stanza->from(), move_string);
		this->node.sendStanza(XMPP::Stanza::createIQResult(stanza.release()));
        this->game_state = std::auto_ptr<XML::Tag>(this->game->state());
		this->notifyMove(move_string);
	} catch (const char* msg) {
        throw invalid_move(msg);
	}
}

void GameRoom::notifyState(const XMPP::Jid& user) {
    XML::TagGenerator generator;
	XMPP::Stanza* stanza = new XMPP::Stanza("iq");
    stanza->to() = user;
    stanza->from() = this->room_jid;
    stanza->subtype() = "set";
    generator.openTag("query");
	generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#game");
	generator.addAttribute("action", "state");
    generator.addChild(this->game_state->clone());
	stanza->children().push_back(generator.getTag());
    this->node.sendIq(stanza);
}

void GameRoom::handleResign(XMPP::Stanza* _stanza) {
    std::auto_ptr<XMPP::Stanza> stanza(_stanza);
    if(not Util::has_key(this->all_players, stanza->from()))
        throw XMPP::not_acceptable("Only players can do that");
    this->game->resign(stanza->from());
    this->node.sendStanza(XMPP::Stanza::createIQResult(stanza.release()));
}

void GameRoom::handleDrawAccept(XMPP::Stanza* _stanza) {
    std::auto_ptr<XMPP::Stanza> stanza(_stanza);
    if(not Util::has_key(this->all_players, stanza->from()))
        throw XMPP::not_acceptable("Only players can do that");
	XMPP::Jid requester = stanza->from();
	this->draw_agreement.agreed(requester);
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza.release()));
	if(this->draw_agreement.left_count() == 0) {
        this->game->draw();
	}
	if(this->draw_agreement.agreed_count() == 1) {
		this->notifyRequest(REQUEST_DRAW, requester);
	}
}

void GameRoom::handleDrawDecline(XMPP::Stanza* stanza) {
	this->draw_agreement.clear();
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza));
}

void GameRoom::handleCancelAccept(XMPP::Stanza* _stanza) {
    std::auto_ptr<XMPP::Stanza> stanza(_stanza);
    if(not Util::has_key(this->all_players, stanza->from()))
        throw XMPP::not_acceptable("Only players can do that");
	XMPP::Jid requester = stanza->from();
	this->cancel_agreement.agreed(requester);
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza.release()));
	if(this->cancel_agreement.left_count()==0) {
		this->cancelGame();
	}
	if(this->cancel_agreement.agreed_count() == 1) {
		this->notifyRequest(REQUEST_CANCEL, requester);
	}
}

void GameRoom::handleCancelDecline(XMPP::Stanza* _stanza) {
    std::auto_ptr<XMPP::Stanza> stanza(_stanza);
    if(not Util::has_key(this->all_players, stanza->from()))
        throw XMPP::not_acceptable("Only players can do that");
	this->cancel_agreement.clear();
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza.release()));
}

void GameRoom::handleAdjournAccept(XMPP::Stanza* _stanza) {
    std::auto_ptr<XMPP::Stanza> stanza(_stanza);
    if(not Util::has_key(this->all_players, stanza->from()))
        throw XMPP::not_acceptable("Only players can do that");
	XMPP::Jid requester = stanza->from();
	this->adjourn_agreement.agreed(requester);
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza.release()));
	if(this->adjourn_agreement.left_count()==0) {
		this->adjournGame();
	}
	if(this->adjourn_agreement.agreed_count() == 1) {
		this->notifyRequest(REQUEST_CANCEL, requester);
	}
}

void GameRoom::handleAdjournDecline(XMPP::Stanza* _stanza) {
    std::auto_ptr<XMPP::Stanza> stanza(_stanza);
    if(not Util::has_key(this->all_players, stanza->from()))
        throw XMPP::not_acceptable("Only players can do that");
	this->adjourn_agreement.clear();
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza.release()));
}

void GameRoom::notifyRequest(GameRequest request, const XMPP::Jid& requester) {
	std::string action = action_table[request];
	XMPP::Stanza stanza("iq");
	XML::Tag* query = new XML::Tag("query");
	stanza.subtype()="set";
	query->setAttribute("xmlns", "http://c3sl.ufpr.br/chessd#game");
	query->setAttribute("action", action);
	stanza.children().push_back(query);
	foreach(player, this->all_players) {
		if(*player != requester) {
			stanza.to() = *player;
			this->node.sendIq(new XMPP::Stanza(stanza));
		}
	}
}

XMPP::Stanza* createResultStanza(const GameResult& result) {
	XML::TagGenerator tag_generator;
	XMPP::Stanza* stanza = new XMPP::Stanza("iq");
	stanza->subtype() = "set";
	tag_generator.openTag("query");
	tag_generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#game");
	tag_generator.addAttribute("action", "end");
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
	this->muc.broadcastIq(stanza);
}

XMPP::Stanza* createMoveStanza(XML::Tag* state, const std::string& long_move) {
	XML::TagGenerator tag_generator;
	XMPP::Stanza* stanza = new XMPP::Stanza("iq");
	stanza->subtype() = "set";
	tag_generator.openTag("query");
	tag_generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#game");
	tag_generator.addAttribute("action", "move");
	tag_generator.openTag("move");
	tag_generator.addAttribute("long", long_move);
	tag_generator.closeTag();
	tag_generator.addChild(state);
	stanza->children().push_back(tag_generator.getTag());
	return stanza;
}

void GameRoom::notifyMove(const std::string& long_move) {
	XMPP::Stanza* stanza = createMoveStanza(this->game_state->clone(), long_move);
	this->muc.broadcastIq(stanza);
}

void GameRoom::adjournGame() {
	this->game->adjourn();
	GameResult* result = this->game->done();
	this->core.adjournGame(this->game_id, result);
}

void GameRoom::cancelGame() {
	this->core.cancelGame(this->game_id);
}

void GameRoom::reportUser(const XMPP::Jid& jid, const std::string& nick, bool available) {
    if(not available and not this->game_active and this->muc.occupants().size() == 0) {
        this->handlers.close_game();
    }
    if(available) {
        this->notifyState(jid);
    }
}
