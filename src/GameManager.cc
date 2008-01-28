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

#include "GameManager.hh"
#include "Util/utils.hh"

using namespace XMPP;
using namespace std;

GameManager::GameManager(
        const XML::Tag& config,
        DatabaseManager& database_manager,
        const XMPP::ErrorHandler& handle_error) :
    ComponentBase(config, "Game Manager"),
    node_name(config.getAttribute("node_name")),
    database_manager(database_manager),
    handle_error(handle_error),
    game_ids(0)
{

}

GameManager::~GameManager() {
}

void GameManager::onClose() {

}

void GameManager::onError(const string& msg) {
    this->handle_error(msg);
}

void GameManager::createGame(Game* game) {
	this->dispatcher.queue(boost::bind(&GameManager::_insertGame, this, game));
}

void GameManager::_insertGame(Game* game) {
    int game_id = this->game_ids++;
	int room_id = this->room_ids.acquireID();
	Jid room_jid = Jid("game_" + Util::to_string(room_id), this->node_name);
	/* Create the game room */
	GameRoom* game_room = new GameRoom(game_id, game, room_jid, this->database_manager,
			GameRoomHandlers(boost::bind(&ComponentBase::sendStanza, this, _1),
				boost::bind(&GameManager::closeGameRoom, this, room_id)));
	/* Register the new jabber node */
	this->root_node.setNodeHandler(room_jid.node(),
			boost::bind(&GameRoom::handleStanza, game_room, _1));
	/* Add the new jabber node to the disco */
	this->root_node.disco().items().insert(new XMPP::DiscoItem(room_jid.node(),
				room_jid));
	game_rooms.insert(room_id, game_room);
}

void GameManager::closeGameRoom(int room_id) {
	this->dispatcher.queue(boost::bind(&GameManager::_closeGameRoom, this, room_id));
}

void GameManager::_closeGameRoom(int room_id) {
	this->room_ids.releaseID(room_id);
	Jid room_jid = Jid("game_" + Util::to_string(room_id), this->node_name);
	this->root_node.removeNodeHandler(room_jid.node());
	this->root_node.disco().items().erase(room_jid);
	this->game_rooms.erase(room_id);
}
