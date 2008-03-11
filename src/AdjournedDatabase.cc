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

#include "AdjournedDatabase.hh"

#include "Util/utils.hh"

AdjournedDatabase::AdjournedDatabase(pqxx::work& work) : work(work) 
{
}

void AdjournedDatabase::insertGame(const PersistentAdjourned& game)
{
    int game_id;

    pqxx::result r = work.exec("SELECT nextval('adjourned_game_id_seq')");

    r[0][0].to(game_id);

    std::string query;

    query =
            " INSERT INTO adjourned_games VALUES"
            "   ( " + pqxx::to_string(game_id) + "" +
            "   ,'" + this->work.esc(game.category) + "'" +
            "   , " + pqxx::to_string(Util::ptime_to_time_t(game.time_stamp)) + "" +
            "   ,'" + this->work.esc(game.history) + "')";

    this->work.exec(query);
 
    foreach(player, game.players) {
        query =
            " INSERT INTO adjourned_game_players VALUES"
            "   ( " + pqxx::to_string(game_id) + "" +
            "   ,'" + this->work.esc(player->partial()) + "')";
        work.exec(query);
    }
}

std::string AdjournedDatabase::getGameHistory(int game_id) {
    std::string query = 
        "SELECT history FROM adjourned_games WHERE game_id = " + Util::to_string(game_id);

    pqxx::result result = this->work.exec(query);

    if(result.empty()) {
        throw adjourned_game_not_found("The requested game is not in the database");
    }

    return result[0][0].c_str();
}

std::vector<PersistentAdjourned> AdjournedDatabase::searchGames(
                const std::vector<std::string> players,
                int offset,
                int max_results)
{
    std::string select =
            " SELECT adjourned_games.game_id, category, time_stamp ";
    std::string from =
            " FROM adjourned_games ";
    std::string where;

    for(int i=0;i<int(players.size());++i) {
        std::string id_str = Util::to_string(i);
        from += ", adjourned_game_players g" + id_str + " ";
        if(i > 0)
            where += " and ";
        else
            where = " WHERE ";
        where += " adjourned_games.game_id = g" + id_str
            + ".game_id and g" + id_str + ".username = '"
            + this->work.esc(players[i]) + "' ";
    }
    std::string query = select + from + where + " limit " + Util::to_string(max_results) + " offset " + Util::to_string(offset);

    pqxx::result result = this->work.exec(query);

    std::vector<PersistentAdjourned> games;

    foreach(r, result) {
        PersistentAdjourned game;
        time_t t;
        r->at("game_id").to(game.id);
        r->at("time_stamp").to(t);
        game.time_stamp = boost::posix_time::from_time_t(t);
        game.category = r->at("category").c_str();
        std::string query =
            "SELECT username FROM adjourned_game_players WHERE game_id = "
            + pqxx::to_string(game.id);
        pqxx::result result = this->work.exec(query);
        foreach(r, result) {
            Player player;
            player = XMPP::Jid(r->at("username").c_str());
            game.players.push_back(player);
        }
        games.push_back(game);
    }
    return games;
}

void AdjournedDatabase::eraseGame(int game_id) {

    std::string query;

    query =
            " DELETE FROM adjourned_games WHERE "
            "   game_id= " + pqxx::to_string(game_id);

    this->work.exec(query);

    query =
            " DELETE FROM adjourned_game_players WHERE "
            "   game_id= " + pqxx::to_string(game_id);

    this->work.exec(query);

}
