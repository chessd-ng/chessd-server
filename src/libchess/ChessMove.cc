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

#include <iostream>
#include "ChessMove.hh"
using namespace std;

int Position::posx() const {
	return (this->pos[0]);
}
int Position::posy() const {
	return (this->pos[1]);
}

std::string Position::toStringNotation() const {
	std::string ans;
	ans=(char)(pos[0] + 'a');
	ans+=(char)(pos[1] + '1');
	return ans;
}

Position::Position(string& p) {
	if(p.size() == 3)
	{
		this->pos[0]=(int)(p[0]-'0');
		this->pos[1]=(int)(p[1]-'0');
	}
}

Position::Position(int x, int y) {
	this->pos[0]=x;
	this->pos[1]=y;
}
Position::Position() { 
	this->pos[0]=pos[1]=0;
};

bool Position::operator ==(const Position& p) const {
	if(this->pos[0] == p.pos[0] && this->pos[1] == p.pos[1])
		return true;
	return false;
}

bool Position::operator !=(const Position& p) const {
	if(this->pos[0] == p.pos[0] && this->pos[1] == p.pos[1])
		return false;
	return true;
}

ChessMove::ChessMove() {
	this->from=Position();
	this->to=Position();
};

ChessMove::ChessMove(int jogador,const std::string &pgn) {
	int x,y;
	if(pgn.size() >=4) {
		x=pgn[0]-'a';
		y=pgn[1]-'1';
		this->from=Position(x,y);
		x=pgn[2]-'a';
		y=pgn[3]-'1';
		this->to=Position(x,y);
		this->jogador=jogador;
	} else {
		this->from=this->to=Position(-1,-1);
	}
}

ChessMove::ChessMove(const Position& a, const Position& b,const int j)	{
	this->from=a;
	this->to=b;
	this->jogador=j;
}

Position ChessMove::getto() const {
	return this->to;
}
Position ChessMove::getfrom() const {
	return this->from;
}
int ChessMove::getColor() const {
	return this->jogador;
}


