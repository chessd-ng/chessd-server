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

#ifndef DATABASEINTERFACE_HH
#define DATABASEINTERFACE_HH

#include <boost/date_time/posix_time/posix_time.hpp>

#include <pqxx/pqxx>

#include <string>
#include <memory>
#include <vector>

#include "DatabaseException.hh"
#include "Game.hh"

struct PersistentGame {
    public:
        int id;
        PlayerResultList players;
        std::string category;
        boost::posix_time::ptime time_stamp;
        std::string history;
        std::string result;
};

struct PersistentRating {
    PersistentRating() : rating(0),
                         volatility(0),
                         wins(0),
                         defeats(0),
                         draws(0),
                         max_rating(0),
                         last_game(0) { }
    int rating;
    double volatility;
    int wins;
    int defeats;
    int draws;
    int max_rating;
    int last_game;
    boost::posix_time::ptime max_timestamp;
};

struct PersistentAdjournedGame {
    public:
        int id;
        PlayerList players;
        std::string category;
        boost::posix_time::ptime time_stamp;
        std::string history;
};

/*! \brief An interface to the database.
 *
 * An instance of this class is received on a database transaction.
 */
class DatabaseInterface : public pqxx::transactor<>
{
    public:

        DatabaseInterface(pqxx::work&);

        /*! \brief Store a game in the database */
        void insertGame(const PersistentGame& game);

        /*! \brief Sotre a game from a result and update ratings */
        void insertGameResult(GameResult& game_result);

        /*! \brief Search games in the database */
        std::vector<PersistentGame> searchGames(
                const std::vector<std::string> players,
                int offset,
                int max_results);

        /*! \brief Get a game's history */
        std::string getGameHistory(int game_id);

        /*! \brief Get the ratings of a player */
        std::vector<std::pair<std::string, PersistentRating> >
                getRatings(const std::string& user,
                           const std::string& category);

        /*! \brief Get a rating of a player that is going to be updated */
        PersistentRating getRatingForUpdate(const std::string& user,
                                            const std::string& category);

        /*! \brief Set the rating of the player */
        void setRating(const std::string& user,
                       const std::string& category,
                       const PersistentRating& rating);

        /*! \brief Get the type of a user */
        std::string getUserType(const std::string& user);

        /*! \brief Return a list of all admins */
        std::vector<std::string> getAdmins();

        /*! \brief Store a game in the database */
        void insertAdjournedGame(const PersistentAdjournedGame& game);

        /*! \brief Store an adjourned game */
        std::vector<PersistentAdjournedGame> searchAdjournedGames(
                const std::vector<std::string>& players,
                int offset,
                int max_results);

        /*! \brief Get a game's history */
        std::string getAdjournedGameHistory(int game_id);

        /*! \brief Erase game from the database */
        void eraseAdjournedGame(int game_id);

        /*! \brief Set user info */
        void setUserEmail(const std::string& username, const std::string& email);

    private:

        /*! \brief Get the user's id by his name */
        int getUserId(const std::string& username);

        /*! \brief Get the username id by his id */
        std::string getUsername(int user_id);

        pqxx::work& work;
        
};

#endif
