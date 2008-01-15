#ifndef RATINGMANAGER_HH
#define RATINGMANAGER_HH

#include <map>
#include <string>

#include <boost/function.hpp>

#include "DatabaseManager.hh"

#include "Rating.hh"


class RatingDBInterface {
    public:

        RatingDBInterface(pqxx::work& w);

        Rating getRating(const std::string& user, const std::string& category);

        Rating getRating(const std::string& user, const std::string& category) const;

        void setRating(const std::string& user, const std::string& category, const Rating& rating);

    private:

        pqxx::work& work;
};

typedef boost::function<void (RatingDBInterface&)> RatingTransactor;
typedef boost::function<void (const RatingDBInterface&)> ConstRatingTransactor;

class RatingDatabase {
    public:

        RatingDatabase(DatabaseManager&);

        ~RatingDatabase();

        void perform(const RatingTransactor& transactor);

        void perform_const(const ConstRatingTransactor& transactor);


    private:

        DatabaseManager& database_manager;
};


#endif
