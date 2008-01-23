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

#ifndef _JOGOH_
#define _JOGOH_

#include "ChessMove.hh"
#include "Board.hh"
#include "Piece.hh"
#include "State.hh"

class BoardGame {
	private:
	protected:
		virtual void putPieces()=0;
		virtual void updateTurn()=0;
		mutable Board* gameboard;
		bool VerifyDiagonal(const ChessMove& para) const;
		bool VerifyHorizontal(const ChessMove& para) const;
		bool VerifyVertical(const ChessMove& para) const;
		int nlines,ncolums;
		int winner;

	public:
		BoardGame(int n,int m) {gameboard = new Board(n,m); nlines=n;ncolums=m;winner=-1;}
		virtual ~BoardGame() {delete gameboard;}
		virtual bool verifyCheckMate()const=0;
		virtual bool verifyDraw()const=0;
		virtual int Winner() const=0 ;
		virtual bool verifyandmakeMove(const std::string &jogada)=0;
#ifdef TESTE
		//para testes
		virtual void Desenha() const {gameboard->desenha();}
		virtual void hist()const=0;
#endif
};
#endif
