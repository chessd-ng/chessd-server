/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
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

#ifndef __ESTADOH__
#define __ESTADOH__

#include <vector>
#include <string>
#include "Piece.hh"
#include "ChessMove.hh"

struct State {
	std::string tabfen;
	std::string castle;
	Position lastenpassant;
	Position enpassant;
	int halfmoves;
	int fullmoves;
	int vez;

	static std::string toString(int n);

	State();
	State(const std::string posfen);
	State(const State& a);
	std::string getFEN() const;
	std::string getentireFEN() const;
};

class History {
	private:
		std::vector<State> allgame;
	public:
		History();
		History(const State& est);
		~History();
		void putinHistory(const State& est);
		const std::vector<State> &getHistory() const ;
};
#endif
