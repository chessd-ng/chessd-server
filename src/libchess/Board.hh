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

		const std::vector<Piece*>& operator[](int value);

		void createPiece(const Position& pos,Piece *pe);
		
		//TODO change the name of the three functions below
		/*Functions to get info about the Board*/
		int getType(const Position& p) const;

		int getPieceReal(const Position& p) const;

		int getColor(const Position& p) const;

		/*! \brief Mave a move, it just move pieces*/
		void makeMove(const ChessMove& j)=0;
};

class ChessBoard : public Board {
	public:
		ChessBoard(int n, int m);

		//Make a ChessMove
		void makeMove(const ChessMove &j);
	private:
};

//Recreate the Board From the FEN given
//void setState(const std::string& FEN);
//std::string Board::getPosforFEN() const;
#endif
