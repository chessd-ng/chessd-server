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

#include "ServerCore.hh"
#include "Util/utils.hh"

#include "MatchManager.hh"
#include "AdminComponent.hh"
#include "ProfileManager.hh"
#include "TourneyManager.hh"

using namespace XMPP;
using namespace std;

#define XMLNS_CHESSD_GAME "http://c3sl.ufpr.br/chessd#game"


ServerCore::ServerCore(
        const XML::Tag& config,
        DatabaseManager& database_manager,
        const XMPP::ErrorHandler& handle_error) :
    ComponentBase(config, "Chessd"),
    node_name(config.getAttribute("component_name")),
    database_manager(database_manager),
    handle_error(handle_error),
    game_ids(0)
{
    /* set a presence handler */
    this->root_node.setPresenceHandler(
            boost::bind(&ServerCore::handlePresence, this, _1));

    /* instaciate the match module */
    this->modules.push_back(new MatchManager(config,
                *this, database_manager,
                boost::bind(&Node::sendStanza, &this->root_node, _1)));

    /* instaciate the admin module */
    this->modules.push_back(new AdminComponent(
                config.getAttribute("server_name"),
                database_manager,
                boost::bind(&Node::sendStanza, &this->root_node, _1)));

    /* instaciate the profile module */
    this->modules.push_back(new ProfileManager(
                database_manager,
                boost::bind(&Node::sendStanza, &this->root_node, _1)));

    /* instaciate the tourney manager module */
    this->modules.push_back(new TourneyManager(
                *this,
                database_manager,
                boost::bind(&Node::sendStanza, &this->root_node, _1)));

    /* register the modules's namespaces */
    foreach(module, this->modules) {
        vector<string> namespaces = module->namespaces();
        foreach(ns, namespaces) {
            this->root_node.setIqHandler(
                    boost::bind(&ServerModule::handleStanza, &*module, _1), *ns);
            this->root_node.disco().features().insert(*ns);
        }
    }
    this->root_node.disco().features().insert(XMLNS_CHESSD_GAME);
}

ServerCore::~ServerCore() {
}

void ServerCore::onConnect() {
    foreach(module, this->modules) {
        module->start();
    }
}

void ServerCore::onClose() {

}

void ServerCore::onError(const string& msg) {
    this->handle_error(msg);
}

void ServerCore::createGame(Game* game,
        const OnGameStart& on_game_start,
        const OnGameEnd& on_game_end) {
    this->dispatcher.queue(boost::bind(&ServerCore::_createGame,
                this, game, on_game_start, on_game_end));
}

void ServerCore::_createGame(Game* game,
        const OnGameStart& on_game_start,
        const OnGameEnd& on_game_end) {
    GameId game_id = game_ids ++;
    Jid room_jid = Jid("game_" + Util::to_string(game_id), this->node_name);
    /* Create the game room */
    GameRoom* game_room = new GameRoom(game, room_jid, this->database_manager,
            this->dispatcher,
            GameRoomHandlers(boost::bind(&ComponentBase::sendStanza, this, _1),
                boost::bind(&ServerCore::closeGame, this, game_id),
                boost::bind(&ServerCore::hideGame, this, game_id),
                on_game_end));
    /* Register the node */
    this->root_node.setNodeHandler(room_jid.node(),
            boost::bind(&GameRoom::handleStanza, game_room, _1));
    /* Add the new jabber node to the disco */
    this->root_node.disco().items().insert(new XMPP::DiscoItem(game->title(),
                room_jid));
    game_rooms.insert(game_id, game_room);
    /* Notify game ceation */
    if(not on_game_start.empty())
        on_game_start(room_jid);
}

void ServerCore::hideGame(GameId room_id) {
    this->dispatcher.queue(boost::bind(&ServerCore::_hideGame, this, room_id));
}

void ServerCore::_hideGame(GameId room_id) {
    Jid room_jid = Jid("game_" + Util::to_string(room_id), this->node_name);
    /* erase the room from the disco items
     * so it is not visible anymore */
    this->root_node.disco().items().erase(room_jid);
}

void ServerCore::closeGame(GameId room_id) {
    this->dispatcher.queue(boost::bind(&ServerCore::_closeGame, this, room_id));
}

void ServerCore::_closeGame(GameId room_id) {
    Jid room_jid = Jid("game_" + Util::to_string(room_id), this->node_name);
    this->root_node.removeNodeHandler(room_jid.node());
    this->root_node.disco().items().erase(room_jid);
    this->game_rooms.erase(room_id);
}

void ServerCore::handlePresence(const Stanza& stanza) {
    Jid user = stanza.from();
    bool available = false;
    if(stanza.subtype().empty()) {
        available = true;
    } else if(stanza.subtype() == "unavailable") {
        available = false;
    } else {
        return;
    }
    this->users_status[user].available = available;
    this->notifyUserStatus(user, this->users_status[user]);
}

void ServerCore::notifyUserStatus(const Jid& user_name, const UserStatus& status) {
    foreach(module, this->modules) {
        module->notifyUserStatus(user_name, status);
    }
}
