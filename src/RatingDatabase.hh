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

#ifndef RATINGDATABASE_HH
#define RATINGDATABASE_HH

#include <pqxx/pqxx>

#include <string>

#include <map>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Rating.hh"

struct PersistentRating {
    PersistentRating() : rating(0), volatility(0), wins(0), defeats(0), draws(0), max_rating(0) { }
    int rating;
    double volatility;
    int wins;
    int defeats;
    int draws;
    int max_rating;
    boost::posix_time::ptime max_timestamp;
};

class RatingDatabase {
    public:

        RatingDatabase(pqxx::work& w);

        std::vector<std::pair<std::string, PersistentRating> > getRatings(const std::string& user, const std::string& category);

        PersistentRating getRatingForUpdate(const std::string& user, const std::string& category);

        void setRating(const std::string& user, const std::string& category, const PersistentRating& rating);

        std::string getUserType(const std::string& user);

    private:

        pqxx::work& work;
};

#endif
