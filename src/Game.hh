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

#ifndef GAME_HH
#define GAME_HH

#include <map>
#include <string>
#include <vector>
#include "Team.hh"
#include "Rating.hh"
#include "XML/Xml.hh"
#include "Util/Timer.hh"

enum TeamResult {
	WINNER,
	DRAWER,
	LOSER,
	UNDEFINED
};

typedef std::vector<std::pair<Team, TeamResult> > TeamResultList;

class GameResult {
	public:
		virtual ~GameResult() { }
		
		/*! \brief The game category */
		virtual const std::string& category() const = 0;

		/*! \brief The reason why the game ended */
		virtual const std::string& end_reason() const = 0;

		/*! \brief List of all players in the game */
		virtual const PlayerList& players() const = 0;

		virtual const TeamResultList& results() const = 0;

		/*! \brief The game history */
		virtual XML::Tag* history() const = 0;

		/*! \brief Update the player ratings
		 *
		 * The given ratings must contain all players in the game.
		 * The map is modified to match new ratings
		 * */
		virtual void updateRating(std::map<Player, Rating>& ratings) const = 0;
};

//typedef XML::Tag GameState;

class Game {
	public:
		virtual ~Game() { }

		/*! \brief Current state of the game.
         *
         * \param current_time is the time counted from the begining of the game.
         */
		virtual XML::Tag* state(const Util::Time& current_time) const = 0;

		/*! \brief The game category. */
		virtual const std::string& category() const = 0;

		/*! \brief A title for the game. */
		virtual const std::string& title() const = 0;

		/*! \brief The player has resigned. */
		virtual void resign(const Player& player) = 0;

		/*! \brief The player has called a flag. */
		virtual void call_flag(const Player& player) = 0;

		/*! \brief The players agreed on a draw. */
		virtual void draw() = 0;

		/*! \brief The players agreed on a draw. */
		virtual void adjourn() = 0;

		/*! \brief Has the game ended?  */
		virtual bool done(const Util::Time& current_time) = 0;

        /*! \brief Returns the game result. */
		virtual GameResult* result() const = 0;
		
		/*! \brief Make a move in the game.
		 *
         * \param player us the player who made the move.
		 * \param movement is the description of the movement.
         * \param time_stamp is the time of the move since the begining of the game.
         */
		virtual void move(const Player& player, const std::string& movement, const Util::Time& time_stamp) = 0;

		/*! \brief The list of teams playing the game. */
		virtual const TeamList& teams() const = 0;
};



#endif
