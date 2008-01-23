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
#include <ctype.h>
#include <stdlib.h>
#include "Piece.hh"

char TabelaChar[]=".KQRBNP";
char TabelaNome[][7]={
	"",
	"Rei",
	"Rainha",
	"Torre",
	"Bispo",
	"Cavalo",
	"Peao"
};

//

Piece::Piece(char nome) {
	if(nome == '.') 
		this->cor = Piece::NENHUM;
	else if( (nome >= 'a') and (nome <= 'z') )
		this->cor=Piece::PRETAS;
	else 
		this->cor=Piece::BRANCAS;

	this->tipo = chartotype(nome);
}

Piece::Piece_type Piece::chartotype(char a) const {
	a=toupper(a);
	switch(a) {
		case '.':
			return NADA;
		case 'K':
			return REI;
		case 'Q':
			return RAINHA;
		case 'R':
			return TORRE;
		case 'B':
			return BISPO;
		case 'N':
			return CAVALO;
		case 'P':
			return PEAO;
	}
	return NADA;
}
void Piece::desenha() const {
	std::cout << (char)(this->cor == BRANCAS ? TabelaChar[this->tipo] : tolower(TabelaChar[this->tipo]) ) << " ";
}

char Piece::getPieceReal() const {
	return (char)(this->cor == BRANCAS ? (TabelaChar[this->tipo]) : ((char)tolower(TabelaChar[this->tipo])) );
}
char Piece::getType() const {
	return TabelaChar[this->tipo];
}

int Piece::getPlayer() const {
	return this->cor;
}
