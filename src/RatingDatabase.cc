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

#include "RatingDatabase.hh"

RatingDatabase::RatingDatabase(pqxx::work& work) : work(work) { }

Rating RatingDatabase::getRating(const std::string& user, const std::string& category)
{
    std::string query =
        "SELECT *"
        " FROM player_rating"
        " WHERE username='" + this->work.esc(user) + "'" +
          " AND category='" + this->work.esc(category) + "'" +
        " FOR UPDATE";
        
    pqxx::result r = work.exec(query);

    if(r.size() == 0) {
        return Rating();
    } else {
        Rating rating;
        r[0]["rating"].to(rating.rating());
        r[0]["volatility"].to(rating.volatility());
        r[0]["wins"].to(rating.wins());
        r[0]["losses"].to(rating.losses());
        r[0]["draws"].to(rating.draws());
        return rating;
    }
}

std::string RatingDatabase::getUserType(const std::string& user)
{
    std::string query =
        "SELECT type"
        " FROM player_type"
        " WHERE username='" + this->work.esc(user) + "'";
        
    pqxx::result r = work.exec(query);

    if(r.size() == 0) {
        return "user";
    } else {
        return r[0][0].c_str();
    }
}

Rating RatingDatabase::getRatingForUpdate(const std::string& user, const std::string& category)
{
    std::string query =
        "SELECT *"
        " FROM player_rating"
        " WHERE username='" + this->work.esc(user) + "'" +
          " AND category='" + this->work.esc(category) + "'" +
        " ";
        
    pqxx::result r = work.exec(query);

    if(r.size() == 0) {
        return Rating();
    } else {
        Rating rating;
        r[0]["rating"].to(rating.rating());
        r[0]["volatility"].to(rating.volatility());
        r[0]["wins"].to(rating.wins());
        r[0]["losses"].to(rating.losses());
        r[0]["draws"].to(rating.draws());
        return rating;
    }
}

void RatingDatabase::setRating(const std::string& user, const std::string& category, const Rating& rating)
{
    std::string query =
        " UPDATE player_rating"
        " SET rating=" + pqxx::to_string(rating.rating()) +
            ",volatility=" + pqxx::to_string(rating.volatility()) +
            ",wins=" + pqxx::to_string(rating.wins()) +
            ",losses=" + pqxx::to_string(rating.losses()) +
            ",draws=" + pqxx::to_string(rating.draws()) +
        " WHERE username='" + this->work.esc(user) + "'" +
          " AND category='" + this->work.esc(category) + "'";
        
    pqxx::result r = work.exec(query);

    
    if(r.affected_rows() == 0) {
        // entry does not exists on the table

        query = 
            " INSERT INTO player_rating VALUES"
            "('" + this->work.esc(user) + "'" +
            ",'" + this->work.esc(category) + "'" +
            ", "+ pqxx::to_string(rating.rating()) +
            ", " + pqxx::to_string(rating.volatility()) +
            ", " + pqxx::to_string(rating.wins()) +
            ", " + pqxx::to_string(rating.draws()) +
            ", " + pqxx::to_string(rating.losses()) + ")";
        
        work.exec(query);
    }
}
