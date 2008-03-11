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

#ifndef ADJOURNEDDATABASE_HH
#define ADJOURNEDDATABASE_HH

#include <string>
#include <memory>
#include <vector>

#include <pqxx/pqxx>

#include "Team.hh"

#include "Util/Date.hh"

struct PersistentAdjourned {
    public:
        int id;
        PlayerList players;
        std::string category;
        boost::posix_time::ptime time_stamp;
        std::string history;
};

class adjourned_game_not_found : public std::runtime_error {
    public:
        explicit adjourned_game_not_found(const std::string& msg) : std::runtime_error(msg) { }
};

/*! \brief An interface to the games in the database */
class AdjournedDatabase {
    public:

        /*! brief Constructor */
        AdjournedDatabase(pqxx::work& w);

        /*! \brief Store a game in the database */
        void insertGame(const PersistentAdjourned& game);

        std::vector<PersistentAdjourned> searchGames(
                const std::vector<std::string> players,
                int offset,
                int max_results);

        std::string getGameHistory(int game_id);

        void eraseGame(int game_id);

    private:
        pqxx::work& work;
};

#endif
