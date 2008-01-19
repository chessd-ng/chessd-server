#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include "Piece.hh"

char TabelaChar[]=".KQRBNP";
/*
char TabelaName[][7]={
	"",
	"Rei",
	"Rainha",
	"Torre",
	"Bispo",
	"Cavalo",
	"Peao"
};
*/

Piece::Piece(int __type,int __color) : _type(__type), _color(__color) {
}

ChessPiece::ChessPiece(int __type, int __color) : Piece(__type,__color) {
}
ChessPiece::ChessPiece(char name) : Piece(chartotype(name),chartocolor(name)) {
}

ChessPiece::ChessPiece() : Piece(NOTYPE,NOCOLOR) {
}

int Piece::type() const {
	return this->_type;
}

int Piece::color() const {
	return this->_color;
}

ChessPiece::PieceType ChessPiece::chartotype(char a) {
	a=toupper(a);
	switch(a) {
		case '.':
			return NOTYPE;
		case 'K':
			return KING;
		case 'Q':
			return QUEEN;
		case 'R':
			return ROOK;
		case 'B':
			return BISHOP;
		case 'N':
			return KNIGHT;
		case 'P':
			return PAWN;
	}
	return NOTYPE;
}

ChessPiece::PieceColor ChessPiece::chartocolor(char name) {
	return (name=='.')?NOCOLOR:((name>='a') and (name<='z'))?BLACK:WHITE;
}

int ChessPiece::pieceReal() const {
	return (int)(this->color() == WHITE ? (TabelaChar[this->type()]) : (tolower(TabelaChar[this->type()])));
}
