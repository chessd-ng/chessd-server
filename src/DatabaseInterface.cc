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

#include "DatabaseInterface.hh"

#include "Util/Date.hh"

using namespace boost::posix_time;

DatabaseInterface::DatabaseInterface(pqxx::work& work) :
    work(work) { }

int DatabaseInterface::getUserId(const std::string& _username) {
    int user_id;
    pqxx::result result;
    std::string username = this->work.esc(_username);
    
    /* prepare the query */
    std::string query = "SELECT user_id "
                        "FROM users "
                        "WHERE user_name ='" + username + "'";
    
    /* execute te query */
    result = this->work.exec(query);

    /* if user is not in the database, create it */
    if(result.empty()) {
        /* prepare query */
        query = "INSERT INTO users (user_name) "
                "VALUES ('" + username + "') "
                "RETURNING user_id";
        /* execute query */
        result = this->work.exec(query);
    }

    /* get the user id from te result */
    result[0]["user_id"].to(user_id);

    return user_id;
}

std::string DatabaseInterface::getUsername(int user_id) {
    std::string query;
    std::string username;
    pqxx::result result;

    /* prepare the query */
    query = "SELECT user_name "
            "FROM users "
            "WHERE user_id=" + pqxx::to_string(user_id);

    /* execute the query */
    result = this->work.exec(query);

    /* if the user is not found, throw an exception */
    if(result.empty()) {
        throw user_not_found("User id not found");
    }

    /* get the username from the result */
    result[0]["user_name"].to(username);

    return username;
}

std::vector<std::pair<std::string, PersistentRating> >
    DatabaseInterface::getRatings(const std::string& username,
                                  const std::string& category)
{
    std::string query;
    std::vector<std::pair<std::string, PersistentRating> > ratings;
    int user_id = this->getUserId(username);

    /* prepare SQL query */
    /* if no category is given, take all of them */
    if(category.empty()) {
        query =
            "SELECT * "
            " FROM player_rating "
            " WHERE user_id='" + pqxx::to_string(user_id) + "'";
    } else {
        query = 
            "SELECT * "
            " FROM player_rating "
            " WHERE user_id='" + pqxx::to_string(user_id) + "' AND";
            "       category='" + this->work.esc(category) + "'";
    }
    
    /* execute query */
    pqxx::result result = work.exec(query);

    /* read results */
    foreach(r, result) {
        PersistentRating rating;
        std::string category;
        time_t t;
        r->at("category").to(category);
        r->at("rating").to(rating.rating);
        r->at("volatility").to(rating.volatility);
        r->at("wins").to(rating.wins);
        r->at("defeats").to(rating.defeats);
        r->at("draws").to(rating.draws);
        r->at("max_rating").to(rating.max_rating);
        r->at("max_timestamp").to(t);
        r->at("last_game").to(rating.last_game);
        rating.max_timestamp = from_time_t(t);
        ratings.push_back(std::make_pair(category, rating));
    }
    return ratings;
}

std::string DatabaseInterface::getUserType(const std::string& user)
{
    /* prepare query */
    std::string query =
        "SELECT user_type"
        " FROM users"
        " WHERE user_name='" + this->work.esc(user) + "'";
    
    /* execute query */
    pqxx::result r = work.exec(query);

    /* read result */
    if(not r.empty()) {
        return r[0]["user_type"].c_str();
    } else {
        /* if nothing was found, default to user */
        return "user";
    }
}

PersistentRating
DatabaseInterface::getRatingForUpdate(const std::string& username,
                                   const std::string& category)
{
    int user_id = this->getUserId(username);

    /* prepare query */
    std::string query =
        "SELECT * "
        " FROM player_rating "
        " WHERE user_id=" + pqxx::to_string(user_id) + " AND " +
        "       category='" + this->work.esc(category) + "' " +
        " FOR UPDATE";
    
    /* execute query */
    pqxx::result r = work.exec(query);

    /* read results */
    /* if not found, 
     * default to 0 */
    if(not r.empty()) {
        PersistentRating rating;
        time_t t;
        r[0]["rating"].to(rating.rating);
        r[0]["volatility"].to(rating.volatility);
        r[0]["wins"].to(rating.wins);
        r[0]["defeats"].to(rating.defeats);
        r[0]["draws"].to(rating.draws);
        r[0]["max_rating"].to(rating.max_rating);
        r[0]["max_timestamp"].to(t);
        r[0]["last_game"].to(rating.last_game);
        rating.max_timestamp = from_time_t(t);
        return rating;
    } else {
        return PersistentRating();
    }
}

void DatabaseInterface::setRating(const std::string& username,
                               const std::string& category,
                               const PersistentRating& rating)
{
    int user_id = this->getUserId(username);

    /* prepare query */
    std::string query =
        " UPDATE player_rating"
        " SET rating=" + pqxx::to_string(rating.rating) +
        "    ,volatility=" + pqxx::to_string(rating.volatility) +
        "    ,wins=" + pqxx::to_string(rating.wins) +
        "    ,defeats=" + pqxx::to_string(rating.defeats) +
        "    ,draws=" + pqxx::to_string(rating.draws) +
        "    ,max_rating=" + pqxx::to_string(rating.max_rating) +
        "    ,max_timestamp=" + pqxx::to_string(Util::ptime_to_time_t(rating.max_timestamp)) +
        "    ,last_game=" + pqxx::to_string(rating.last_game) +
        " WHERE user_id='" + pqxx::to_string(user_id) + "' AND " +
        "       category='" + this->work.esc(category) + "'";
    
    /* execute query */
    pqxx::result r = work.exec(query);

    
    /* if the entry was not in the database
     * then create it */
    if(r.affected_rows() == 0) {

        query = 
            "INSERT INTO player_rating VALUES"
            "  ( " + pqxx::to_string(user_id) + "," +
            "   '" + this->work.esc(category) + "'," +
            "    " + pqxx::to_string(rating.rating) + ","
            "    " + pqxx::to_string(rating.volatility) + ","
            "    " + pqxx::to_string(rating.wins) + ","
            "    " + pqxx::to_string(rating.draws) + ","
            "    " + pqxx::to_string(rating.defeats) + ","
            "    " + pqxx::to_string(rating.max_rating) + ","
            "    " + pqxx::to_string(Util::ptime_to_time_t(rating.max_timestamp)) + ","
            "    " + pqxx::to_string(rating.last_game) + ")";
        
        work.exec(query);
    }
}

void DatabaseInterface::insertGameResult(GameResult& game_result)
{

    std::string category = game_result.category();
    Rating tmp;
    PersistentRating rating;
    PersistentGame game;

    /* create a map with the player's rating */
    std::map<Player, Rating> ratings;
    std::map<Player, PersistentRating> pratings;
    foreach(player, game_result.players()) {
        std::string name = player->jid.partial();
        rating = this->getRatingForUpdate(name, category);
        pratings.insert(std::make_pair(player->jid, rating));
        tmp.rating() = rating.rating;
        tmp.volatility() = rating.volatility;
        tmp.wins() = rating.wins;
        tmp.draws() = rating.draws;
        tmp.losses() = rating.defeats;
        tmp.last_game() = rating.last_game;
        ratings.insert(std::make_pair(player->jid, tmp));
    }

    /* update ratings */
    game_result.updateRating(ratings);

    /* create vector of persistent ratings */
    foreach(it, ratings) {
        rating.rating = it->second.rating();
        rating.volatility = it->second.volatility();
        rating.wins = it->second.wins();
        rating.draws = it->second.draws();
        rating.defeats = it->second.losses();
        rating.max_rating = pratings[it->first].max_rating;
        rating.max_timestamp = pratings[it->first].max_timestamp;
        rating.last_game = Util::ptime_to_time_t(boost::posix_time::second_clock::local_time());
        if(rating.rating > rating.max_rating) {
            rating.max_rating = rating.rating;
            rating.max_timestamp = boost::posix_time::second_clock::local_time();
        }
        this->setRating(it->first.partial(), category, rating);
    }
    
    /* set game values */
    game.players = game_result.players();
    game.category = category;
    game.time_stamp = boost::posix_time::second_clock::local_time();
    game.result = game_result.end_reason();

    /* get history */
    std::auto_ptr<XML::Tag> history(game_result.history());
    game.history = history->xml();

    /* insert to the database */
    this->insertGame(game);
}

void DatabaseInterface::insertGame(const PersistentGame& game) {

    int game_id;
    std::string query;

    /* insert the game in the database */
    query =
            " INSERT INTO games (category, time_stamp, history, result) "
            " VALUES "
            "   ('" + this->work.esc(game.category) + "'" +
            "   , " + pqxx::to_string(Util::ptime_to_time_t(game.time_stamp)) + "" +
            "   ,'" + this->work.esc(game.history) + "'"
            "   ,'" + this->work.esc(game.result) + "')"
            " RETURNING game_id";
    pqxx::result r = this->work.exec(query);

    /* get the game_id */
    r[0]["game_id"].to(game_id);
 
    /* insert the players */
    foreach(player, game.players) {
        int user_id = this->getUserId(player->jid.partial());
        query =
            " INSERT INTO game_players VALUES"
            "   ( " + pqxx::to_string(game_id) + "" +
            "   ,'" + pqxx::to_string(user_id) + "'"
            "   ,'" + this->work.esc(player->score) + "'"
            "   ,'" + this->work.esc(player->role) + "')";
        work.exec(query);
    }
}


std::string DatabaseInterface::getGameHistory(int game_id) {

    /* find the game in the database */
    std::string query = 
        "SELECT history FROM games WHERE game_id = " + pqxx::to_string(game_id);
    pqxx::result result = this->work.exec(query);

    /* check result */
    if(result.empty()) {
        throw game_not_found("The requested game is not in the database");
    }

    return result[0]["history"].c_str();
}

std::vector<PersistentGame> DatabaseInterface::searchGames(
                const std::vector<std::string> players,
                int offset,
                int max_results)
{
    std::vector<PersistentGame> games;
    time_t t;

    /* prepare sql query */
    std::string select =
            " SELECT games.game_id, category, time_stamp ";
    std::string from =
            " FROM games ";
    std::string where;

    /* XXX this is necessary in order to
     * accept an arbitrary number of players */
    for(int i=0;i<int(players.size());++i) {
        int user_id = this->getUserId(players[i]);
        std::string id_str = Util::to_string(i);
        from += ", game_players g" + id_str + " ";
        if(where.empty())
            where = " WHERE ";
        else
            where += " AND ";
        where += " games.game_id = g" + id_str
            + ".game_id AND g" + id_str + ".user_id = '" + pqxx::to_string(user_id) + "' ";
    }
    std::string query = select + from + where +
                           " ORDER BY game_id DESC " +
                           " LIMIT " + pqxx::to_string(max_results) +
                           " OFFSET " + pqxx::to_string(offset)
                           ;

    /* execute query */
    pqxx::result result = this->work.exec(query);

    /* list results */
    foreach(r, result) {
        PersistentGame game;

        /*  parse values */
        r->at("game_id").to(game.id);
        r->at("time_stamp").to(t);
        game.time_stamp = boost::posix_time::from_time_t(t);
        game.category = r->at("category").c_str();

        /* get players */
        std::string query =
            "SELECT user_id, role, score FROM game_players WHERE game_id = "
            + Util::to_string(game.id);
        pqxx::result result = this->work.exec(query);
        foreach(r, result) {
            PlayerResult player;
            int user_id;
            r->at("user_id").to(user_id);
            player.jid = XMPP::Jid(this->getUsername(user_id));
            player.role = r->at("role").c_str();
            player.score = r->at("score").c_str();
            game.players.push_back(player);
        }

        /* store game */
        games.push_back(game);
    }
    return games;
}

void DatabaseInterface::insertAdjournedGame(const PersistentAdjournedGame& game)
{
    int game_id;

    /* get and id from the sequence */

    std::string query;

    /* inert game */
    query =
            " INSERT INTO adjourned_games (category, time_stamp, history) VALUES"
            "   ('" + this->work.esc(game.category) + "'" +
            "   , " + pqxx::to_string(Util::ptime_to_time_t(game.time_stamp)) + "" +
            "   ,'" + this->work.esc(game.history) + "')"
            " RETURNING game_id";
    pqxx::result r = this->work.exec(query);
    r[0]["game_id"].to(game_id);
 
    /* insert players */
    foreach(player, game.players) {
        int user_id = this->getUserId(player->partial());
        query =
            " INSERT INTO adjourned_game_players VALUES"
            "   ( " + pqxx::to_string(game_id) + "" +
            "   ,'" + pqxx::to_string(user_id) + "')";
        work.exec(query);
    }
}

std::string DatabaseInterface::getAdjournedGameHistory(int game_id) {
    std::string query;
    
    /* find game */
    query = "SELECT history FROM adjourned_games WHERE game_id = " + Util::to_string(game_id);
    pqxx::result result = this->work.exec(query);

    if(result.empty()) {
        throw game_not_found("The requested game is not in the database");
    }

    return result[0][0].c_str();
}

std::vector<PersistentAdjournedGame> DatabaseInterface::searchAdjournedGames(
                const std::vector<std::string>& players,
                int offset,
                int max_results)
{
    std::vector<PersistentAdjournedGame> games;

    std::string select =
            " SELECT adjourned_games.game_id, category, time_stamp ";
    std::string from =
            " FROM adjourned_games ";
    std::string where;

    /* prepare sql query */
    for(int i=0;i<int(players.size());++i) {
        int user_id = this->getUserId(players[i]);
        std::string id_str = Util::to_string(i);
        from += ", adjourned_game_players g" + id_str + " ";
        if(i > 0)
            where += " AND ";
        else
            where = " WHERE ";
        where += " adjourned_games.game_id = g" + id_str
            + ".game_id AND g" + id_str + ".user_id = '"
            + pqxx::to_string(user_id) + "' ";
    }
    std::string query = select + from + where + " limit " + Util::to_string(max_results) + " offset " + Util::to_string(offset);

    /* search games */
    pqxx::result result = this->work.exec(query);


    /* chek each result */
    foreach(r, result) {
        PersistentAdjournedGame game;
        time_t t;
        r->at("game_id").to(game.id);
        r->at("time_stamp").to(t);
        game.time_stamp = boost::posix_time::from_time_t(t);
        game.category = r->at("category").c_str();

        /* get players */
        std::string query =
            "SELECT user_id FROM adjourned_game_players WHERE game_id = "
            + pqxx::to_string(game.id);
        pqxx::result result = this->work.exec(query);
        foreach(r, result) {
            int user_id;
            r->at("user_id").to(user_id);
            Player player = XMPP::Jid(this->getUsername(user_id));
            game.players.push_back(player);
        }
        games.push_back(game);
    }
    return games;
}

void DatabaseInterface::eraseAdjournedGame(int game_id) {

    std::string query;

    /* erase game */
    query =
            " DELETE FROM adjourned_games WHERE "
            "   game_id=" + pqxx::to_string(game_id);
    this->work.exec(query);
}

std::vector<std::string> DatabaseInterface::getAdmins() {

    std::vector<std::string> admins;
    std::string admin;

    /* prepare query */
    std::string query =
        " SELECT user_name"
        " FROM users "
        " WHERE user_type='admin'";

    /* execute query */
    pqxx::result result = work.exec(query);
    
    /* read result */
    foreach(r, result) {
        r->at("user_name").to(admin);
        admins.push_back(admin);
    }
    
    return admins;
}
