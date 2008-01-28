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

#include "State.hh"
#include <string>
#include <iostream>

std::string State::toString(int n) {
	char tmp[16];
	sprintf(tmp,"%d",n);
	return std::string(tmp);
}

State::State() {
	this->castle=std::string("KQkq");
	this->enpassant=Position(-1,-1);
	this->lastenpassant=Position(-1,-1);
	this->halfmoves=0;
	this->fullmoves=1;
	this->vez=ChessPiece::WHITE;
	//cuidado com isso
	this->tabfen=std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}
State::State(const std::string posfen) {
	this->tabfen=posfen;
	this->castle=std::string("KQkq");
	this->enpassant=Position(-1,-1);
	this->lastenpassant=Position(-1,-1);
	this->halfmoves=0;
	this->fullmoves=1;
	this->vez=ChessPiece::WHITE;
}
State::State(const State& a) {
	*this=a;
}
std::string State::getFEN() const {
	return std::string(tabfen);
}
std::string State::getentireFEN() const {
	std::string ans;
	ans=this->tabfen;
	ans+=" ";
	ans+=(char)(this->vez == ChessPiece::WHITE ? 'w' : 'b');
	ans+=" ";
	ans+=this->castle;
	ans+=" ";
	if( this->enpassant == Position(-1,-1) )
		ans+='-';
	else
		ans+=this->enpassant.toStringNotation();
	ans+=" ";
	ans+=toString(this->halfmoves);
	ans+=" ";
	ans+=toString(this->fullmoves);
	return ans;
}

History::History() {
	this->allgame.clear();
}

History::History(const State& est) {
	this->allgame.clear();
	this->putinHistory(est);
}

History::~History() {
	allgame.clear();
}

void History::putinHistory(const State& est) {
	allgame.push_back(est);
}

const std::vector<State>& History::getHistory() const {
	return this->allgame;
}

#ifdef TESTE
void History::Desenha() const {
	for(int i=0;i<(int)allgame.size();i++) {
		std::cout << allgame[i].getentireFEN() << std::endl;
	}
}
#endif
