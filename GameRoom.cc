#include "GameRoom.hh"
#include "GameProtocol.hh"
#include "Util/utils.hh"
#include <boost/bind.hpp>
#include <boost/generator_iterator.hpp>


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

GameRoom::GameRoom(int game_id,
		Game* game,
		const XMPP::Jid& room_jid,
		const GameRoomHandlers& handlers) :
	game_id(game_id),
	game(game),
	room_jid(room_jid),
	handlers(handlers),
	node(handlers.send_stanza, room_jid, game->title(), "game", "game"),
	muc(node, room_jid),
	game_active(true)
{

	/* Set features */
	this->node.disco().features().insert("http://c3sl.ufpr.br/chessd#game");

	/* Set game iqs */
	this->node.setIqHandler(boost::bind(&GameRoom::handleGame, this, _1),
			"http://c3sl.ufpr.br/chessd#game");

	foreach(team, game->teams()) {
		foreach(player, *team) {
			this->draw_agreement.insert(*player);
			this->cancel_agreement.insert(*player);
			this->all_players.insert(*player);
		}
	}
}

GameRoom::~GameRoom() { }

void GameRoom::handleStanza(XMPP::Stanza* stanza) {
	this->node.handleStanza(stanza);
}

void GameRoom::handleGame(XMPP::Stanza* stanza) {
	std::string action;
	try {
		if(not this->game_active)
			throw "The game is not active";
		action = GameProtocol::parseQuery(stanza->children().front());
		if(not Util::has_key(this->all_players, stanza->from()))
			throw "You are not playing the game";
	} catch (const char * msg) {
		stanza = XMPP::Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg);
		this->node.sendStanza(stanza);
		return;
	}
	if(action == "move") {
		this->handleGameMove(stanza);
	} else if(action == "resign") {
		this->handleGameResign(stanza);
	} else if(action == "draw-accept") {
		this->handleGameDrawAccept(stanza);
	} else if(action == "draw-decline") {
		this->handleGameDrawDecline(stanza);
	} else if(action == "cancel-accept") {
		this->handleGameCancelAccept(stanza);
	} else if(action == "cancel-decline") {
		this->handleGameCancelDecline(stanza);
	} else if(action == "adjourn-accept") {
		this->handleGameAdjournAccept(stanza);
	} else if(action == "adjourn-decline") {
		this->handleGameAdjournDecline(stanza);
	} else {
		stanza = XMPP::Stanza::createErrorStanza(stanza, "modify", "bad-request");
		this->node.sendStanza(stanza);
		return;
	}
}

void GameRoom::check_end_game() {
	GameResult* result = this->game->done();
	if(result) {
		this->game_active = false;
		this->notifyResult(*result);
		this->core.endGame(this->game_id, result);
	}
}

void GameRoom::handleGameMove(XMPP::Stanza* stanza) {
	try {
		std::string move = GameProtocol::parseMove(stanza->children().front());
		this->game->move(stanza->from(), move);
		stanza = XMPP::Stanza::createIQResult(stanza);
		this->node.sendStanza(stanza);
		this->notifyMove(move);
	} catch (const char* msg) {
		stanza = XMPP::Stanza::createErrorStanza(stanza, "modify", "bad-request", msg);
		this->node.sendStanza(stanza);
		return;
	}
}

void GameRoom::handleGameResign(XMPP::Stanza* stanza) {
	this->game->resign(stanza->from());
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza));
}

void GameRoom::handleGameDrawAccept(XMPP::Stanza* stanza) {
	XMPP::Jid requester = stanza->from();
	this->draw_agreement.agreed(requester);
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza));
	if(this->draw_agreement.left_count()==0) {
		this->game->draw();
	}
	if(this->draw_agreement.agreed_count() == 1) {
		this->notifyRequest(REQUEST_DRAW, requester);
	}
}

void GameRoom::handleGameDrawDecline(XMPP::Stanza* stanza) {
	this->draw_agreement.clear();
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza));
}

void GameRoom::handleGameCancelAccept(XMPP::Stanza* stanza) {
	XMPP::Jid requester = stanza->from();
	this->cancel_agreement.agreed(requester);
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza));
	if(this->cancel_agreement.left_count()==0) {
		this->cancelGame();
	}
	if(this->cancel_agreement.agreed_count() == 1) {
		this->notifyRequest(REQUEST_CANCEL, requester);
	}
}

void GameRoom::handleGameCancelDecline(XMPP::Stanza* stanza) {
	this->cancel_agreement.clear();
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza));
}

void GameRoom::handleGameAdjournAccept(XMPP::Stanza* stanza) {
	XMPP::Jid requester = stanza->from();
	this->adjourn_agreement.agreed(requester);
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza));
	if(this->adjourn_agreement.left_count()==0) {
		this->adjournGame();
	}
	if(this->adjourn_agreement.agreed_count() == 1) {
		this->notifyRequest(REQUEST_CANCEL, requester);
	}
}

void GameRoom::handleGameAdjournDecline(XMPP::Stanza* stanza) {
	this->adjourn_agreement.clear();
	this->node.sendStanza(XMPP::Stanza::createIQResult(stanza));
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
			this->node.sendIq(stanza.clone());
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
	stanza->children().push_back(tag_generator.closeTag());
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
	tag_generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#game");
	tag_generator.addAttribute("action", "move");
	tag_generator.openTag("move");
	tag_generator.addAttribute("long", long_move);
	tag_generator.closeTag();
	tag_generator.addChild(state);
	stanza->children().push_back(tag_generator.closeTag());
	return stanza;
}

void GameRoom::notifyMove(const std::string& long_move) {
	XMPP::Stanza* stanza = createMoveStanza(this->game->state(), long_move);
	this->muc.broadcastIq(stanza);
}

void GameRoom::adjournGame() {
	this->game->adjourn();
	GameResult* result = this->game->done();
	this->core.adjournGame(this->game_id, result);
	this->game_active = false;
}

void GameRoom::cancelGame() {
	//this->game->cancel();
	this->core.cancelGame(this->game_id);
	this->game_active = false;
}
