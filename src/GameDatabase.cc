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
            "   ,'" + this->work.esc(game.result) + "'" +
            "   , " + pqxx::to_string(game.time_stamp) + "" +
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
