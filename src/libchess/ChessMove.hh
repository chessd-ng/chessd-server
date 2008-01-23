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

#ifndef __JOGADAH__
#define __JOGADAH__
#include <string>
struct Position
{
	int pos[2];

	int posx() const ;
	int posy() const ;
	std::string toStringNotation() const ;

	Position(std::string& p);
	Position(int x, int y);
	Position() ; 

	//ver se n precisa copiar
	bool operator ==(const Position& p) const ;
	bool operator !=(const Position& p) const ;
	Position operator +(const Position& p) const {
		return Position(this->posx()+p.posx(),this->posy()+p.posy());
	}
};

class ChessMove
{
	private:
		Position from;
		Position to;
		int jogador;
	public:
		ChessMove();
		ChessMove(int jogador,const std::string&);
		
		ChessMove(const Position& a, const Position& b,const int j);

		Position getto() const ;
		Position getfrom() const ;
		int getPlayer() const ;
};

#endif
