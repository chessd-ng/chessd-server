#include <iostream>

#include "DatabaseInterface.hh"

DatabaseInterface::DatabaseInterface(pqxx::work& work) :
    game_database(work),
    rating_database(work)
{
}
