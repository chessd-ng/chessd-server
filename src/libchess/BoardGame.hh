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
