#include <iostream>

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
