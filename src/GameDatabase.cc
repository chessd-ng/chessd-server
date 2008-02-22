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

#include <iostream>

#include "GameDatabase.hh"

#include "Util/utils.hh"

GameDatabase::GameDatabase(pqxx::work& work) : work(work) 
{
}

void GameDatabase::insertGame(const PersistentGame& game)
{
    int game_id;

    pqxx::result r = work.exec("SELECT nextval('game_id_seq')");

    r[0][0].to(game_id);

    std::string query;

    query =
            " INSERT INTO games VALUES"
            "   ( " + pqxx::to_string(game_id) + "" +
            "   ,'" + this->work.esc(game.category) + "'" +
            "   , " + pqxx::to_string(game.time_stamp) + "" +
            "   ,'" + this->work.esc(game.result) + "'" +
            "   ,'" + this->work.esc(game.history) + "')";

    this->work.exec(query);
 
    foreach(player, game.players) {
        query =
            " INSERT INTO game_players VALUES"
            "   ( " + pqxx::to_string(game_id) + "" +
            "   ,'" + this->work.esc(*player) + "')";
        work.exec(query);
    }
}

std::string GameDatabase::getGameHistory(int game_id) {
    std::string query = 
        "SELECT history FROM games WHERE game_id = " + Util::to_string(game_id);

    pqxx::result result = this->work.exec(query);

    if(result.empty()) {
        throw game_not_found("The requested game is not in the database");
    }

    return result[0][0].c_str();
}

std::vector<PersistentGame> GameDatabase::searchGames(
                const std::vector<std::string> players,
                int offset,
                int max_results)
{
    std::string select =
            " SELECT games.game_id, category, result_string, time_stamp ";
    std::string from =
            " FROM games ";
    std::string where =
            " WHERE ";

    for(int i=0;i<int(players.size());++i) {
        std::string id_str = Util::to_string(i);
        from += ", game_players g" + id_str + " ";
        if(i > 0)
            where += " and ";
        where += " games.game_id = g" + id_str
            + ".game_id and g" + id_str + ".username = '"
            + this->work.esc(players[i]) + "' ";
    }
    std::string query = select + from + where + " limit " + Util::to_string(max_results) + " offset " + Util::to_string(offset);

    pqxx::result result = this->work.exec(query);

    std::vector<PersistentGame> games;

    foreach(r, result) {
        PersistentGame game;
        r->at("game_id").to(game.id);
        r->at("time_stamp").to(game.time_stamp);
        game.category = r->at("category").c_str();
        game.result = r->at("result_string").c_str();
        std::string query =
            "SELECT username FROM game_players WHERE game_id = "
            + Util::to_string(game.id);
        pqxx::result result = this->work.exec(query);
        foreach(r, result) {
            game.players.push_back(r->at("username").c_str());
        }
        games.push_back(game);
    }
    return games;
}
