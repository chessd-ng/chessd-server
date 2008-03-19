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

#include "RatingDatabase.hh"

#include "Util/utils.hh"
#include "Util/Date.hh"

using namespace boost::posix_time;

RatingDatabase::RatingDatabase(pqxx::work& work) : work(work) { }

std::vector<std::pair<std::string, PersistentRating> > RatingDatabase::getRatings(const std::string& user, const std::string& category)
{
    std::string query;
    std::vector<std::pair<std::string, PersistentRating> > ratings;

    /* prepare SQL query */
    /* if no category is givn, take all of them */
    if(category.empty()) {
        query =
            "SELECT * "
            " FROM player_rating"
            " WHERE username='" + this->work.esc(user) + "'";
    } else {
        query = 
            "SELECT *"
            " FROM player_rating"
            " WHERE username='" + this->work.esc(user) + "'" +
            " AND category='" + this->work.esc(category) + "'";
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
        r->at("losses").to(rating.defeats);
        r->at("draws").to(rating.draws);
        r->at("max_rating").to(rating.max_rating);
        r->at("max_timestamp").to(t);
        r->at("last_game").to(rating.last_game);
        rating.max_timestamp = from_time_t(t);
        ratings.push_back(std::make_pair(category, rating));
    }
    return ratings;
}

std::string RatingDatabase::getUserType(const std::string& user)
{
    /* prepare query */
    std::string query =
        "SELECT type"
        " FROM player_type"
        " WHERE username='" + this->work.esc(user) + "'";
    
    /* execute query */
    pqxx::result r = work.exec(query);

    /* read result */
    /* if nothing was found, default
     * to user */
    if(r.size() == 0) {
        return "user";
    } else {
        return r[0][0].c_str();
    }
}

PersistentRating RatingDatabase::getRatingForUpdate(const std::string& user, const std::string& category)
{
    /* prepare query */
    std::string query =
        "SELECT *"
        " FROM player_rating"
        " WHERE username='" + this->work.esc(user) + "'" +
          " AND category='" + this->work.esc(category) + "'" +
        " FOR UPDATE";
    
    /* execute query */
    pqxx::result r = work.exec(query);

    /* read results */
    /* if not found, 
     * default to 0 */
    if(r.size() == 0) {
        return PersistentRating();
    } else {
        PersistentRating rating;
        time_t t;
        r[0]["rating"].to(rating.rating);
        r[0]["volatility"].to(rating.volatility);
        r[0]["wins"].to(rating.wins);
        r[0]["losses"].to(rating.defeats);
        r[0]["draws"].to(rating.draws);
        r[0]["max_rating"].to(rating.max_rating);
        r[0]["max_timestamp"].to(t);
        r[0]["last_game"].to(rating.last_game);
        rating.max_timestamp = from_time_t(t);
        return rating;
    }
}

void RatingDatabase::setRating(const std::string& user, const std::string& category, const PersistentRating& rating)
{
    /* prepare query */
    std::string query =
        " UPDATE player_rating"
        " SET rating=" + pqxx::to_string(rating.rating) +
            ",volatility=" + pqxx::to_string(rating.volatility) +
            ",wins=" + pqxx::to_string(rating.wins) +
            ",losses=" + pqxx::to_string(rating.defeats) +
            ",draws=" + pqxx::to_string(rating.draws) +
            ",max_rating=" + pqxx::to_string(rating.max_rating) +
            ",max_timestamp=" + pqxx::to_string(Util::ptime_to_time_t(rating.max_timestamp)) +
            ",last_game=" + pqxx::to_string(rating.last_game) +
        " WHERE username='" + this->work.esc(user) + "'" +
          " AND category='" + this->work.esc(category) + "'";
    
    /* execute query */
    pqxx::result r = work.exec(query);

    
    /* if the entry was not in the database
     * then create it */
    if(r.affected_rows() == 0) {

        query = 
            " INSERT INTO player_rating VALUES"
            "('" + this->work.esc(user) + "'" +
            ",'" + this->work.esc(category) + "'" +
            ", "+ pqxx::to_string(rating.rating) +
            ", " + pqxx::to_string(rating.volatility) +
            ", " + pqxx::to_string(rating.wins) +
            ", " + pqxx::to_string(rating.draws) +
            ", " + pqxx::to_string(rating.defeats) +
            ", " + pqxx::to_string(rating.max_rating) +
            ", " + pqxx::to_string(Util::ptime_to_time_t(rating.max_timestamp)) + ")";
            ", " + pqxx::to_string(rating.last_game) + ")";
        
        work.exec(query);
    }
}
