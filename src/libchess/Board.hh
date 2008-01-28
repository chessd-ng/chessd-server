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

#ifndef _TABULEIROH_
#define _TABULEIROH_

#include "Piece.hh"

#include <iostream>
#include <vector>

class Board
{
	private:
	protected:
		int nlines,ncolums;
		std::vector<std::vector<Piece*> > board;
	public:
		Board(int n,int m); 

		virtual ~Board();

		const std::vector<Piece*>& operator[](int value) const;

		void createPiece(const Position& pos,Piece *pe);
		
		//TODO change the name of the three functions below
		/*Functions to get info about the Board*/
		int getType(const Position& p) const;

		int getPieceReal(const Position& p) const;

		int getColor(const Position& p) const;

		/*! \brief Mave a move, it just move pieces*/
		virtual void makeMove(const ChessMove& j)=0;
};

class ChessBoard : public Board {
	public:
		ChessBoard(int n, int m);

		virtual ~ChessBoard();

		//Make a ChessMove
		virtual void makeMove(const ChessMove &j);
	private:
};

//Recreate the Board From the FEN given
//void setState(const std::string& FEN);
//std::string Board::getPosforFEN() const;
#endif
