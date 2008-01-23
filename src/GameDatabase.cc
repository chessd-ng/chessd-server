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
