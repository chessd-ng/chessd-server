#ifndef _JOGOH_
#define _JOGOH_

#include "ChessMove.hh"
#include "Board.hh"
#include "Piece.hh"
#include "State.hh"

class BoardGame {
	private:
	protected:
		virtual void updateTurn()=0;

		mutable Board* gameboard;

		int nlines,ncolums;

	public:
		BoardGame(int n,int m) : nlines(n), ncolums(m) {}

		virtual ~BoardGame() {}

		virtual int winner() const=0;

		virtual int turn() const =0;

		virtual bool verifyAndMakeMove(const std::string &move)=0;
};
#endif
