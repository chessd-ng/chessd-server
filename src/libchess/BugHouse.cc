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

#include "BugHouse.hh"

BugHouse::BugHouse() {
	this->games=std::vector<Chess>(2);
	this->pieces=std::vector<std::vector<ChessPiece*> >(4);
	playerteam[0]=0;
	playerteam[1]=1;
	playerteam[2]=1;
	playerteam[3]=0;
}

int BugHouse::numberOfTurns(int game) const {
	return this->games[game].numberOfTurns();
}

int BugHouse::winner() const {
	for(int i=0;i<games.size();i++)
		if(this>games[i].verifyCheckMate())
			return *this->playerteam.find(games[i].winner()+i*2);
	return -1;
}

int BugHouse::turn(int game) const {
	return this->games[game].turn();
}

bool BugHouse::verifyCheckMate() const {
	for(int i=0;i<games.size();i++)
		if(this->games[i].verifyCheckMate())
			return true;
	return false;
}

bool BugHouse::verifyDraw() const {
//	for(int i=0;i<games.size();i++)
//		if(this->games[i].verifyDraw())
//			return true;
	return false;
}

const BugHouseState& BugHouse::getState() const {
	return this->current_state;
}

bool BugHouse::verifyAndMakeMove(int player, const std::string& move) {
	if(move[1]=='@') {
	} else {
		int g=player/2;
		if(games[g].turn() == player%2)
			return games[g].verifyAndMakeMove(move);
	}
	return false;
}
