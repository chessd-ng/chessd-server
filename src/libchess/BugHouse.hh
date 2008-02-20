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

#include "BugHouseChess.hh"
#include <vector>
#include <map>

/*
 * assumptions:
 * team A will be players 0 and 3
 * team B will be players 1 and 2
 * players 0 and 1 will be playing against each other
 * players 2 and 3 will be playing against each other
 * 0 and 2 will be white
 * 1 and 3 will be black
*/

class BugHouse {
	public:
		BugHouse();

		~BugHouse();
		
		/*! \brief return the number of turns of a game*/
		int numberOfTurns(int game) const;

		/*! \brief returns the team who have won*/
		int winner() const;

		/*! \brief returns the turn of game*/
		int turn(int game) const;

		bool verifyCheckMate() const;

		bool verifyDraw() const;

		const BugHouseState& getState() const;

		bool verifyAndMakeMove(int player, const std::string& move);

	private:

		void updateState();

		std::vector<BugHouseChess> games;

		std::map<int,int> playerteam;

		BugHouseState current_state;
};
