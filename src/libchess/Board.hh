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

class Board
{
	private:
		int nlines,ncolums;
		Piece** board;
	public:
		Board(int n,int m); 
		~Board();
#ifdef DEBUG
		//para testes
		void desenha() const;
#endif

		//pega o FEN completo
		std::string getFEN() const;

		//Pega apenas a parte das posicoes das pegas do FEN
		std::string getPosforFEN() const;
		void createPiece(const Position& pos,const Piece& pe);

		//Funcoes para pegar informacoes do boardleiro
		char getType(const Position& p) const;
		char getPieceReal(const Position& p) const;
		int getPlayer(const Position& p) const;

		//essa funcao estabelece o boardleiro a partir do FEN dado
		void setState(const std::string& FEN);
		//Executa uma ChessMove
		void makeChessMove(const ChessMove& j);
};

#endif
