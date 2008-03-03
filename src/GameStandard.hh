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

#ifndef GAMESTANDARD_HH
#define GAMESTANDARD_HH

#include "GameChess.hh"

class GameStandard : public GameChess {
	public:
		GameStandard(const StandardPlayerList& _players);

		GameStandard(XML::Tag* adjourned_game) : GameChess(adjourned_game) { };

		virtual ~GameStandard() {};

		virtual GameResult* result() const;

	private:
};

class ChessStandardGameResult : public ChessGameResult {
	public:
		ChessStandardGameResult(const std::string &endreason,const PlayerResultList &l,XML::Tag* _hist);

//		virtual void updateRating(std::map<Player, Rating> &ratings) const;
	private:
};

#endif
