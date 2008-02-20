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

#include "State.hh"
#include "../Util/utils.hh"
#include <string>
#include <iostream>

State::State() {
}

State::State(const std::string& posfen) {
	this->board_fen=posfen;
}

const std::string& State::boardFEN() const {
	return this->board_fen;
}

int State::turn() const {
	return this->_turn;
}

ChessState::ChessState() {
	this->castle=std::string("KQkq");
	this->enpassant=Position(-1,-1);
	this->halfmoves=0;
	this->fullmoves=1;
	this->_turn=ChessPiece::WHITE;
	//XXX be careful with this
	this->board_fen=std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

ChessState::ChessState(const std::string& _board_fen) : State(_board_fen) {
	this->castle=std::string("KQkq");
	this->enpassant=Position(-1,-1);
	this->halfmoves=0;
	this->fullmoves=1;
	this->_turn=ChessPiece::WHITE;
}

std::string ChessState::FEN() const {
	std::string ans;
	ans=this->board_fen;
	ans+=" ";
	ans+=(char)(this->_turn == ChessPiece::WHITE ? 'w' : 'b');
	ans+=" ";
	ans+=this->castle;
	ans+=" ";
	if( this->enpassant == Position(-1,-1) )
		ans+='-';
	else
		ans+=this->enpassant.toStringNotation();
	ans+=" ";
	ans+=Util::to_string(this->halfmoves);
	ans+=" ";
	ans+=Util::to_string(this->fullmoves);
	return ans;
}

bool ChessState::operator==(const ChessState& s) const {
	return (this->board_fen == s.board_fen) and (this->castle == s.castle) and (this->enpassant == s.enpassant);
}

BugHouseState::BugHouseState() {
	states=std::vector<ChessState>(2);
}

ChessState& BugHouseState::operator[](int v) {
	return states[v];
}
