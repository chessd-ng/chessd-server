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

int Position::x() const {
	return (this->_x);
}
int Position::y() const {
	return (this->_y);
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

Position::Position(int __x, int __y) {
	this->pos[0]=__x;
	this->pos[1]=__y;
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

Position Position::operator +(const Position& p) const {
	return Position(this->x()+p.x(),this->y()+p.y());
}

Move::Move() {
}

Move::Move(const std::string& __move) : _move(__move) {
}

const std::string& Move::move() const {
	return this->_move;
}

ChessMove::ChessMove() {
	this->_from=Position();
	this->_to=Position();
};

ChessMove::ChessMove(int player, const std::string &mv) : Move(mv) {
	int x,y;
	if(mv.size() >=4) {
		x=mv[0]-'a';
		y=mv[1]-'1';
		this->_from=Position(x,y);
		x=mv[2]-'a';
		y=mv[3]-'1';
		this->_to=Position(x,y);
		this->player=player;
	} else {
		this->_from=this->_to=Position(-1,-1);
	}
}

ChessMove::ChessMove(const Position& a, const Position& b,const int j) : Move(a.toStringNotation()+b.toStringNotation()) {
	this->_from=a;
	this->_to=b;
	this->player=j;
}

ChessMove::ChessMove(const Move &mv) : Move(mv.move()) {
	int x,y;
	std::string aux=mv.move();
	if(aux.size() >=4) {
		x=aux[0]-'a';
		y=aux[1]-'1';
		this->_from=Position(x,y);
		x=aux[2]-'a';
		y=aux[3]-'1';
		this->_to=Position(x,y);
		this->player=player;
	} else {
		this->_from=this->_to=Position(-1,-1);
	}
}

Position ChessMove::to() const {
	return this->_to;
}
Position ChessMove::from() const {
	return this->_from;
}
int ChessMove::color() const {
	return this->player;
}


