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

#ifndef __PECASH__
#define __PECASH__

#include "ChessMove.hh"

#include <iostream>

class Piece
{
	public:
		//Fazer Assim, dá mais flexibilidade
		enum Piece_type {
			NADA=0,REI=1,RAINHA=2,TORRE=3,BISPO=4,CAVALO=5,PEAO=6
		};
		enum Piece_color {
			BRANCAS=0,PRETAS=1,NENHUM=-1
		};
	private:
		Piece_type tipo;
		Piece_color cor;
	public:
		//funcao por motivos de testes
		void desenha() const;

		char getType() const;
		char getPieceReal() const;
		int getPlayer() const;
		Position getPos() const;
		Piece_type chartotype(char a) const;
		
		//Contrutoras
		Piece(Piece_type t,Piece_color c) {
			this->tipo=t;
			this->cor=c;
		};
		Piece() {
			this->tipo=NADA;
			this->cor=NENHUM;
		}
		Piece(char a);

		~Piece() {
		}
};

#endif
