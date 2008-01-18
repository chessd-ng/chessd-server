#ifndef DATABASEINTERFACE_HH
#define DATABASEINTERFACE_HH

#include <pqxx/pqxx>

#include "GameDatabase.hh"

#include "RatingDatabase.hh"

class DatabaseInterface : public pqxx::transactor<>
{
    public:

        DatabaseInterface(pqxx::work&);

        GameDatabase game_database;

        RatingDatabase rating_database;

    private:

        
};

#endif
