#ifndef RATINGDATABASE_HH
#define RATINGDATABASE_HH

#include <pqxx/pqxx>

#include <string>

#include "Rating.hh"


class RatingDatabase {
    public:

        RatingDatabase(pqxx::work& w);

        Rating getRating(const std::string& user, const std::string& category);

        Rating getRatingForUpdate(const std::string& user, const std::string& category);

        void setRating(const std::string& user, const std::string& category, const Rating& rating);

    private:

        pqxx::work& work;
};

#endif
