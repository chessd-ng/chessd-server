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

#ifndef GAMEDATABASE_HH
#define GAMEDATABASE_HH

#include <string>
#include <memory>
#include <vector>

#include <pqxx/pqxx>

#include "Team.hh"

struct PersistentGame {
    public:
        int id;
        PlayerResultList players;
        std::string category;
        int time_stamp;
        std::string history;
};

class game_not_found : public std::runtime_error {
    public:
        explicit game_not_found(const std::string& msg) : std::runtime_error(msg) { }
};

/*! \brief An interface to the games in the database */
class GameDatabase {
    public:

        /*! brief Constructor */
        GameDatabase(pqxx::work& w);

        //PersistentGame* getGame(int game_id) const;

        /*! \brief Store a game in the database */
        void insertGame(const PersistentGame& game);

        std::vector<PersistentGame> searchGames(
                const std::vector<std::string> players,
                int offset,
                int max_results);

        std::string getGameHistory(int game_id);

    private:
        pqxx::work& work;
};

#endif
