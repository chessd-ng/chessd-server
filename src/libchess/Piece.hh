#ifndef __PECASH__
#define __PECASH__

#include "ChessMove.hh"

#include <iostream>

class Piece
{
	public:
		Piece(int type,int color);
		Piece();
		virtual ~Piece() { }

		int type() const;
		int color() const;
		virtual int pieceReal() const = 0;
	private:
		int _type;
		int _color;
};

class ChessPiece : public Piece {
	public:
		enum PieceType {
			NOTYPE=0,KING=1,QUEEN=2,ROOK=3,BISHOP=4,KNIGHT=5,PAWN=6
		};
		enum PieceColor {
			NOCOLOR=-1,WHITE=0,BLACK=1
		};

		ChessPiece(int __type, int __color);		
		ChessPiece(char name);
		ChessPiece();

		static PieceType ChessPiece::chartotype(char a);
		static PieceColor ChessPiece::chartocolor(char a);
		virtual int pieceReal() const;

	private:
};

#endif
